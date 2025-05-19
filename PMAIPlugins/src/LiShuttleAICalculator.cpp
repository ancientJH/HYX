#include "LiShuttleAICalculator.h"
#include "SamplingGridFeature.h"
#include "GeoInfo.h"
#include "AntCalcGain.h"
#include "CellularType.h"
#include "PropagationFunc.h"
#include "IClutterParamsManager.h"
#include "RadialPathlossData.h"
#include "CellAIGridFeature.h"
#include "SectionPathlossData.h"
#include "LishuttleCtl.h"
#include <UPlanCommonExport.h>

#ifdef USINGPYTHON
#include <Python.h>
#endif

void executeCMD(const char* cmd, float** result, std::shared_ptr<SectionPathLossCalcParam> sectionParam)
{
#define BUF_PS_SIZE 2048
	char buf_ps[BUF_PS_SIZE];
	int lineNumber = sectionParam->GetScanningLineNumber();
	int startIndex = sectionParam->GetStartPointIndex();
	int endIndex = sectionParam->GetEndPointIndex();
	int i = 0;
	int j = 0;
	int k = startIndex;
	

	ULog_Write(ULOG_INFO, "Execute the command (%s)", cmd);
	CmdExec cmdExecutor;
	if (!cmdExecutor.command_line_sync(cmd)) {
		ULog_Write(ULOG_FATAL, "Failed to execute the command(%s)!", cmd);
		return;
	}

	ULog_Write(ULOG_INFO, "Succed to execute the command(%s)!", cmd);
	while (cmdExecutor.gets(buf_ps, BUF_PS_SIZE) != NULL){
		if (j >= lineNumber) {
			ULog_Write(ULOG_ERROR, "Error lineNumber (%d)!", j);
			return;
		}
		

		ULog_Write(ULOG_INFO, "i(%d)=%s", i, buf_ps);
		if (i < 3) {
			i++;
			continue;
		}
		
		char* digitStr = buf_ps;
		while (*digitStr == '[' || *digitStr == ' ')
			++digitStr;
		result[j][k] = atof(digitStr);
		if (k < endIndex) {
			k++;
		}
		else {
			k = startIndex;
			j++;
		}
		i++;
	}

	ULog_Write(ULOG_INFO, "End to execute the command(%s)!", cmd);
}

//void executeCMD(const char* cmd, float** result, std::shared_ptr<SectionPathLossCalcParam> sectionParam)
//{
//	char buf_ps[2048];
//	int lineNumber = sectionParam->GetScanningLineNumber();
//	int startIndex = sectionParam->GetStartPointIndex();
//	int endIndex = sectionParam->GetEndPointIndex();
//	int i = 0;
//	int j = 0;
//	int k = startIndex;
//	string output[1000000];
//	char ps[2048] = { 0 };
//	FILE* ptr;
//	strcpy(ps, cmd);
//	//map<int, float>outlier;
//	ULog_Write(ULOG_INFO, "open file");
//	if ((ptr = _popen(ps, "r")) != NULL)
//	{
//
//		cout << "open file" << endl;
//		while (fgets(buf_ps, 2048, ptr) != NULL)
//		{
//			if (j >= lineNumber) {
//				ULog_Write(ULOG_ERROR, "Error lineNumber!");
//				return;
//			}
//			//output获取每一个print的值
//			output[i] = buf_ps;
//			cout << i << " = " << output[i];
//			/*if (i > 0)
//				strcat(result, buf_ps);*/
//
//				//result = NULL;
//			if (i < 3) {
//				i++;
//				continue;
//			}
//			while (output[i].front() == '[' || output[i].front() == ' ') {
//				output[i].erase(0, 1);
//			}
//			result[j][k] = atof(output[i].c_str());
//			/*if (result[j][k] >= 120) {
//				outlier.insert(make_pair(i, result[j][k]));
//			}*/
//			if (k < endIndex) {
//				k++;
//			}
//			else {
//				k = startIndex;
//				j++;
//			}
//			i++;
//
//			/*strcat(result, buf_ps);
//			i++;
//			if (strlen(result) > 2048)
//				break;*/
//				// 具体了解数据
//
//		}
//		cout << "end file" << endl;
//
//		/*for (int i = 0; i < outlier.size(); i++) {
//			cout << i << " = " << outlier[i] << endl;
//		}*/
//
//		_pclose(ptr);
//		ptr = NULL;
//		ULog_Write(ULOG_INFO, "end file");
//	}
//	else
//	{
//		printf("popen %s error\n", ps);
//	}
//}

