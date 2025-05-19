#include "PathLossofIndoorEngine.h"
#include "UPlanCommonExport.h"
#include "IIndoorGeoInfo.h"
#include "IndoorAPLoader.h"
#include "AntCalcGain.h"
#include "LiShuttleIndoorCalculator.h"

PathLossofIndoorEngine::PathLossofIndoorEngine(IServiceHelper* pServiceHelper) \
	: AppEngine(pServiceHelper, PATHLOSS_INDOOR_SERVICE), m_stopFlag(false), m_taskId(""), m_raySearcher(nullptr)
{
	if (m_pServiceHelper)
		m_pServiceHelper->AddService(PATHLOSS_INDOOR_SERVICE, (void*)this);
}

PathLossofIndoorEngine::~PathLossofIndoorEngine()
{
	if (m_pServiceHelper)
		m_pServiceHelper->RemoveService(PATHLOSS_INDOOR_SERVICE);
}

const char* PathLossofIndoorEngine::GetResultFolder() const
{
	return m_calcParams.pathlossDirectory.c_str();
}

bool PathLossofIndoorEngine::GetApPathLoss(APPathLossInBuilding& apPathLoss) const
{
	return LiShuttleIndoorCalculator::ReadApPathLoss(apPathLoss, m_calcParams.pathlossDirectory.c_str());
}

bool PathLossofIndoorEngine::Init()
{
	if (!SetupPathLossofIndoorParams(&m_calcParams, m_pServiceHelper->GetRuntimeSchema(), \
		m_pServiceHelper->GetCommonParams(), m_pServiceHelper->GetAppFolder())) {
		ULog_Write(ULOG_FATAL, "Error occurs when parsing the xml file for pathloss of indoor!");
		return false;
	}

	return true;
}

void PathLossofIndoorEngine::Run(const char* taskId)
{
	m_taskId = taskId;

	if (!Init())
		return;

	IndoorAPLoader* pIndoorAps = (IndoorAPLoader*)(m_pServiceHelper->GetService(INDOOR_AP_PARAMS_SERVICE));
	if (!pIndoorAps) {
		ULog_Write(ULOG_ERROR, "No indoor APs!");
		return;
	}

	if (!m_raySearcher)
		m_raySearcher = new LiShuttleIndoorCalculator( m_calcParams, \
			(AntCalcGain*)(m_pServiceHelper->GetService(ANTENNA_CALC_SERVICE)), \
			(IMaterialFinder*)(m_pServiceHelper->GetService(MATERIAL_FINDER_SERVICE)) );

	ApList apList;
	pIndoorAps->GetAllAps(apList);

	ULog_Write(ULOG_INFO, "Pathloss calculation begins(Engine %s).", m_taskId.c_str());

	Calculate(apList);
	apList.clear();

	ULog_Write(ULOG_INFO, "Pathloss calculation ends(Engine %s).", m_taskId.c_str());
}

void PathLossofIndoorEngine::SetRayCalcParams(IndoorAPRayCalcParams& apRayParams, const IGeoBuildingFloorPtr pBuildingFloor, \
	const std::shared_ptr<UAp> ap) const
{
	apRayParams.ap = ap;
	apRayParams.h = (ap->antInstParams.heightToGround > pBuildingFloor->GetFloorHeight()) ? pBuildingFloor->GetFloorHeight() : ap->antInstParams.heightToGround;
	apRayParams.resolution = m_calcParams.calcResolution;
	for (auto cell : ap->cells)
		if (cell->propModel.calcRadius >= 0.1 && apRayParams.resolution > cell->propModel.calcRadius)
			apRayParams.resolution = cell->propModel.calcRadius;
}

void PathLossofIndoorEngine::Calculate(const ApList& apList) const
{
	if (apList.empty()) {
		ULog_Write(ULOG_ERROR, "No indoor APs!");
		return;
	}

	IIndoorGeoInfo* pIndoorGeo = (IIndoorGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_INDOOR_SERVICE));
	if (!pIndoorGeo || pIndoorGeo->GetBuildingCount() <= 0) {
		ULog_Write(ULOG_ERROR, "No indoor geo service!");
		return;
	}

	GeoBuilding* pBuilding = pIndoorGeo->GetBuilding(0);
	if (!pBuilding) {
		ULog_Write(ULOG_ERROR, "No buildings for calculating indoor pathloss!");
		return;
	}
	DiffractionTreeManager diffTreeManager(pBuilding, m_calcParams.lengthError, m_calcParams.minWallLengthInLos);

#if false
	//测试：输出建筑物内墙到文本文件
	std::string filePath = m_calcParams.pathlossDirectory + "/building.txt";
	pBuilding->Export(filePath.c_str());
#endif

	for (auto ap : apList) {
		if (m_stopFlag)
			break;

		auto pBuildingFloor = pBuilding->GetBuildingFloor(ap->floor);
		if (!pBuildingFloor) {
			ULog_Write(ULOG_ERROR, "No data of the floor %d for AP %s!", ap->floor, ap->apID.c_str());
			continue;
		}

		auto boundary = pBuildingFloor->GetBoundary();
		if (!boundary->IsPointInPolygon(GeoXYPoint(ap->X, ap->Y))) {
			ULog_Write(ULOG_ERROR, "The ap %s isn't in the region of the floor %d!", ap->apID.c_str(), ap->floor);
			continue;
		}

		IndoorAPRayCalcParams rayCalcParams;
		SetRayCalcParams(rayCalcParams, pBuildingFloor, ap);

		ULog_Write(ULOG_INFO, "Calculate the rays of AP %s", ap->apID.c_str());

		CalculateApRay(rayCalcParams, pBuilding, &diffTreeManager);

		ULog_Write(ULOG_INFO, "Rays calculation of AP %s ends.", ap->apID.c_str());
	}
}

void PathLossofIndoorEngine::CalculateApRay(const IndoorAPRayCalcParams& apRayParams, \
	const GeoBuilding* pBuilding, IDiffTreeManager* pDiffTreeManager) const
{
	m_raySearcher->Run(apRayParams, pBuilding, pDiffTreeManager);
}
