#include "CalcParamFactory.h"
#include "GeoInfo.h"
#include "NETypes.h"
#include "UPlanCommonExport.h"
#include "PropagationFunc.h"
#include "PropagationTypes.h"
#include "LiShuttlePropagationModel.h"
#include "ClutterParamsManager.h"
#include "tinyxml2.h"
#include <string>

using namespace tinyxml2;
using namespace uplancommon;

void UnifyRayTracingParam(std::shared_ptr<AntPathLossCalcParam> antCalcParam)
{
	for (auto sectorParam : antCalcParam->SectorCalcParams)
	{
		auto mainSectionParam = sectorParam->SectionParamArray.front();
		if (mainSectionParam->PropagModel->PropModelType == RAYTRACING_MODEL)
		{
			if ( (antCalcParam->RayTracingResolution < PATHLOSS_MINRESOLUTON) || \
				((antCalcParam->RayTracingResolution > mainSectionParam->CalcResolution) \
				&& (mainSectionParam->CalcResolution >= PATHLOSS_MINRESOLUTON)) )
			{
				antCalcParam->RayTracingResolution = mainSectionParam->CalcResolution;
			}

			if ( (antCalcParam->RayTracingRadius < PATHLOSS_MINCALCRADIUS) || \
				((antCalcParam->RayTracingRadius < mainSectionParam->CalcEndDistance) \
					&& (mainSectionParam->CalcEndDistance >= PATHLOSS_MINCALCRADIUS)) )
			{
				antCalcParam->RayTracingRadius = mainSectionParam->CalcEndDistance;
			}

			if ( (antCalcParam->RayTracingAngleRes < PATHLOSS_MINRESOLUTON) || \
				((antCalcParam->RayTracingAngleRes > mainSectionParam->AngleResolution) \
				&& (mainSectionParam->AngleResolution >= PATHLOSS_MINRESOLUTON)) )
			{
				antCalcParam->RayTracingAngleRes = mainSectionParam->AngleResolution;
			}
		}
	}

	for (auto sectorParam : antCalcParam->SectorCalcParams)
	{
		auto mainSectionParam = sectorParam->SectionParamArray.front();
		if (mainSectionParam->PropagModel->PropModelType == RAYTRACING_MODEL)
		{
			mainSectionParam->CalcStartDistance = antCalcParam->RayTracingResolution;
			mainSectionParam->CalcEndDistance = antCalcParam->RayTracingRadius;
			mainSectionParam->CalcResolution = antCalcParam->RayTracingResolution;
			mainSectionParam->AngleResolution = antCalcParam->RayTracingAngleRes;
		}
	}
}

bool CheckParam(const std::shared_ptr<AntPathLossCalcParam> antCalcParam)
{
	if (!antCalcParam) {
		ULog_Write(ULOG_ERROR, "antCalcParam is null!");
		return false;
	}

	if (antCalcParam->SectorCalcParams.size() == 0) {
		ULog_Write(ULOG_ERROR, "No sector!");
		return false;
	}

	for (auto sector : antCalcParam->SectorCalcParams) {
		if (!sector->Cell) {
			ULog_Write(ULOG_ERROR, "No cell!");
			return false;
		}
		if (sector->SectionParamArray.size() == 0) {
			ULog_Write(ULOG_ERROR, "No valid pathloss calculation parameter!");
			return false;
		}

		for (auto section : sector->SectionParamArray) {
			if (!section->PropagModel) {
				ULog_Write(ULOG_ERROR, "No valid propagaton model has been specified!");
				return false;
			}
			if (!section->ClutterParamsManager) {
				ULog_Write(ULOG_ERROR, "Clutter parameter manager is null!");
				return false;
			}
		}
	}

	return true;
}

