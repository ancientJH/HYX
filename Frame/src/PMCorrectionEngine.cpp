#include "PMCorrectionEngine.h"
#include "UPlanCommonExport.h"
#include "CellLoader.h"
#include "NETypes.h"
#include "LosFileRecordManager.h"
#include "RadialRaySpectrum.h"
#include <algorithm>
#include "MeasuredDataParser.h"
#include "LiShuttleDtFileParser.h"
#include "LiShuttleTypes.h"
#include "CalibrationParams.h"
#include "PathLossCalcEngine.h"
#include "PropModelAnalyzer.h"
#include "MeasuredDataAnchors.h"
#include "CellPLContainer.h"
#include "MTLishuttleDTFile.h"
#include "FieldTestDigest.h"
#include "MTLishuttleDTFile.h"
#include <cassert>
#include <memory>

#define PM_CALIBRATION_MULTITHREAD

PMCorrectionEngine::PMCorrectionEngine(IServiceHelper* pAppHelper) :\
	AppEngine(pAppHelper, PM_CALIBRATION_SERVICE), m_taskId(""), m_pCorCalculator(NULL), m_pPMCorrData(NULL)
{
}

PMCorrectionEngine::~PMCorrectionEngine()
{
	if (NULL != m_pCorCalculator) {
		delete m_pCorCalculator;
		m_pCorCalculator = NULL;
	}

	if (m_pPMCorrData) {
		delete m_pPMCorrData;
		m_pPMCorrData = NULL;
	}
}

const char* PMCorrectionEngine::GetResultFolder() const
{
	return m_calcParams.calibrationDirectory.c_str();
}

bool PMCorrectionEngine::Init()
{
	if (!SetupCalcParams(&m_calcParams, m_pServiceHelper->GetRuntimeSchema(), \
		m_pServiceHelper->GetCommonParams())) {
		ULog_Write(ULOG_ERROR, "Error occurs when parse the xml file for propagation calibaration!");
		return false;
	}

	AntCalcGain* pAntCalc = (AntCalcGain*)(m_pServiceHelper->GetService(ANTENNA_CALC_SERVICE));
	CellLoader* pCellLoader = (CellLoader*)(m_pServiceHelper->GetService(BS_ENGINEERING_PARAMS_SERVICE));
		
	if (m_pCorCalculator)
		delete m_pCorCalculator;
	m_pCorCalculator = new CorrectCalculator();
	m_pCorCalculator->Init(m_calcParams, pAntCalc, pCellLoader);//校正计算器
	
	//获取路测文件名称
	GetAllFiles(m_calcParams.dtFileDictionaryPath.c_str());

	//检查相关设置项
	if (m_calcParams.minPower > m_calcParams.maxPower) {
		ULog_Write(ULOG_ERROR, "The settings of power's limit are not right");//值上下限错误
		return false;
	}

	return true;
}

void PMCorrectionEngine::Run(const char* taskId)
{
	ULog_Write(ULOG_INFO, "Propagation model calibration begins(Engine %s).", m_taskId.c_str());

	m_taskId = taskId;

	//初始化校正模块
	if (!Init())
		return;

	IGeoInfo* pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));
	AntCalcGain* pAntGain = (AntCalcGain*)(m_pServiceHelper->GetService(ANTENNA_CALC_SERVICE));
	CellLoader* pCellLoader = (CellLoader*)(m_pServiceHelper->GetService(BS_ENGINEERING_PARAMS_SERVICE));
	PathLossCalcEngine* pPLCalcEngine = dynamic_cast<PathLossCalcEngine*>(m_pServiceHelper->GetAppEngine(PATHLOSS_CALC_SERVICE));
	if (!pPLCalcEngine || !pPLCalcEngine->Init()) {
		ULog_Write(ULOG_ERROR, "Can't initialize the pathloss calc service!");
		return;
	}
	
	//初始化校正数据集
	if (m_pPMCorrData)
		delete m_pPMCorrData;
	CellPLContainer plContainer(pPLCalcEngine);
	m_pPMCorrData = new PMCorrectionData(m_calcParams, &plContainer, pAntGain, pGeoInfo);

	//1. 读取路测数据、预装载ray文件、初步分析
	GeoCoordinate cordTrans;
	cordTrans.SetProjectionParams(pGeoInfo->getProjParams());

	IMeasurementDataParser* pDataParser = new MeasuredDataParser(pCellLoader, &cordTrans);

	MeasurementFileParser* pDtFileParser = nullptr;
	
