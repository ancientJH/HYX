#include "PathLossCalcEngine.h"
#include "SectorPathlossData.h"
#include "LiShuttlePropagationCalculator.h"
#include "LiShuttleAICalculator.h"
#include "LiShuttleAIModel.h"
#include "LiShuttleSceneModel.h"
#include "UPlanCommonExport.h"
#include "CellLoader.h"
#include "GeoWallBlock.h"
#include "GisPreLoadData.h"
#include "GeoBlockAssist.h"
#include "GeoBlockTraverser.h"
#include "CalcRegion.h"
#include "PropagationFunc.h"
#include "FileAccesser.h"
#include "RayTracker.h"
#include <set>

PathLossCalcEngine::PathLossCalcEngine(IServiceHelper* pAppHelper) :\
	AppEngine(pAppHelper, PATHLOSS_CALC_SERVICE), m_pCalcParamFactory(NULL), \
	m_stopFlag(false), m_taskId("")
{
	if (m_pServiceHelper)
		m_pServiceHelper->AddService(RAYS_RECORD_SERVICE, (void*)this);
}

PathLossCalcEngine::~PathLossCalcEngine()
{
	if (NULL != m_pCalcParamFactory)
	{
		delete m_pCalcParamFactory;
		m_pCalcParamFactory = NULL;
	}

	if (m_pServiceHelper)
		m_pServiceHelper->RemoveService(RAYS_RECORD_SERVICE);
}

IPathLossAccesser* PathLossCalcEngine::GetPathLossAccesser() const
{
	return (IPathLossAccesser*)(&m_fileAccesser);
}

bool PathLossCalcEngine::GetRaySaved() const
{
	return m_calcParams.rayCacheSaved;
}

void PathLossCalcEngine::SetRaySaved(bool raySaved)
{
	m_calcParams.rayCacheSaved = raySaved;
}

void PathLossCalcEngine::PrintDebugInfo()
{
	PrintMemInfo();

#ifdef INNERCLASS_TEST
	int plUseCount = SectionPathLossData::GetUseCount();
	int rayUseCount = RaySpectrum::GetUseCount();
	ULog_Write(ULOG_INFO, "Pathloss data use count: %d, RaySpectrum use count: %d", plUseCount, rayUseCount);

	int wallUseCount = GeoWall::GetUseCount();
	int wallBlockUseCount = GeoWallBlock::GetUseCount();
	int wallTravUseCount = GeoBlockTraverser::GetUseCount();
	int blockAssistUseCount = GeoBlockAssist::GetUseCount();
	int gisPreLoadUseCount = GisPreLoadData::GetUseCount();
	ULog_Write(ULOG_INFO, "Wall use: %dk, Wall block use: %d, Wall traverser use: %d, Block assist use: %d, gisPreLoad use: %d", \
		wallUseCount/1000, wallBlockUseCount, wallTravUseCount, blockAssistUseCount, gisPreLoadUseCount);

	int calcRegionUseCount = CalcRegion::GetUseCount();
	ULog_Write(ULOG_INFO, "CalcRegion: %d", calcRegionUseCount);
#endif
}

const char* PathLossCalcEngine::GetResultFolder() const
{
	return m_calcParams.pathlossDirectory.c_str();
}

bool PathLossCalcEngine::Init()
{
	if (!SetupPathLossParams(&m_calcParams, m_pServiceHelper->GetRuntimeSchema(), \
		m_pServiceHelper->GetCommonParams(), m_pServiceHelper->GetAppFolder())) {
		ULog_Write(ULOG_FATAL, "Error occurs when parsing the xml file for pathloss!");
		return false;
	}
	ULog_Write(ULOG_INFO, "The count of used threads is %d", m_calcParams.threadCount);

	IGeoInfo* pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));
	if (!pGeoInfo) {
		ULog_Write(ULOG_ERROR, "No right outdoor map has been configed.");
		return false;
	}

	m_mapRect = pGeoInfo->GetMapRect();

	if (m_pCalcParamFactory) {
		delete m_pCalcParamFactory;
		m_pCalcParamFactory = NULL;
	}
	m_pCalcParamFactory = new CalcParamFactory(m_calcParams, \
		(ISceneInfo*)(m_pServiceHelper->GetService(SCENEINFO_COLLECTION_SERVICE)), pGeoInfo->GetMapName());
	m_pCalcParamFactory->SetAntLibrary(((AntCalcGain*)(m_pServiceHelper->GetService(ANTENNA_CALC_SERVICE)))->m_pAntLibrary);

	m_fileAccesser.LoadRecordFile(m_calcParams.pathlossDirectory.c_str());
	m_fileAccesser.SetSysDigest(m_pServiceHelper->GetServiceDigest());

	if (m_calcParams.correctByDt && !m_calcParams.dtFileFolder.empty()) {
		FileOperator fileOperator(m_calcParams.dtFileFolder.c_str());
		fileOperator.LoadFile(FILENAME_OF_DTANCHOR, dynamic_cast<IDataSerialize*>(&m_measAnchors));
	}

	return true;
}