CalcParamFactory::CalcParamFactory(const PathlossCalcParams& calcParam, ISceneInfo* pSceneInfo, const char* city) \
	: m_pClutterParamsManager(NULL), m_calcParams(calcParam), m_pSceneInfo(pSceneInfo), m_city(city)
{																																																																																																																																																																																																																																														OUTOFDEADLINE;
	m_pPropModelFactory = PropagationModelFactory::instance();

	//强制生成ClutterParamsManager
	if (NULL == GetClutterParamsManager()) {
		throw;
	}
}

CalcParamFactory::~CalcParamFactory()
{
	/*m_materialInfoMap.clear();
	m_sceneInfoMap.clear();*/
	m_clutterMatchMap.clear();

	if (NULL != m_pClutterParamsManager) {
		delete m_pClutterParamsManager;
		m_pClutterParamsManager = NULL;
	}
}

void CalcParamFactory::SetAntLibrary(AntennaLoader* pAntLibrary)
{
	m_pAntLibrary = pAntLibrary;
}

IClutterParamsManager* CalcParamFactory::GetClutterParamsManager()
{
	if (NULL != m_pClutterParamsManager)
		return m_pClutterParamsManager;

	if (!m_pSceneInfo) {
		ULog_Write(ULOG_FATAL, "Null pointer of m_pSceneInfo!");
		return nullptr;
	}
	m_pClutterParamsManager = new ClutterParamsManager(m_pSceneInfo);

	LoadClutterMatching(m_calcParams.clutterMatchFilePath.c_str(), m_calcParams.clutterMatchName.c_str());   //修改

	for (auto clutterMatching : m_clutterMatchMap) {
		m_pClutterParamsManager->SetClutterParams(clutterMatching.first, clutterMatching.second);
	}
	m_pClutterParamsManager->SetClutterParams(DEFAULT_CLUTTER_CODE, DEFAULT_SCENE_CODE);

	return m_pClutterParamsManager;
}

void CalcParamFactory::LoadClutterMatching(const char* xmlFileName, const char* clutterMatchingName)
{
	m_clutterMatchMap.clear();

	::LoadClutterMatching(&m_clutterMatchMap, xmlFileName, clutterMatchingName);

	if (m_clutterMatchMap.empty()) {
		ULog_Write(ULOG_ERROR, "The cluttersettings(%s) haven't been found!", clutterMatchingName);
		throw;
	}
}

std::shared_ptr<AntPathLossCalcParam> CalcParamFactory::GetAntCalcParam(\
	AntInstParam* pAntennaParam, const std::list<std::shared_ptr<UCell>>& cells)
{
	std::shared_ptr<AntPathLossCalcParam> nullParam;
	if (NULL == pAntennaParam) {
		ULog_Write(ULOG_ERROR, "Null Antenna!");
		return nullParam;
	}

	if ( (cells.empty()) || (cells.front()->propModels.empty()) )
	{
		ULog_Write(ULOG_ERROR, "No valid cells or propagation models!");
		return nullParam;
	}

	if ((m_calcParams.antGainCalced) && \
		((NULL == m_pAntLibrary) || (NULL == m_pAntLibrary->GetAntenna(pAntennaParam->antModel.c_str()))))
	{
		for (std::shared_ptr<UCell> cell : cells) {
			ULog_Write(ULOG_ERROR, "The antenna of cell(%d) is not found in the library!", cell->cellID.c_str());
		}
		return nullParam;
	}

	auto antCalcParam = std::make_shared< AntPathLossCalcParam >();
	antCalcParam->pAntennaGainParam = pAntennaParam;
	antCalcParam->StartAngle = 0;
	antCalcParam->EndAngle = 360;
	antCalcParam->TransId = cells.front()->parent->id;
	antCalcParam->X = pAntennaParam->x;
	antCalcParam->Y = pAntennaParam->y;
	antCalcParam->Frequency = cells.front()->dlFreq;
	
	GetSectorsCalcPara(antCalcParam, cells);

	if (!CheckParam(antCalcParam)) {
		antCalcParam.reset();
		return antCalcParam;
	}
	
	UnifyRayTracingParam(antCalcParam);

	return antCalcParam;
}

