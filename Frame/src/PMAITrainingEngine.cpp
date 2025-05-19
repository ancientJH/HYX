#include "PMAITrainingEngine.h"
#include "LiShuttleAICalculator.h"
#include "UPlanCommonExport.h"
#include "CellLoader.h"
#include "PropagationFunc.h"
#include "MeasuredDataParser.h"
#include "GeoCoordinate.h"
#include "CommaFileParser.h"
#include "SwanUtility.h"
#include "LiShuttleDtFileParser.h"
#include <cassert>

void PMAITrainingEngine::GetAllFiles(std::string pathDir,std::vector<std::string>& files)
{
	Dir dir(pathDir.c_str());
	string p;
	string filename = "index.txt";
	string indexPath = p.assign(pathDir).append("\/index.txt");

	StreamFile infile;
	u32 open = infile.open(indexPath.c_str(), "rb");

	ITextFileParser* pFileParser = new CommaFileParser();
	pFileParser->parseStreamFile(&infile);

	ITextLineParser* pTextLine;

	while (NULL != (pTextLine = pFileParser->getNextLine())) {
		string fileName = pTextLine->getField(0);
		files.push_back(fileName);
	}
}

PMAITrainingEngine::PMAITrainingEngine(IServiceHelper* pAppHelper) :\
	AppEngine(pAppHelper, PM_AI_SERVICE), m_pCalcParamFactory(NULL), m_taskId(""), \
	m_pCalculator(NULL), m_pAIGridFeatureProbe(NULL)
{
}

PMAITrainingEngine::~PMAITrainingEngine()
{
	if (m_pAIGridFeatureProbe) {
		delete m_pAIGridFeatureProbe;
		m_pAIGridFeatureProbe = NULL;
	}

	if (m_pCalculator) {
		delete m_pCalculator;
		m_pCalculator = NULL;
	}
		
	if (NULL != m_pCalcParamFactory)
	{
		delete m_pCalcParamFactory;
		m_pCalcParamFactory = NULL;
	}
}

const char* PMAITrainingEngine::GetResultFolder() const
{
	return m_calcParams.pathlossDirectory.c_str();
}

void PMAITrainingEngine::Run(const char* taskId)
{
	m_taskId = taskId;

	if (!Init())
		return;

	CreateFeature();

#define RUNTIMEFLAG_LEN 256
	char runtimeFlag[RUNTIMEFLAG_LEN] = { 0 };
	std::string folder = m_pServiceHelper->GetAppFolder();
	if (folder.back() != '\\' && folder.back() != '/')
		folder.append(PathSplit);
	folder.append("pyAndsh");
	folder.append(PathSplit);
	std::string commandStr = "bash ";
	commandStr.append(folder);
	commandStr.append("activate.sh ");
	commandStr.append(m_calcParams.featurePath);
	commandStr.append(" ");
	commandStr.append(folder);
	commandStr.append(m_calcParams.pythonSchema);
	ULog_Write(ULOG_INFO, "Training command : %s", commandStr.c_str());
	executeCMD(commandStr.c_str(), runtimeFlag, RUNTIMEFLAG_LEN);

	ULog_Write(ULOG_INFO, "runtimeFlag = %s", runtimeFlag);
	if (strcmp(runtimeFlag, "1") == 0) {

		ULog_Write(ULOG_INFO, "Finished to run the ai task");
	}
	else
	{
		ULog_Write(ULOG_INFO, "Failed to Run the ai task failed");
	}
}

bool PMAITrainingEngine::Init()
{
	if (!SetupPMAIParams(&m_calcParams, m_pServiceHelper->GetRuntimeSchema(), \
		m_pServiceHelper->GetCommonParams(), m_pServiceHelper->GetAppFolder())) {
		ULog_Write(ULOG_ERROR, "Error occurs when parse the xml file for propagation ai training!");
		return false;
	}

	IGeoInfo* pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));
	AntCalcGain* pAntCalc = (AntCalcGain*)(m_pServiceHelper->GetService(ANTENNA_CALC_SERVICE));

	if (m_pCalcParamFactory) {
		delete m_pCalcParamFactory;
		m_pCalcParamFactory = NULL;
	}
	m_pCalcParamFactory = new CalcParamFactory(m_calcParams, \
		(ISceneInfo*)(m_pServiceHelper->GetService(SCENEINFO_COLLECTION_SERVICE)), pGeoInfo->GetMapName());
	m_pCalcParamFactory->SetAntLibrary(pAntCalc->m_pAntLibrary);

	if (m_pCalculator)
		delete m_pCalculator;
	m_pCalculator = new LiShuttleAICalculator();
	m_pCalculator->m_pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));
	m_pCalculator->m_pAntCalcGain = pAntCalc;

	if (m_pAIGridFeatureProbe)
		delete m_pAIGridFeatureProbe;
	m_pAIGridFeatureProbe = new AIGridFeatureProbe((LiShuttleAICalculator*)m_pCalculator);

	TransceiverList transceiverList;
	((CellLoader*)(m_pServiceHelper->GetService(BS_ENGINEERING_PARAMS_SERVICE)))->GetCells(\
		& transceiverList, m_calcParams.transFilter.c_str());
	for (auto tran : transceiverList) {
		if (tran->antInstParams.size() == 0) {
			ULog_Write(ULOG_ERROR, "Transciver(%s) has no valid antennas.", tran->transID.c_str());
			continue;
		}

		auto antCalcParam = m_pCalcParamFactory->GetAntCalcParam(&(tran->antInstParams.front()), tran->cells);

		if (!antCalcParam) {
			ULog_Write(ULOG_ERROR, "The parameters of transciver(%s) are invalid.", tran->transID.c_str());
			continue;
		}

		m_pAIGridFeatureProbe->SetCellParams(antCalcParam);
	}

	return true;
}