#ifndef PM_CALIBRATION_MULTITHREAD
	pDtFileParser = dynamic_cast<MeasurementFileParser*>(\
		new LiShuttleDtFileParser(pDataParser, m_calcParams.rsrpOffset));
	pDtFileParser->AddVisitor(dynamic_cast<IMeasuredDataVisitor*>(m_pPMCorrData));
#else
	pDtFileParser = dynamic_cast<MTLishuttleDTFile*>(\
		new MTLishuttleDTFile(m_calcParams, pDataParser, &plContainer, pAntGain, pGeoInfo, pCellLoader, \
			m_calcParams.rsrpOffset, m_calcParams.threadCount));
	dynamic_cast<MTLishuttleDTFile*>(pDtFileParser)->SetPMCorrData(m_pPMCorrData);
#endif

	//加载路测数据
	std::string filePath;
	FileOperator dtFileOperator(m_calcParams.dtFileDictionaryPath.c_str());
	for (std::string dtFile : m_dtDataFiles) {
		char* fullpath;
		if (dtFileOperator.GetFullPath(&fullpath, dtFile.c_str())) {
			if (!IsFileExist(fullpath)) {
				ULog_Write(ULOG_INFO, "%s doesn't exist!", fullpath);
				continue;
			}
			dtFileOperator.FreePath(fullpath);
		}

		bool ret = dtFileOperator.LoadFile(dtFile.c_str(), pDtFileParser);
		if (ret)
			ULog_Write(ULOG_INFO, "Load file: %s.", dtFile.c_str());
		else
			ULog_Write(ULOG_INFO, "Failed to load file: %s.", dtFile.c_str());
	}
	pDtFileParser->PostProcess();
	
	delete pDtFileParser;
	delete pDataParser;
	
	//2. 计算实际路径损耗加权值，并读取射线谱数据并装载，仅装载有路测数据的射线点
	m_pPMCorrData->CalcRealPathLoss(true);
	//m_pPMCorrData->MountRaySpectrum();

	//3. 数据过滤 滤除背向、无主径、落在室内的栅格点
	m_pPMCorrData->FilterGridData();
	
	m_pPMCorrData->PrintProcessedInfo();

	//4. 数据分析
	PropModelAnalyzer* pPropModelAna = dynamic_cast<PropModelAnalyzer*>(\
			(ICellParamsAnalyzer*)(m_pServiceHelper->GetService(PROPAGATION_MODEL_ANALYZER)));

	const std::map<int, std::list<CellCorrectionData*>>& siteCellMap = m_pPMCorrData->GetCellsBySite();
	pPropModelAna->UpdateAnalyzeData(siteCellMap, LiShuttlePropagationModel::GetCopyofDefaultSub6GModel(), pAntGain, m_calcParams.threadCount);

	pPropModelAna->Save();

	//更新小区路损的大尺度偏离项
	pCellLoader->CalibrateCellParams(pPropModelAna);	

	//5. 校正
	ULog_Write(ULOG_INFO, "The calibration process begins");

	m_pCorCalculator->Run(m_pPMCorrData);
	m_pCorCalculator->PrintProcessedInfo();
	
	ULog_Write(ULOG_INFO, "The calibration process ends");

	//6. 输出经校正整理后的栅格路测数据
	MeasuredDataAnchors measAnchors;
	m_pPMCorrData->CreateMeasAnchors(&measAnchors);
	dtFileOperator.SaveFile(FILENAME_OF_DTANCHOR, dynamic_cast<IDataSerialize*>(&measAnchors));
	if (m_calcParams.isUpdateDtFiles)
		dtFileOperator.SaveFile(FILENAME_OF_CLEANGRID, dynamic_cast<IDataSerialize*>(m_pPMCorrData));

	//7. 保存模型结果
	SaveModels();
	SaveCellMeanVariance();

	//输出小区级的校正总结报告
	ExportCellSummary();

	//输出场景模型的校正总结报告
	if (m_calcParams.isCalScene)
		ExportSceneSummary();