bool TransCompare(const std::shared_ptr<Transceiver>& a, const std::shared_ptr<Transceiver>& b) {
	if (a->parentId < b->parentId)
		return true;
	else if (a->parentId > b->parentId)
		return false;

	return a->id < b->id;
}

void PathLossCalcEngine::Run(const char* taskId)
{
	m_taskId = taskId;

	if (!Init())
		return;

	TransceiverList focusedTrans;
	((CellLoader*)(m_pServiceHelper->GetService(BS_ENGINEERING_PARAMS_SERVICE)))->GetCells(\
						&focusedTrans, m_calcParams.transFilter.c_str());
	
	if (focusedTrans.size() == 0) {
		ULog_Write(ULOG_INFO, "No cells are selected!");
		return;
	}

	//按站进行升序排列
	focusedTrans.sort(TransCompare);	

	ULog_Write(ULOG_INFO, "Pathloss calculation begins(Engine %s).", m_taskId.c_str());

	m_calcParams.areRaysRecorded ? TraceRays(focusedTrans) : Calculate(focusedTrans, m_calcParams.rayCacheSaved);

	focusedTrans.clear();

	ULog_Write(ULOG_INFO, "Pathloss calculation ends(Engine %s).", m_taskId.c_str());
}

void PathLossCalcEngine::TraceRays(const std::list<std::shared_ptr<Transceiver>>& transceiverList)
{
	if (transceiverList.size() == 0) {
		ULog_Write(ULOG_ERROR, "transceiverList shouldn't be null!");
		return;
	}

	if (m_calcParams.filePathOfFocusRoads.empty()) {
		ULog_Write(ULOG_ERROR, "filePathOfFocusRoads shouldn't be empty!");
		return;
	}

	LiShuttlePropagationCalculator propCalculator;
	propCalculator.m_pAntCalcGain = (AntCalcGain*)(m_pServiceHelper->GetService(ANTENNA_CALC_SERVICE));
	propCalculator.m_pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));
	propCalculator.m_calcParams = m_calcParams;

	RayTracker rayTracker(propCalculator.m_pGeoInfo->GetCoordinate(), m_calcParams.calcResolution);
	rayTracker.LoadFile(m_calcParams.filePathOfFocusRoads.c_str());

	int k = 0;
	HashValueType lastHashValue = 0;
	for (auto tran : transceiverList) {
		++k;
		if (m_stopFlag)
			break;

		if (!m_mapRect.IsPointInRect(tran->X, tran->Y)) {
			ULog_Write(ULOG_INFO, "The cell(%s) is not in the map", tran->transID.c_str());
			continue;
		}

		if (tran->antInstParams.size() == 0) {
			ULog_Write(ULOG_ERROR, "Transciver(%s) has no valid antennas.", tran->transID.c_str());
			continue;
		}

		auto antCalcParam = m_pCalcParamFactory->GetAntCalcParam(\
			& (tran->antInstParams.front()), tran->cells);
		if (!antCalcParam) {
			ULog_Write(ULOG_ERROR, "The parameters of transciver(%s) are invalid.", tran->transID.c_str());
			continue;
		}

		rayTracker.ReplaceAnt(antCalcParam);
		if (0 == rayTracker.GetCountInCell())
			continue;

		std::string digest;
		antCalcParam->ToString(digest);
		std::hash<std::string> hashstr;
		HashValueType currHashValue = hashstr(digest);

		auto sectionParam = antCalcParam->SectorCalcParams.front()->SectionParamArray.front();

		ULog_Write(ULOG_INFO, "Rays Tracing of cell %s(f=%.1fMHz) begins.(%dth cell to be traced)", \
			tran->transID.c_str(), tran->cells.front()->dlFreq, k);

		propCalculator.TraceRaysForSamplingSpots(&rayTracker, sectionParam, lastHashValue != currHashValue);

		lastHashValue = currHashValue;
		
		antCalcParam.reset();

		ULog_Write(ULOG_INFO, "Rays Tracing of cell %s ends.(%dth cell to be traced)", \
			tran->transID.c_str(), k);
	}  //end of for (transceiverList)

	//保存
	std::string fileName = GetFileName(m_calcParams.filePathOfFocusRoads.c_str());
	fileName += OUTDOOR_RAYTRACING_EXTNAME;
	m_fileAccesser.SaveData(fileName.c_str(), dynamic_cast<IDataSerialize*>(&rayTracker));
}