void PMAITrainingEngine::GetFullPathFiles(std::string& fullPath, const char* relativePath) const
{
	fullPath = m_calcParams.featurePath;
	if ((fullPath[fullPath.length() - 1] != '\\') && (fullPath[fullPath.length() - 1] != '/')) {
		fullPath.append(PathSplit);
	}
	fullPath.append(relativePath);
}

void PMAITrainingEngine::CreateFeature()
{
	if (!m_pAIGridFeatureProbe || !m_pCalculator) {
		ULog_Write(ULOG_FATAL, "Invalid Pointers!");
		return;
	}

	ULog_Write(ULOG_INFO, "Begin to create feature matrices(Engine %s).", m_taskId.c_str());

	//读取路测数据
	GeoCoordinate cordTrans;
	cordTrans.SetProjectionParams(((IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME)))->getProjParams());

	IMeasurementDataParser* pDataParser = new MeasuredDataParser(\
		(CellLoader*)(m_pServiceHelper->GetService(BS_ENGINEERING_PARAMS_SERVICE)), &cordTrans);
	MeasurementFileParser* pDtFileParser = new LiShuttleDtFileParser(pDataParser);
	pDtFileParser->AddVisitor(m_pAIGridFeatureProbe);
	
	FileOperator* pDtFileOperator = new FileOperator(m_calcParams.MDTpath.c_str());

	std::vector<std::string> files;
	GetAllFiles(m_calcParams.MDTpath, files);
	ULog_Write(ULOG_INFO, "count of files: %d", files.size());
	for (auto file : files) {
		pDtFileOperator->LoadFile(file.c_str(), pDtFileParser);
		PrintMemInfo();
	}

	delete pDtFileParser;
	delete pDataParser;

	//输出特征数据
	SaveAIFeature();
	ULog_Write(ULOG_INFO, "End to create feature matrices(Engine %s).", m_taskId.c_str());
}

const char* PMAITrainingEngine::GetFeaturePath() const
{
	return m_calcParams.featurePath.c_str();
}

const char* PMAITrainingEngine::GetPythonSchema() const
{
	return m_calcParams.pythonSchema.c_str();
}

void PMAITrainingEngine::SaveAIFeature() {
	if (!m_pAIGridFeatureProbe) {
		ULog_Write(ULOG_ERROR, "AIGridFeatureProbe shoule not be null");
		return;
	}
	FileOperator* fileOperator = new FileOperator(m_calcParams.featurePath.c_str());

	//保存栅格特征 一个小区一个文件 同时计算小区统计特征
	auto cellAIGridFeatures = m_pAIGridFeatureProbe->GetCellAIGridFeatures();
	for (auto cellAIGridFeature : cellAIGridFeatures)
	{
		if (cellAIGridFeature.second) {
			if (cellAIGridFeature.second->GetMDTsum() == 0) continue;
			std::string relativePath = cellAIGridFeature.first + ".csv";
			cellAIGridFeature.second->StatFeatures();
			fileOperator->SaveFile(relativePath.c_str(), cellAIGridFeature.second);
			ULog_Write(ULOG_INFO, "End to save grid feature file cell:%s", cellAIGridFeature.first.c_str());
		}
	}

	//保存小区特征，一行代表一个小区
	std::string relativePath = "indexFeature.csv";
	fileOperator->SaveFile(relativePath.c_str(), m_pAIGridFeatureProbe);

	delete fileOperator;
}

//void PMAITrainingEngine::CalculateRangeCount() 
//{
//	if (!m_pAIGridFeatureProbe) {
//		ULog_Write(ULOG_ERROR, "AIGridFeatureProbe shoule not be null");
//		return;
//	}
//	auto cellAIGridFeatures = m_pAIGridFeatureProbe->GetCellAIGridFeatures();
//	for (auto cellAIGridFeature : cellAIGridFeatures)
//	{
//		if (cellAIGridFeature.second) {
//			cellAIGridFeature.second->RangeCount();
//		}
//	}
//}