#ifdef INNERCLASS_TEST
	//数据导出
	/*ExportAllRayStrength();
	ExportProcessData();
	ExportAllGridData();*/
#endif

	ULog_Write(ULOG_INFO, "Propagation model calibration ends (Engine %s).", m_taskId.c_str());
}

void PMCorrectionEngine::ExportCellSummary() const
{
	std::string schemaName = m_pServiceHelper->GetCommonParams()->schemaName;
	std::string cellFeatureFile = m_calcParams.dtFileDictionaryPath + PathSplit;
	cellFeatureFile += m_pServiceHelper->GetCommonParams()->schemaName + "_";
	cellFeatureFile.append(FILENAME_OF_CALIBARATION_CELL);
	m_pPMCorrData->SerializeCellFeatures(cellFeatureFile.c_str());
}

void PMCorrectionEngine::ExportSceneSummary() const
{
	//输出场景模型结果
	std::string sceneFeatureFile = m_calcParams.dtFileDictionaryPath + PathSplit;
	sceneFeatureFile += m_pServiceHelper->GetCommonParams()->schemaName + "_"; 
	sceneFeatureFile.append(FILENAME_OF_CALIBARATION_SCENE);
	
	std::string features = "sceneNo," + LiShuttlePropagationModel::GetFeatureTitle();
	features.append("\n");
	char buf[256];
	const LiShuttleSceneModelMap* modelMap = m_pCorCalculator->GetSceneModels();
	for (auto scene : *modelMap) {
		upl_snprintf(buf, 256, "%d,%s\n", scene.first, scene.second->GetFeatures().c_str());
		features.append(buf);
	}
	WriteToFile(sceneFeatureFile.c_str(), "w", features.c_str(), features.size());//输出场景模型结果
}

void PMCorrectionEngine::ExportProcessData() const
{
	fstream f_dtData;
	f_dtData.open("ProcessData.csv", ios::out | ios::trunc);//写文件 清空源文件
	f_dtData << "cellID" << "," << "cellX" << "," << "cellY" << "," << "index" << "," << "pathloss" << endl;
	const std::map<std::string, CellCorrectionData*>& cellData = m_pPMCorrData->GetCellCorrData();
	auto iter = cellData.begin();
	std::string cellID = "";
	double cellX, cellY, pathloss;
	int index;
	for (; iter != cellData.end(); iter++)
	{
		cellID = iter->second->GetTrasceiver()->transID;
		cellX = iter->second->GetTrasceiver()->X;
		cellY = iter->second->GetTrasceiver()->Y;
		auto gridData = iter->second->GetGrid_Map();
		auto iter2 = gridData->begin();
		for (; iter2 != gridData->end(); iter2++)
		{
			index = iter2->first;
			pathloss = iter2->second->GetRealPathLoss();
			f_dtData << cellID << "," << cellX << "," << cellY << "," << index << "," << pathloss << endl;
		}
	}
}

void PMCorrectionEngine::FilterGridData()
{
	ULog_Write(ULOG_INFO, "begin filter back grid");
	std::map<std::string, CellCorrectionData*>& cellCorData = m_pPMCorrData->GetCellCorrData();
	auto iter = cellCorData.begin();
	for (; iter != cellCorData.end();)
	{
		AntInstParam antInstParam = iter->second->GetTrasceiver()->antInstParams.front();
		auto cellData = iter->second;
		cellData->Filter();
		if (cellData->GetGrid_Map()->empty())
		{
			ULog_Write(ULOG_INFO, "cell %s is filtered", iter->first.c_str());
			iter = cellCorData.erase(iter);
			continue;
		}
		iter++;
	}
	ULog_Write(ULOG_INFO, "end filter back grid,the count of cell after filtering:%d", cellCorData.size());
}