void executeCMD(const char* cmd, char* result, int len)
{
#define BUF_PS_SIZE 2048
	if (len < 1) {
		ULog_Write(ULOG_FATAL, "The command (%s) failed!", cmd);
		return;
	}
	CmdExec cmdExecutor;
	char buf_ps[BUF_PS_SIZE];
	int i = 0;

	ULog_Write(ULOG_INFO, "open file");
	FILE* stdOut = cmdExecutor.command_line_sync(cmd);
	if (!stdOut) {
		ULog_Write(ULOG_FATAL, "The command (%s) failed!", cmd);
		return;
	}

	ULog_Write(ULOG_INFO, "The command (%s) succed!", cmd);
	char* lastLine = NULL;
	//int tmp1 = feof(stdOut);
	//int tmp2 = ferror(stdOut);

	//while (!feof(stdOut) && !ferror(stdOut)) {
		while (cmdExecutor.gets(buf_ps, BUF_PS_SIZE)) {
			ULog_Write(ULOG_INFO, "%d=%s", i, buf_ps);
			//cout << i << " = " << buf_ps;
			lastLine = buf_ps;
			i++;
		}
	//}
	if (lastLine)
		upl_strcpy(result, len - 1, lastLine);
	else
		result[0] = '\0';
	size_t resLen = upl_strlen(result);
	if ( (resLen > 1) && (result[resLen - 1] == '\n') ) {
		result[resLen - 1] = '\0';
	}
	ULog_Write(ULOG_INFO, "End of the commond (%s)!", cmd);
}

//void executeCMD(const char* cmd, char* result)
//{
//	char buf_ps[2048];
//	int i = 0;
//	string output[4096];
//	char ps[2048] = { 0 };
//	FILE* ptr;
//	strcpy(ps, cmd);
//	
//	ULog_Write(ULOG_INFO, "open file");
//	if ((ptr = _popen(ps, "r")) != NULL)
//	{
//
//		cout << "open file" << endl;
//		while (fgets(buf_ps, 2048, ptr) != NULL)
//		{
//			//output获取每一个print的值
//			output[i] = buf_ps;
//
//			cout << i << " = " << output[i];
//
//			/*if (i > 0)
//				strcat(result, buf_ps);*/
//
//				//result = NULL;
//			strcpy(result, "");
//			strcat(result, buf_ps);
//			i++;
//			if (strlen(result) > 2048)
//				break;
//			// 具体了解数据
//		}
//		cout << "end file" << endl;
//		_pclose(ptr);
//		ptr = NULL;
//		ULog_Write(ULOG_INFO, "end file");
//	}
//	else
//	{
//		printf("popen %s error\n", ps);
//	}
//}

int SkipRoof(int beginIndex, std::shared_ptr<GeoRadialArea> calcedSector, \
	ScanningLineInfo* pScanningLine, const short* pBuildingHeights)
{
	int no;
	PointInsideMode mode = (calcedSector) ? \
		calcedSector->PointInArea(pScanningLine->ScanningSpots().back()->samplingCord, no) : \
		PointInsideMode::INSIDE;

	if (PointInsideMode::OUTSIDE == mode)
		return beginIndex;

	int endIndex = pScanningLine->GetEndIndex();
	int i = beginIndex < 0 ? 0 : beginIndex;
	for (; i <= endIndex; i++)
	{
		if (pBuildingHeights[i] < GIS_MIN_BUILDINGHEIGHT)
		{
			break;
		}
	}

	return i;
}