void PathLossCalcEngine::Calculate(int siteId, bool raySaved)
{
	USite* pSite = ((CellLoader*)(m_pServiceHelper->GetService(BS_ENGINEERING_PARAMS_SERVICE)))->getSite(siteId);
	if (pSite)
		Calculate(pSite->m_transList, raySaved);
}

void PathLossCalcEngine::Calculate(const TransceiverList& transceiverList, bool raySaved)
{
	if (transceiverList.size() == 0) {
		ULog_Write(ULOG_ERROR, "transceiverList shouldn't be null!");
		return;
	}

	int k = 0;
	std::unique_ptr<TransceiverPLAccesser> lastTransPLAccesser;
	std::unique_ptr<TransceiverPLAccesser> currTransceiverPL;
	for (auto tran : transceiverList) {
		++k;
		
		if (m_stopFlag)
			break;

		if (!m_mapRect.IsPointInRect(tran->X, tran->Y)) {
			ULog_Write(ULOG_INFO, "The cell(%s) is not in the map", tran->transID.c_str());
			continue;
		}

		//try {
			if (tran->antInstParams.size()==0) {
				ULog_Write(ULOG_ERROR, "Transciver(%s) has no valid antennas.", tran->transID.c_str());
				continue;
			}

			currTransceiverPL = upl_make_unique<TransceiverPLAccesser>(tran->transID.c_str());
			auto antCalcParam = m_pCalcParamFactory->GetAntCalcParam(\
										&(tran->antInstParams.front()), tran->cells);

			if (!antCalcParam) {
				ULog_Write(ULOG_ERROR, "The parameters of transciver(%s) are invalid.", tran->transID.c_str());
				currTransceiverPL.reset();
				continue;
			}

			currTransceiverPL->AddAntCalcParams(antCalcParam);
			
			int modelType = antCalcParam->GetMainModelType();
			ReCalcType needToRecalcType;
			if (m_calcParams.isForcedCalc) {
				needToRecalcType = NEEDTO_CALC_PATHLOSS | NEEDTO_CALC_RAYSPEC;
			}
			else if ((modelType < 0) || (AI_MODEL == modelType)) {
				needToRecalcType = NEEDTO_CALC_PATHLOSS;
			}
			else
			{
				needToRecalcType = m_fileAccesser.NeedReCalcPathLoss(tran->cells.front()->cellID.c_str(), \
					currTransceiverPL->GetPathlossHash(), currTransceiverPL->GetRaySpectrumHash());

				if (m_calcParams.noCheckRay && m_fileAccesser.IsRayFileExist(tran->cells.front()->cellID.c_str()))
					needToRecalcType &= (~NEEDTO_CALC_RAYSPEC);

				if (m_calcParams.noCheckPathloss && m_fileAccesser.IsPathLossFileExist(tran->cells.front()->cellID.c_str())) {
					if ( !raySaved || (!(needToRecalcType & NEEDTO_CALC_RAYSPEC)) )
						needToRecalcType &= (~NEEDTO_CALC_PATHLOSS);
				}
			}
			
			if ( !(needToRecalcType & NEEDTO_CALC_PATHLOSS) && \
				( !(needToRecalcType & NEEDTO_CALC_RAYSPEC) || !raySaved ) )
			{
				currTransceiverPL.reset();
				antCalcParam.reset();
				continue;
			}
			
			ULog_Write(ULOG_INFO, "PathLoss calculation of cell %s(f=%.1fMHz) begins.(%dth cell of the engine)", \
				tran->transID.c_str(), tran->cells.front()->dlFreq, k);
			PrintDebugInfo();

			bool lastRaySpecOK = false;
			if ( (lastTransPLAccesser) && \
				(lastTransPLAccesser->TransRaySpectrum.GetAntCount() > 0) )
			{
				auto raySpectrum = lastTransPLAccesser->TransRaySpectrum.PopHead();
				if (raySpectrum && currTransceiverPL->GetRaySpectrumHash() == raySpectrum->GetHash()){
					raySpectrum->AntConfigID = tran->antInstParams.front().ID;  /*只计算第一个天线*/
					currTransceiverPL->AddRaySpectrum(raySpectrum);
					lastRaySpecOK = true;
				}
			}
			
			if (!lastRaySpecOK && !(needToRecalcType & NEEDTO_CALC_RAYSPEC)) {
				const char* cellId = tran->cells.front()->cellID.c_str();
				auto raySpectrum = std::make_shared<RadialRaySpectrum>(antCalcParam);
				if (m_fileAccesser.LoadPathLossFile(cellId, NULL, raySpectrum.get())) {
					currTransceiverPL->AddRaySpectrum(raySpectrum);
				}
			}

			lastTransPLAccesser.reset();
			lastTransPLAccesser = CalculateOneTransceiver(tran, std::move(currTransceiverPL), raySaved);
			
			antCalcParam.reset();

			ULog_Write(ULOG_INFO, "PathLoss calculation of cell %s ends.(%dth cell of the engine)", \
				tran->transID.c_str(), k);
			PrintDebugInfo();
		/*}
		catch (exception &e) {
			ULog_Write(ULOG_FATAL, "%s", e.what());
			ULog_Write(ULOG_ERROR, "Abnormal aborted! The cell is %s", tran->transID.c_str());
		}*/
	}  //end of for (transceiverList)
}