void PMCorrectionEngine::SaveCommonModel()
{
	auto generalModel = m_pCorCalculator->GetGeneralModel();
	if (generalModel == nullptr)
	{
		ULog_Write(ULOG_FATAL, "no general model to be written!");
		return;
	}
	
	IGeoInfo* pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));
	ICellParamsAnalyzer* pCellParams = (ICellParamsAnalyzer*)(m_pServiceHelper->GetService(PROPAGATION_MODEL_ANALYZER));
	if (pGeoInfo && pCellParams) {
		generalModel->DlExtraLoss = pCellParams->GetCityExtraLoss(pGeoInfo->GetMapName());
	}

	generalModel->ModelName = m_calcParams.corModelName;
	std::shared_ptr<PropagationModel> model = \
		PropagationModelFactory::instance()->InsertModel(generalModel, false);
	if (model) {
		ULog_Write(ULOG_INFO, "The saved common model is \"%s_%d\".", model->GetCategoryName(), \
			model->PropModelID);
	}
	else {
		ULog_Write(ULOG_INFO, "Failed to save the common model!");
	}
}

size_t PMCorrectionEngine::GetModelDirectory(char* szModelDir, size_t len)
{
	char processName[256];
	if (get_executable_path(szModelDir, processName, len) < 0)
	{
		return -1;
	}
	size_t dirLen = upl_strlen(szModelDir);
	size_t configLen = upl_strlen(PL_PROPAGATION_MODEL_DIR);
	if (dirLen + configLen + 2 > len)
	{
		return -1;
	}
	if (szModelDir[dirLen - 1] != PathSplitChar) {
		szModelDir[dirLen] = PathSplitChar;
		dirLen++;
	}
	upl_memcpy(szModelDir + dirLen, PL_PROPAGATION_MODEL_DIR, configLen);
	szModelDir[dirLen + configLen] = '\0';
	return dirLen + configLen;
}

bool PMCorrectionEngine::IsNeededAddModel(const std::list<std::shared_ptr<PropagationModel>>& models, std::shared_ptr<LiShuttlePropagationModel> model) const
{
	bool isNeeded = true;
	for (std::shared_ptr<PropagationModel> temp : models)
	{
		auto tempModel = std::dynamic_pointer_cast<LiShuttlePropagationModel>(temp);
		isNeeded = !(IsModelEqual(tempModel, model));
		if (!isNeeded)
		{
			break;
		}
	}
	return isNeeded;
}

bool PMCorrectionEngine::IsModelEqual(std::shared_ptr<LiShuttlePropagationModel> model1, std::shared_ptr<LiShuttlePropagationModel> model2) const
{
	double error = 0.00001;
	if (fabs(model1->k0 - model2->k0) > error)
		return false;
	if (fabs(model1->knear - model2->knear) > error)
		return false;
	if (fabs(model1->kfar - model2->kfar) > error)
		return false;
	if (fabs(model1->DiffractionCoeff - model2->DiffractionCoeff) > error)
		return false;

	return true;
}

void PMCorrectionEngine::SaveModels() 
{
	//保存本次校正的通用模型
	SaveCommonModel();

	//保存一站一模
	//if (m_pCorCalculator->IsCalExcu()) {
		SaveCellModels();
	//}
	
	//保存场景模型
	if (m_pCorCalculator->IsCalScene()) {
		auto SceneModels = dynamic_pointer_cast<LiShuttleSceneModel>(\
			PropagationModelFactory::instance()->GetSceneModelSet());
		SceneModels->InsertModels(m_pCorCalculator->GetSceneModels());
	}
}

void PMCorrectionEngine::SaveCellModels()
{
	//保存通用模型
	std::shared_ptr<LiShuttlePropagationModel> generalModel = m_pCorCalculator->GetGeneralModel();
	if (!generalModel)
	{
		ULog_Write(ULOG_ERROR, "Failed to get the common model!");
		return;
	}

	//一站一模模型保存
	std::shared_ptr<LiShuttleCellModel> cellModelSet = std::dynamic_pointer_cast<LiShuttleCellModel>(
		PropagationModelFactory::instance()->GetCellModelSet());

	if (cellModelSet)
		cellModelSet->InsertModels(generalModel, m_pCorCalculator->GetCorModels());
}