bool Search(const AntInstParam& antenna, ScanningLineInfo* pScanningLine, int endIndex, double txHeight, double ueHeight,\
	const short* pAltitudes, const short* pBuildingHeights, const short* pClutterIDs)
{
	txHeight = pAltitudes[0] + txHeight;		//绝对高度
	int startIndex = pScanningLine->GetStartIndex();
	if (endIndex < startIndex)
		return false;
	
	double highestPeakSlope = INFINITY;
	double highestPeakHeight = 0;
	double highestPeakDistToTx = 0;
	int highestPeakIndex = 0;
	double distance = 0;
	int lastOutdoorIndex = 0;
	GeoXYPoint cord(pScanningLine->X + pScanningLine->Res2D.x, pScanningLine->Y + pScanningLine->Res2D.y);
	for (int i = 1; i <= endIndex; i++, cord.x += pScanningLine->Res2D.x, cord.y += pScanningLine->Res2D.y)
	{
		distance += pScanningLine->CalcResolution;

		double currSlope = (txHeight - (double)(pAltitudes[i] + pBuildingHeights[i])) / distance;	//发射源和屋顶的斜率
		double currUeSlope = (txHeight - (double)pAltitudes[i] - ueHeight) / distance;		//发射源和UE的斜率
		bool outdoor = (pBuildingHeights[i] < GIS_MIN_BUILDINGHEIGHT) ? true : false;
		bool ueLos = (outdoor) ? ((currUeSlope <= highestPeakSlope) ? true : false) : false;

		if (outdoor)
			lastOutdoorIndex = i;

		//更新最高峰参数
		if (currSlope <= highestPeakSlope) {
			highestPeakSlope = currSlope;
			highestPeakIndex = i;
			highestPeakHeight = pAltitudes[i] + pBuildingHeights[i];
			highestPeakDistToTx = distance;
		}
		
		//设置采样点数据
		if (i >= startIndex) {
			SamplingGridFeature* pFeature = (SamplingGridFeature*)((*pScanningLine)[i - startIndex]);
			if (pFeature && pFeature->isValid) {
				double msAzimuth = pScanningLine->Azimuth;
				double msElevation = atan(currUeSlope);
				AntCalcGain::TransToLCS(antenna, msAzimuth, msElevation);
				if (msElevation > 180) msElevation -= 360;//处理角度问题
				pFeature->MsAzimuth = msAzimuth * 10;
				pFeature->MsElevation = msElevation * 10;
				pFeature->TRDistance = distance;		//收发2D距离
				pFeature->TRRelHeight = txHeight - ueHeight - pAltitudes[i];		//发射天线高度 - 接收天线高度，单位米
				pFeature->TxHeightProfile1 = (i > 1) ? txHeight - pAltitudes[1] - pBuildingHeights[1] : pFeature->TRRelHeight;	//发射天线高度 - 发射端第1个采样点的高度
				pFeature->TxHeightProfile2 = (i > 2) ? txHeight - pAltitudes[2] - pBuildingHeights[2] : pFeature->TxHeightProfile1; //发射天线高度 - 发射端第2个采样点的高度
				pFeature->RxHeightProfile1 = (i > 3) ? txHeight - pAltitudes[i - 1] - pBuildingHeights[i - 1] : pFeature->TRRelHeight; //发射天线高度 - 接收端倒数第1个采样点（接收点之前）的高度
				pFeature->RxHeightProfile2 = (i > 4) ? txHeight - pAltitudes[i - 2] - pBuildingHeights[i - 2] : pFeature->RxHeightProfile1; //发射天线高度 - 接收端倒数第2个采样点的高度
				pFeature->TxSceneCode = pClutterIDs[0];
				pFeature->TxSceneCodeProfile1 = (i > 1) ? pClutterIDs[1] : pClutterIDs[0];	//发射端第1个采样点的场景代码
				pFeature->TxSceneCodeProfile2 = (i > 2) ? pClutterIDs[2] : pFeature->TxSceneCodeProfile1;	//发射端第2个采样点的场景代码
				pFeature->RxSceneCodeProfile1 = (i > 3) ? pClutterIDs[i - 1] : pClutterIDs[i];	//接收端倒数第1个采样点的场景代码
				pFeature->RxSceneCodeProfile2 = (i > 4) ? pClutterIDs[i - 2] : pFeature->RxSceneCodeProfile1;	//接收端倒数第2个采样点的场景代码
				pFeature->TRMaxEdgeHeight = txHeight - highestPeakHeight;		//发射天线高度 - 收发连线最高峰
				pFeature->EdgeDistanceToRx = (i - highestPeakIndex) * pScanningLine->CalcResolution;		//最高峰和接收点的距离
				pFeature->Los = ueLos;					//0:NLOS,1:LOS
				pFeature->Outdoor = outdoor;				//是否在室外
				pFeature->DeepLengthIndoor = (i - lastOutdoorIndex) * pScanningLine->CalcResolution;	//室内穿透深度
			
				//ULog_Write(ULOG_DEBUG, "sizeof gridFeature: %d", sizeof pFeature);
			}
		}
	}

	return true;
}