void CalcParamFactory::GetSectorsCalcPara(std::shared_ptr<AntPathLossCalcParam> antCalcParam,
	const std::list<std::shared_ptr<UCell>>& cells)
{
	if (!antCalcParam) {
		ULog_Write(ULOG_ERROR, "antCalcParam can't be null!!!");
		return;
	}
	
	for (auto cell : cells)
	{
		antCalcParam->SectorCalcParams.push_back(GetSectorParam(cell, antCalcParam));
	}
}

std::shared_ptr<SectorPathLossCalcParam> CalcParamFactory::GetSectorParam(\
	std::shared_ptr<UCell> cell, std::shared_ptr<AntPathLossCalcParam> antCalcParam)
{
	auto sectorParam = std::make_shared<SectorPathLossCalcParam>();
	sectorParam->AntCalcParam = antCalcParam;
	sectorParam->Cell = cell;
	sectorParam->X = antCalcParam->X;
	sectorParam->Y = antCalcParam->Y;
	sectorParam->TxAntennaHeight = antCalcParam->pAntennaGainParam->heightToGround;
	sectorParam->RxAntennaHeight = m_calcParams.ueHeight;
	sectorParam->StartAngle = antCalcParam->StartAngle;
	sectorParam->EndAngle = antCalcParam->EndAngle;
	sectorParam->MergeEdgeMaxDis = m_calcParams.edgeDistanceOfMerged;
	sectorParam->DlFrequency = cell->dlFreq;
	sectorParam->UlFrequency = cell->ulFreq;
	sectorParam->BandWidth = cell->bandWidth;
	sectorParam->threadCount = m_calcParams.threadCount;
	sectorParam->PeneLossMode = m_calcParams.peneLossMode;
	sectorParam->AntGainCalced = m_calcParams.antGainCalced;
	upl_bzero(&(sectorParam->sceneFeature), sizeof(PathlossSceneFeature));
	
	this->SetSectorCalcParam(sectorParam);

	auto mainSectionParam = sectorParam->SectionParamArray.front();
	sectorParam->MainCalcRadius = mainSectionParam->CalcEndDistance;
	
	//总的计算精度和主传模计算精度，取小值。
	double reqResolution = m_calcParams.calcResolution;
	sectorParam->MainResolution = ((reqResolution >= PATHLOSS_MINRESOLUTON)
		&& (reqResolution <= PATHLOSS_MAXRESOLUTON)) ? reqResolution : PATHLOSS_MAXRESOLUTON;
	if ((mainSectionParam->CalcResolution < sectorParam->MainResolution) && (mainSectionParam->CalcResolution >= PATHLOSS_MINRESOLUTON))
	{
		sectorParam->MainResolution = mainSectionParam->CalcResolution;
	}

	sectorParam->MainAngleRes = ((m_calcParams.angleRes >= PATHLOSS_MINANGLERESOLUTON)
		&& (m_calcParams.angleRes <= PATHLOSS_MAXANGLERESOLUTON)) ? m_calcParams.angleRes \
		: PATHLOSS_MAXANGLERESOLUTON;
	if ((mainSectionParam->AngleResolution < sectorParam->MainAngleRes) \
		&& (mainSectionParam->AngleResolution >= PATHLOSS_MINANGLERESOLUTON))
	{
		sectorParam->MainAngleRes = mainSectionParam->AngleResolution;
	}
	
	return sectorParam;
}