void PMCorrectionEngine::SaveCellMeanVariance()
{
	char fileName[512];
	strcpy(fileName, m_calcParams.calibrationDirectory.c_str());
	int length = upl_strlen(fileName);
	if ((fileName[length - 1] != '/') && (fileName[length - 1] != '\\'))
	{
		fileName[length] = (PathSplitChar);
		length++;
	}
	char name[] = "CorrectionMeanVariance.xml";//文件名
	int length1 = upl_strlen(name);
	upl_strcpy(fileName + length, 512 - length, name);
	fileName[length + length1] = '\0';
	tinyxml2::XMLDocument doc;
	XMLDeclaration* dec = doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
	doc.InsertFirstChild(dec);

	double means[2], stdevs[2];
#define GET_MEANS_AND_STDEVS(model) do{ \
		int num = model->means.size(); \
		means[0] =  (num > 0) ? (model)->means.front() : 0; \
		means[1] =  (num > 1) ? (model)->means.back() : 0; \
		num = model->stdevs.size(); \
		stdevs[0] =  (num > 0) ? (model)->stdevs.front() : 0; \
		stdevs[1] =  (num > 1) ? (model)->stdevs.back() : 0; \
	}while(0)

#define INSERT_NEW_MODEL_INDICATORS(_id, _name) do{\
		XMLElement* newModel = doc.NewElement("Model"); \
		newModel->SetAttribute("ID", (_id)); \
		XMLElement* modelname = doc.NewElement("ModelName"); \
		modelname->SetText(_name); \
		XMLElement* oriMean = doc.NewElement("oriMean"); \
		oriMean->SetText(means[0]); \
		XMLElement* oriVariance = doc.NewElement("oriVariance"); \
		oriVariance->SetText(stdevs[0]); \
		XMLElement* corMean = doc.NewElement("corMean"); \
		corMean->SetText(means[1]); \
		XMLElement* corVariance = doc.NewElement("corVariance"); \
		corVariance->SetText(stdevs[1]); \
		newModel->InsertEndChild(modelname); \
		newModel->InsertEndChild(oriMean); \
		newModel->InsertEndChild(oriVariance); \
		newModel->InsertEndChild(corMean); \
		newModel->InsertEndChild(corVariance); \
		doc.InsertEndChild(newModel); \
	}while(0)

	//保存通用模型
	auto generalModel = m_pCorCalculator->GetGeneralModel();
	if (generalModel == nullptr)
	{
		//ULog_Write(ULOG_ERROR, "General Model is not exist!!  Correct failed");
		return;
	}
	GET_MEANS_AND_STDEVS(generalModel);
	INSERT_NEW_MODEL_INDICATORS(0, m_calcParams.corModelName.c_str());

	//保存一站一模
	const std::map<std::string, std::shared_ptr<LiShuttlePropagationModel>>& corModels = m_pCorCalculator->GetCorModels();
	if (corModels.size() == 0)
	{
		return;
	}
	auto modelIter = corModels.begin();
	int modelId = 1;
	for (const std::pair<std::string, std::shared_ptr<LiShuttlePropagationModel>> corModel : corModels)
	{
		if (corModel.second == nullptr)
		{
			ULog_Write(ULOG_FATAL, "cell:%s correction has failed!!", corModel.first.c_str());
			continue;
		}
		GET_MEANS_AND_STDEVS(corModel.second);
		INSERT_NEW_MODEL_INDICATORS(modelId, corModel.first.c_str());

		++modelId;
	}
	
	doc.SaveFile(fileName);
}