void FindModelName(const char* schemaName, const char* cellFeature, char* modelName, int len)
{
	char comm[1024] = { 0 };
	strcpy(comm, "bash ");
	strcat(comm, schemaName);
	strcat(comm, "findmodel.sh ");
	//string file_lo = "bash findmodel.sh /root/.vs/1125release/v2.1/out/build/Linux-GCC-Release/Bin/pyAndsh/pythonschema.xml";// bash findmodel.sh /root/.vs/1125release/v2.1/out/build/Linux-GCC-Release/Bin/pyAndsh/pythonschema.xml
	//strcpy(comm, file_lo.c_str());
	strcat(comm, schemaName);
	strcat(comm, PYTHONSCHEMA_FILE);
	strcat(comm, " ");
	strcat(comm, cellFeature);
	//cout << "command:" << comm << endl;
	executeCMD(comm, modelName, len);
	printf("%s", modelName);
}

LiShuttleAICalculator::LiShuttleAICalculator() : m_pPyModule(NULL), m_pPyFunc(NULL)
{
}

LiShuttleAICalculator::~LiShuttleAICalculator()
{
	m_pPyModule = NULL;
	m_pPyFunc = NULL;
}

const char* LiShuttleAICalculator::CalculatorName() const
{
	return "LiShuttleAI";
}

float LiShuttleAICalculator::CalcDeltaLossFrmDLToUL(const std::shared_ptr<PropagationModel> prop, \
	float dlFreq, float ulFreq, double rxHeight) const
{
	return 0.0f;
}

void LiShuttleAICalculator::GetSectionPathloss(std::shared_ptr<SectionPathLossCalcParam> sectionParam, const char* featurepath, float** result) const {
	char comm[1024] = { 0 };
	auto pythonschema = sectionParam->pythonSchema.c_str();
	//string file_lo = "bash aaload.sh /root/.vs/1125release/v2.1/out/build/Linux-GCC-Release/Bin/pyAndsh/pythonschema.xml";//bash aaload.sh 
	strcpy(comm, "bash ");
	strcat(comm, pythonschema);
	strcat(comm, "aaload.sh ");
	strcat(comm, pythonschema);
	strcat(comm, PYTHONSCHEMA_FILE);
	strcat(comm, " ");
	strcat(comm, featurepath);
	strcat(comm, " ");
	strcat(comm, m_modelName.c_str());

	executeCMD(comm, result, sectionParam);

}