std::unique_ptr<TransceiverPLAccesser> PathLossCalcEngine::CalculateOneTransceiver( \
	const std::shared_ptr<Transceiver> transceiver,\
	std::unique_ptr<TransceiverPLAccesser> pPLAccesser, bool raySaved)
{
	if (!transceiver)
	{
		ULog_Write(ULOG_ERROR, "transceiver shouldn't be null!");
		return pPLAccesser;
	}

	if (!pPLAccesser)
	{
		ULog_Write(ULOG_ERROR, "pPLAccesser shouldn't be null!");
		return pPLAccesser;
	}

	auto sectorParam = pPLAccesser->SectorCalcParamList.front();
	auto pSectorPathLoss = upl_make_unique<SectorPathLossData>(*sectorParam);
	auto raySpec = pPLAccesser->GetAntRaySpectrum(sectorParam->AntCalcParam->pAntennaGainParam->ID);
	bool needCalcRaySpectrum = true;
	if (raySpec){
		pSectorPathLoss->AntRaySpectrum = raySpec;
		needCalcRaySpectrum = false;
	}

	pSectorPathLoss = CalculateOneSector(sectorParam, std::move(pSectorPathLoss));

	if (pSectorPathLoss)
	{
		IGeoInfo* pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));

		//用路测数据修正路损
		if (m_calcParams.correctByDt) {
			std::list<PathlossLabel>* plList = m_measAnchors.GetPathlossAnchors(sectorParam->Cell);
			if (plList)
				pSectorPathLoss->CorrectPathloss(plList);
		}

		//保存总路损
		pSectorPathLoss->SetPathLossMode(PathLossMode::TotalLoss);
		auto sectorPLMatrix = SectorPathLossMatrix::ComposedPathLoss(*(transceiver->cells.front()), \
			* sectorParam, pSectorPathLoss.get(), pGeoInfo);

		if (sectorPLMatrix)
		{
			std::shared_ptr<RaySpectrum> savedRay;
			if (raySaved)
				savedRay = pSectorPathLoss->AntRaySpectrum;
			sectorPLMatrix = SaveSectorPathLoss(std::move(sectorPLMatrix), savedRay);
			savedRay.reset();
			
			if (needCalcRaySpectrum)
				pPLAccesser->AddRaySpectrum(pSectorPathLoss->AntRaySpectrum);
		}

		if (sectorPLMatrix) {
			//sectorPLMatrix->ClearBuffer();
			sectorPLMatrix.reset();
		}

		//保存不含室内线性损耗的路损矩阵
		if (m_calcParams.peneLossMode != MODE_PENELOSS_DEFAULT) {
			pSectorPathLoss->SetPathLossMode(PathLossMode::LossWithoutLinearPene);
			sectorPLMatrix = SectorPathLossMatrix::ComposedPathLoss(*(transceiver->cells.front()), \
				* sectorParam, pSectorPathLoss.get(), pGeoInfo);
			if (sectorPLMatrix)
			{
				static char rayFileName[48], lossFileName[48];
				GetLossFileName(lossFileName, rayFileName, 48, sectorPLMatrix->CellID.c_str());
				std::string fileName = m_calcParams.plAdditionalSubDir;
				fileName.append(PathSplit);
				fileName.append(lossFileName);
				m_fileAccesser.SaveData(fileName.c_str(), sectorPLMatrix.get());
				//sectorPLMatrix->ClearBuffer();
				sectorPLMatrix.reset();
			}
		}