void PMCorrectionEngine::GetPathLossModelName(char* fileName, size_t len)
{
	/*std::string path = "/home/zhangjiarui/.vs/case";
	strcpy(fileName, path.c_str());*/
	strcpy(fileName, m_calcParams.calibrationDirectory.c_str());
	int length = upl_strlen(fileName);
	if ((fileName[length - 1] != '/') && (fileName[length - 1] != '\\'))
	{
		fileName[length] = (PathSplitChar);
		length++;
	}
	char name[] = "CorrectionModels.xml";//文件名
	int length1 = upl_strlen(name);
	upl_strcpy(fileName + length, 512 - length, name);
	fileName[length + length1] = '\0';
}

void PMCorrectionEngine::GetAllFiles(const char* dtDir)
{
	GetDtFiles(&m_dtDataFiles, dtDir);

	//Dir dir(dtDir);
	//string p;
	//string filename = "index.txt";
	//string indexPath = p.assign(dtDir).append("/index.txt");

	/*if (dir.find(filename.c_str()))
	{
		ifstream infile;
		infile.open(indexPath.c_str(), ios::in);
		assert(infile.is_open());
		string s;
		while (!infile.eof())
		{
			getline(infile, s);
			m_dtDataFiles.push_back(s);
		}

	}
	else
	{
		ULog_Write(ULOG_ERROR, "dtDataFile is not found!!!");
	}*/
}

void PMCorrectionEngine::ExportAllRayStrength() const
{
	//得到传播模型LiModel
	auto propModelFactory = PropagationModelFactory::instance();
	std::map<std::string, CellCorrectionData*>& cellCorrData = m_pPMCorrData->GetCellCorrData();
	auto cellData = cellCorrData.begin();
	PropModelConfig& propagationConfig = cellData->second->GetTrasceiver()->cells.front()->propModels.front();
	auto tempIter = propModelFactory->GetPropagationModel(propagationConfig.HashValue(), \
		cellData->second->GetTrasceiver()->cells.front());
	if (tempIter->IsModelSet)
		return;

	auto LiModel = std::dynamic_pointer_cast<LiShuttlePropagationModel>(tempIter);//初始LiModl
	LiShuttleModelParams modelParam;
	fstream f_Grid;
	f_Grid.open("AllStrengths.csv", ios::out | ios::trunc);//写文件 清空源文件
	double azimuth = 0, elevation = 0, antGain = 0, antStrength = 0;
	std::list<double> strengths;
	double strength;
	std::map<std::string, CellCorrectionData*>& cellMap = m_pPMCorrData->GetCellCorrData();
	AntCalcGain* pAntGain = (AntCalcGain*)(m_pServiceHelper->GetService(ANTENNA_CALC_SERVICE));
	for (auto iter = cellMap.begin(); iter != cellMap.end(); iter++)//遍历小区
	{
		AntInstParam antInstParam = iter->second->GetTrasceiver()->antInstParams.front();
		auto gridMap = iter->second->GetGrid_Map();
		auto iter2 = gridMap->begin();
		for (; iter2 != gridMap->end(); iter2++)//遍历栅格
		{
			strengths.clear();
			auto mainRay = iter2->second->GetMainRay();
			if (mainRay == nullptr) continue;
			//遍历其他径路损
			for (std::shared_ptr<RayInfo> ray : iter2->second->GetAllRays())
			{
				if (ray->bMainRay)
				{
					continue;
				}
				LiModel->GetModelParams(modelParam, ray.get());
				strength = LiShuttlePropagationCalculator::GetOnePathStrength(ray.get(), modelParam);
				antGain = pAntGain->Calcuate3DGain(antInstParam, ray->Azimuth, ray->Elevation);
				antStrength = dBto(antGain * 0.5);
				strength *= antStrength;
				strengths.push_back(strength);
			}
			strengths.sort();
			strengths.reverse();

			LiModel->GetModelParams(modelParam, mainRay.get());
			strength = LiShuttlePropagationCalculator::GetOnePathStrength(mainRay.get(), modelParam);
			antGain = pAntGain->Calcuate3DGain(antInstParam, mainRay->Azimuth, mainRay->Elevation);
			antStrength = dBto(antGain * 0.5);
			strength *= antStrength;
			strengths.push_front(strength);
			for (auto s : strengths)
			{
				f_Grid << s << ",";
			}
			f_Grid << endl;
		}
	}
	f_Grid.close();
}