float GetPenePathLoss(IClutterParamsManager* pClutterManager, SamplingGridFeature* pFeature, float fc, float& linearLoss)
{
	linearLoss = 0;
	if (!pClutterManager || !pFeature) {
		ULog_Write(ULOG_ERROR, "Null pointer!");
		return 0;
	}

	short realSC;
	return (pFeature->buildingPolygon && pFeature->buildingPolygon->SceneCode >= 0) ?
		pClutterManager->GetPenetrationLossOfScene(&linearLoss, realSC, pFeature->buildingPolygon->SceneCode, pFeature->DeepLengthIndoor, fc) : \
		pClutterManager->GetPenetrationLoss(&linearLoss, realSC, pFeature->clutterID, pFeature->DeepLengthIndoor, fc);
}

bool LiShuttleAICalculator::InitializePy(const char* feature, const char* cellId)
{
#ifdef USINGPYTHON
	if (!feature || feature[0] == '\0' || !cellId || cellId[0] == '\0') {
		ULog_Write(ULOG_ERROR, "Null Pointer!");
		return false;
	}

	Py_Initialize();
	if (!Py_IsInitialized())
	{
		ULog_Write(ULOG_ERROR, "An error occured when initialzing the python modules!");
		return false;
	}
	PyObject* pModule_find = NULL;
	PyObject* pFunc_find = NULL;
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	char* modelName = NULL;
	pModule_find = PyImport_ImportModule("FindModel");
	if (!pModule_find)
	{
		ULog_Write(ULOG_ERROR, "An error occured when importing the python module - FindModel!");
		Py_Finalize();
		return false;
	}

	pFunc_find = PyObject_GetAttrString(pModule_find, "find_modelName");
	if (!pFunc_find)
	{
		ULog_Write(ULOG_ERROR, "An error occured when getting the python function - find_modelName!");
		Py_Finalize();
		return false;
	}

	PyObject* pArgs_find = Py_BuildValue("(s)", feature);
	if (!pArgs_find)
	{
		ULog_Write(ULOG_ERROR, "An error occured when building the features of cell %s using python!", cellId);
		Py_Finalize();
		return false;
	}

	PyObject* pValue = PyEval_CallObject(pFunc_find, pArgs_find);
	if (!pValue)
	{
		ULog_Write(ULOG_ERROR, "An error occured when calling the python function - find_modelName with cell %s!", cellId);
		Py_Finalize();
		return false;
	}

	PyArg_Parse(pValue, "s", &modelName);
	if (!modelName || 0 == upl_strcmp(modelName, "none"))
	{
		ULog_Write(ULOG_ERROR, "The DNN model of cell %s can't be found!", cellId);
		Py_Finalize();
		return false;
	}

	ULog_Write(ULOG_INFO, "The model name of cell %s is %s", cellId, modelName);

	PyObject* pModule_load = PyImport_ImportModule("LoadModel");//python脚本的文件名
	if (!pModule_load) {
		ULog_Write(ULOG_ERROR, "An error occured when importing the python module - LoadModel!");
		Py_Finalize();
		return false;
	}

	PyObject* pFunc_load = PyObject_GetAttrString(pModule_load, "load_model");//函数
	if (!pFunc_load) {
		ULog_Write(ULOG_ERROR, "An error occured when getting the python function - load_model!");
		Py_Finalize();
		return false;
	}

	m_pPyModule = pModule_find;
	m_pPyFunc = pFunc_load;
	m_modelName = modelName;

	return true;
#else
	return false;
#endif
}

void LiShuttleAICalculator::FinalizePy()
{
#ifdef USINGPYTHON
	Py_Finalize();
#endif
	m_pPyModule = NULL;
	m_pPyFunc = NULL;
	m_modelName.clear();
}

losstype LiShuttleAICalculator::GetPathLossOnSpot(char* gridFeature) const
{
	float pathloss = SectionPathLossData::InvalidPathlossValue.pathloss;

	return pathloss;
}