#ifdef WIN32 & INNERCLASS_TEST
		//pSectorPathLoss = OutputPathLossTxt(sectorParam->Cell->cellID.c_str(), std::move(pSectorPathLoss)); //调试用
#endif

	}

	if (pSectorPathLoss) {
		pSectorPathLoss->Clear();
		pSectorPathLoss.reset();
	}
	
	return pPLAccesser;
}

std::unique_ptr<SectorPathLossData> PathLossCalcEngine::CalculateOneSector(const std::shared_ptr<SectorPathLossCalcParam> sectorCalcParam, \
	std::unique_ptr<SectorPathLossData> pSectorPathLoss)
{
	std::unique_ptr<SectorPathLossData> emptyLoss;
	if (!sectorCalcParam)
	{
		ULog_Write(ULOG_ERROR, "sectorCalcParam shouldn't be null!");
		return emptyLoss;
	}

	PropagationCalculator* pPropCalculator = NULL;

	if (RAYTRACING_MODEL == sectorCalcParam->SectionParamArray.front()->PropagModel->PropModelType) {
		pPropCalculator = new LiShuttlePropagationCalculator();
	}
	else if (strcmp(LiShuttleAIModel::CategoryName, \
		sectorCalcParam->SectionParamArray.front()->PropagModel->GetCategoryName()) == 0) {
		pPropCalculator = new LiShuttleAICalculator();
	}else{
		ULog_Write(ULOG_ERROR, "Propagation Model(%s) of cell(%s) is not supported!", \
			sectorCalcParam->SectionParamArray.front()->PropagModel->GetCategoryName(), \
			sectorCalcParam->Cell->cellID.c_str());
		return emptyLoss;
	}

	if (pPropCalculator) {
		pPropCalculator->m_pAntCalcGain = (AntCalcGain*)(m_pServiceHelper->GetService(ANTENNA_CALC_SERVICE));
		pPropCalculator->m_pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));
		auto sectionParam = sectorCalcParam->SectionParamArray.front();
		sectionParam->pythonSchema = m_calcParams.pythonSchema;
		pPropCalculator->m_calcParams = m_calcParams;
		pPropCalculator->CalculateSectionPathLoss(pSectorPathLoss.get(), sectionParam, &m_stopFlag);
		delete pPropCalculator;
	}
	
	return pSectorPathLoss;
}

std::unique_ptr<SectorPathLossMatrix> PathLossCalcEngine::SaveSectorPathLoss(\
			std::unique_ptr<SectorPathLossMatrix> sectorPathLossMat, \
			std::shared_ptr<RaySpectrum> raySpec)
{
	std::unique_ptr<SectorPathLossMatrix> empty;
	if (!sectorPathLossMat)
	{
		ULog_Write(ULOG_ERROR, "sectorPathLossMat shouldn't be null!");
		return empty;
	}

	m_fileAccesser.SavePathLossFile(sectorPathLossMat->CellID.c_str(), \
			sectorPathLossMat->GetDataBound(), sectorPathLossMat.get(), \
		raySpec ? dynamic_cast<IDataSerialize*>(raySpec.get()) : nullptr);
	
	return sectorPathLossMat;
}

std::unique_ptr<SectorPathLossData> PathLossCalcEngine::OutputPathLossTxt(const char* cellId, std::unique_ptr<SectorPathLossData> pSectorPathLoss) const
{
	if (!cellId || !pSectorPathLoss)
		return pSectorPathLoss;

	std::string filePath = m_calcParams.pathlossDirectory + "/";
	filePath.append(cellId);
	filePath.append("_detail.txt");

	StreamFile streamFile(filePath.c_str(), "w");
	if (streamFile.isOpen()) {
		pSectorPathLoss->Serialize(streamFile);
	}
	streamFile.close();

	return pSectorPathLoss;
}