void CalcParamFactory::SetSectorCalcParam(\
	std::shared_ptr<SectorPathLossCalcParam> sectorParam)
{
	double startRadius = 0;
	int i = 0;
	for (PropModelConfig& pmc : sectorParam->Cell->propModels)
	{
		auto sectionParam = this->CreateSectionParam(sectorParam, startRadius, pmc);
		if ((i > 0) \
			&& (sectionParam->PropagModel->PropModelType == RAYTRACING_MODEL))
		{
			continue;       //射线跟踪模型只能是一个sector的主模型，否则无效
		}
		sectionParam->CalcEndDistance = (pmc.calcRadius < EPSILON) ? \
			m_calcParams.calcRadius + startRadius : pmc.calcRadius + startRadius;
		startRadius = sectionParam->CalcEndDistance;
		/*double angle = (((sectionParam->CalcResolution / sectionParam->CalcEndDistance) * 180.0) / PI);
		if (sectionParam->AngleResolution < angle)
			sectionParam->AngleResolution = angle;*/
		sectionParam->ClutterParamsManager = this->GetClutterParamsManager();
		
		sectorParam->SectionParamArray.push_back(sectionParam);
		i++;
	}	
}

std::shared_ptr<SectionPathLossCalcParam> CalcParamFactory::CreateSectionParam(\
	std::shared_ptr<SectorPathLossCalcParam> sectorParam, \
	double startRadius, PropModelConfig& pmc) const
{
	auto param = std::make_shared<SectionPathLossCalcParam>();
	param->threadCount = m_calcParams.threadCount;
	param->CalcStartDistance = startRadius;
	double reqResolution = m_calcParams.calcResolution;
	//总的计算精度和各传模自配的计算精度，取小值。
	param->CalcResolution = ((reqResolution >= PATHLOSS_MINRESOLUTON)
		&& (reqResolution <= PATHLOSS_MAXRESOLUTON)) ? reqResolution : PATHLOSS_MAXRESOLUTON;
	if ((pmc.calcResolution < param->CalcResolution) && (pmc.calcResolution >= PATHLOSS_MINRESOLUTON))
	{
		param->CalcResolution = pmc.calcResolution;
	}

	param->AngleResolution = ((m_calcParams.angleRes >= PATHLOSS_MINANGLERESOLUTON)
		&& (m_calcParams.angleRes <= PATHLOSS_MAXANGLERESOLUTON)) ? m_calcParams.angleRes \
		: PATHLOSS_MAXANGLERESOLUTON;
	if ((pmc.calcAngleRes < param->AngleResolution) && (pmc.calcAngleRes >= PATHLOSS_MINANGLERESOLUTON))
	{
		param->AngleResolution = pmc.calcAngleRes;
	}
	
	param->TxAntennaHeight = sectorParam->TxAntennaHeight;
	param->RxAntennaHeight = sectorParam->RxAntennaHeight;
	param->X = sectorParam->X;
	param->Y = sectorParam->Y;
	param->StartAngle = sectorParam->StartAngle;
	param->EndAngle = sectorParam->EndAngle;
	param->MergeEdgeMaxDis = sectorParam->MergeEdgeMaxDis;
	param->Frequency = sectorParam->DlFrequency;
	param->BandWidth = sectorParam->BandWidth;
	if (m_calcParams.MaxDistanceOfVS > 0)
		param->MaxDistanceOfVS = m_calcParams.MaxDistanceOfVS;
	param->usingBuildingPolygon = m_calcParams.usingBuildingPolygon;
	param->microThreshold = m_calcParams.microThreshold;
	param->macroThreshold = m_calcParams.macroThreshold;
	param->macroMinAntHeight = m_calcParams.macroMinAntHeight;
	param->microMaxRadius = m_calcParams.microMaxRadius;
	param->ShadowFadingForOutdoor = m_calcParams.shadowFadingForOutdoor;
	param->ShadowFadingForIndoor = m_calcParams.shadowFadingForIndoor;
	param->AntGainCalced = sectorParam->AntGainCalced;
	param->PeneLossMode = sectorParam->PeneLossMode;
	//param->calcHeights.assign(m_calcParams.calcHeights.begin(), m_calcParams.calcHeights.end());
	param->PropagModel = m_pPropModelFactory->GetPropagationModel(pmc.HashValue(), sectorParam->Cell);
	
	param->SectorCalcParam = sectorParam;

	return param;
}