void LiShuttleAICalculator::SaveFeature(RadialSectionSpotData* pSpotData, const char* featurepath) {
	FileOperator* fileOperator = new FileOperator(featurepath);
	std::string relativePath = pSpotData->getCellID(); 
	relativePath += ".csv";

	fileOperator->SaveFile(relativePath.c_str(), pSpotData);

	delete fileOperator;
}
void LiShuttleAICalculator::CalculateLinePathLoss(std::shared_ptr<RadialPathLossData> sectionPathLoss, \
	const std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
	const ScanningLineInfo* pScanningLine, int rayIndex, float** predictedPL) const
{
	int spotCount = pScanningLine->GetSpotsCount();
	auto startIndex = pScanningLine->GetStartIndex();
	if (spotCount < 0) {
		ULog_Write(ULOG_ERROR, "Error spotCount!");
		return;
	}
	PathLossValue pl = SectionPathLossData::InvalidPathlossValue;	//记录当前点路损信息
	losstype lastOutdoorLoss = SectionPathLossData::InvalidPathlossValue.pathloss;		//最后一个室外点的路损
	for (int i = 0; i < spotCount; i++)
	{
		SamplingGridFeature* pFeature = (SamplingGridFeature*)((*pScanningLine)[i]);
		char gridFeature[1024];
		pFeature->ToString(gridFeature, 1024);
		if (!pFeature)
			continue;
		if (pFeature->Outdoor) {
			//调用python...
			pl.pathloss = predictedPL[rayIndex][startIndex + i];
			pl.linearIndoorLoss = 0;
			pl.spotType = SpotType::OUTDOOR;
			lastOutdoorLoss = pl.pathloss;	//设置最后一个室外点的路损
		}
		else {
			if (lastOutdoorLoss == SectionPathLossData::InvalidPathlossValue.pathloss) {
				//之前未计算过室外路损，则把当前点特征值输入python进行计算
				lastOutdoorLoss = predictedPL[rayIndex][startIndex + i];
				//lastOutdoorLoss = GetPathLossOnSpot(gridFeature);
			}

			pl.pathloss = lastOutdoorLoss + GetPenePathLoss(sectionParam->ClutterParamsManager, pFeature, sectionParam->Frequency, pl.linearIndoorLoss);
			pl.spotType = SpotType::INDOOR;
		}
		sectionPathLoss->SetPathLossValue(rayIndex, i, pl);
	}
}

void LiShuttleAICalculator::CalculateSectionPathLoss(SectorPathLossData* pLosData, \
	std::shared_ptr<SectionPathLossCalcParam> sectionParam, bool* pStopCalculator)
{
	//生成特征
	RadialSectionSpotData* pSpotData = new RadialSectionSpotData(sectionParam);
	CalculateSectionFeature(pSpotData, sectionParam);
	if (m_hasInvalidGrid) {
		ULog_Write(ULOG_ERROR, "Warning: there are some invalid grids!");
	}

	CellAIGridFeature* cellFeature = new CellAIGridFeature(sectionParam->SectorCalcParam, this);
	cellFeature->SetSpotFeature(pSpotData);
	cellFeature->CalculateStatisticsFeature();

	SaveFeature(pSpotData, m_calcParams.featurePath.c_str());

	char feature[1024], modelName[100] = { 0 };
	cellFeature->GetCellFeature(feature, 1024);

	//根据小区特征 调用python得到modelName
	//若无该小区对应模型  则打出日志，并退出该小区的计算

	
	auto pre_addr = &*pSpotData;
	int len = 9;//模型名的长度
	FindModelName(sectionParam->pythonSchema.c_str(), feature, modelName, len);
	auto addr = &*pSpotData;
	string none = "none\n";
	//string mName = modelName;
	if (!strcmp(modelName, none.c_str())) {
		return;
	}
	m_modelName = modelName;

	//计算损耗
	auto featurepath = m_calcParams.featurePath + "/" + sectionParam->SectorCalcParam->Cell->cellID + ".csv";
	float** predictedPL = NULL;
	//给predictedPL分配内存空间
	size_t lineNumber = sectionParam->GetScanningLineNumber();
	int startIndex = sectionParam->GetStartPointIndex();
	int endIndex = sectionParam->GetEndPointIndex();
	predictedPL = new float* [lineNumber];
	for (int i = 0; i < lineNumber; i++) {
		predictedPL[i] = new float[startIndex, endIndex];
	}
	GetSectionPathloss(sectionParam, featurepath.c_str(), predictedPL);

	/*for (int i = 0; i < lineNumber; i++) {
		for (int j = startIndex; j <= endIndex; j++) {
			cout << predictedPL[i][j] << " ";
		}
		cout << endl;
	}*/

	auto sectionPathLoss = std::make_shared<RadialPathLossData>(sectionParam);
	for (size_t i = 0; i < lineNumber; i++) {
		ScanningLineInfo* pScanningLine = pSpotData->GetScanningLine(i);
		if (NULL == pScanningLine)
			break;
		CalculateLinePathLoss(sectionPathLoss, sectionParam, pScanningLine, i, predictedPL);
	}

	delete cellFeature;
	//delete pSpotData;
	for (int i = 0; i < lineNumber; i++) {
		if (predictedPL[i]) {
			delete predictedPL[i];
		}
	}
	delete predictedPL;
	//FinalizePy();

	pLosData->AddSectionPathLoss(std::dynamic_pointer_cast<SectionPathLossData>(sectionPathLoss));
}

CellCategory LiShuttleAICalculator::CalculateSectionFeature(RadialSectionSpotData* pSpotData, \
	std::shared_ptr<SectionPathLossCalcParam> sectionParam)
{
	CellCategory cellCategory = CellCategory::MiniCell;

	if (NULL == pSpotData) {
		ULog_Write(ULOG_ERROR, "pSpotData mustn't be NULL!");
		return cellCategory;
	}

	//预加载地理数据
	auto clutterData = PreLoadGisData(sectionParam, DemDataType::Clutter);
	auto heightData = PreLoadGisData(sectionParam, DemDataType::Height);
	auto buildingHeightData = PreLoadGisData(sectionParam, DemDataType::Building);
	std::map<DemDataType, std::shared_ptr<GisPreLoadData>> gisData;
	gisData[DemDataType::Height] = heightData;
	gisData[DemDataType::Clutter] = clutterData;
	gisData[DemDataType::Building] = buildingHeightData;
	
	//如果传入的路损对象未包含射线谱，则需要进行射线谱的预计算。
	CellularType cellType(buildingHeightData->Buffer(), buildingHeightData->Bound().size(), sectionParam->TxAntennaHeight, \
			sectionParam->microThreshold, sectionParam->macroThreshold);
	cellCategory = cellType.GetType();
	if ((sectionParam->TxAntennaHeight < sectionParam->macroMinAntHeight) && \
		(cellCategory == CellCategory::MacroCell)) {
		cellCategory = CellCategory::MiniCell;
	}
	
	if (cellCategory == CellCategory::MacroCell)
		ULog_Write(ULOG_INFO, "%s", "MacroCell");
	else if (cellCategory == CellCategory::MicroCell)
		ULog_Write(ULOG_INFO, "%s", "MicroCell");
	else
		ULog_Write(ULOG_INFO, "%s", "MiniCell");

	auto calcedSector = GetCalcedSector(GeoXYPoint(sectionParam->X, sectionParam->Y), \
		sectionParam->StartAngle, sectionParam->EndAngle, sectionParam->SectorCalcParam->AntCalcParam->pAntennaGainParam->calcRange);

	//内存分配
	int startIndex = sectionParam->GetStartPointIndex();
	int endIndex = sectionParam->GetEndPointIndex();
	if (endIndex < startIndex)
		return cellCategory;
	short* pAltitudes = (short*)upl_malloc(sizeof(short) * (endIndex + 1));
	short* pClutters = (short*)upl_malloc(sizeof(short) * (endIndex + 1));
	short* pBuildingHeights = (short*)upl_malloc(sizeof(short) * (endIndex + 1));
	std::map<DemDataType, short*> gisDataOnLine;
	gisDataOnLine[DemDataType::Height] = pAltitudes;
	gisDataOnLine[DemDataType::Clutter] = pClutters;
	gisDataOnLine[DemDataType::Building] = pBuildingHeights;
	
	double resolution = sectionParam->CalcResolution;
	double radius = sectionParam->CalcEndDistance;
	double angleRes = sectionParam->AngleResolution * PI / 180.0;
	double x = resolution * cos(sectionParam->StartAngle * PI / 180.0);
	double y = resolution * sin(sectionParam->StartAngle * PI / 180.0);
	GeoXYLine res2D(GeoXYPoint(0, 0), GeoXYPoint(x, y));
	GeoRatationFactor ratationFactor(angleRes);
	
	size_t lineNumber = sectionParam->GetScanningLineNumber();
	for (size_t i = 0; i < lineNumber; i++, ratationFactor.RotateLine(res2D, res2D)) {
		ScanningLineInfo* pScanningLine = pSpotData->GetScanningLine(i);
		if (NULL == pScanningLine)
			break;

		std::vector<GeoBuildingPtr> buildingPolygons;
		LoadGisDataOnLine(gisDataOnLine, buildingPolygons, gisData, sectionParam, res2D.back());
		CorrectAltitudes(gisDataOnLine, endIndex - startIndex + 1);

		this->InitScanningLine(*pScanningLine, sectionParam, gisDataOnLine, buildingPolygons, res2D);

		Search(*(sectionParam->SectorCalcParam->AntCalcParam->pAntennaGainParam), pScanningLine, endIndex, \
			sectionParam->TxAntennaHeight, sectionParam->RxAntennaHeight, pAltitudes, pBuildingHeights, pClutters);
	}

	//内存释放
	upl_free(pAltitudes);
	upl_free(pClutters);
	upl_free(pBuildingHeights);

	return cellCategory;
}

void LiShuttleAICalculator::InitScanningLine(ScanningLineInfo& scanningLineInfo, \
	const std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
	const std::map<DemDataType, short*>& gisDataOnLine, \
	const std::vector<GeoBuildingPtr>& buidlingPolygons, GeoXYLine& res2D) const
{
	PropagationCalculator::InitScanningLine(scanningLineInfo, sectionParam, res2D);

	double deltaX = res2D.back().x;
	double deltaY = res2D.back().y;

	//生成扫描点数据
	int startIndex = sectionParam->GetStartPointIndex();
	int endIndex = sectionParam->GetEndPointIndex();
	if (endIndex < startIndex)
		return;
	double x = sectionParam->X + (double)startIndex * deltaX;
	double y = sectionParam->Y + (double)startIndex * deltaY;
	scanningLineInfo.ScanningSpots().resize(endIndex - startIndex + 1);
	const short* pAltitudes = gisDataOnLine.at(DemDataType::Height);
	const short* pClutters = gisDataOnLine.at(DemDataType::Clutter);
	const short* pBuildHights = gisDataOnLine.at(DemDataType::Building);
	for (int i = startIndex; i <= endIndex; i++, x += deltaX, y += deltaY)
	{
		scanningLineInfo[i - startIndex] = SamplingGridFeature::CreateInstance(GeoXYPoint(x, y));
		PropagationCalculator::SetSpotInfo(scanningLineInfo[i - startIndex], i, pAltitudes, pClutters, pBuildHights, \
			buidlingPolygons);
	}
	scanningLineInfo.Line.GetPointList().clear();
	scanningLineInfo.Line.GetPointList().push_back(scanningLineInfo.GetHead()->samplingCord);
	scanningLineInfo.Line.GetPointList().push_back(scanningLineInfo.GetBack()->samplingCord);
}


