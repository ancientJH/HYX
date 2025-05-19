#pragma once

#include "PropFrame.h"
#include "NEIndoorTypes.h"
#include "PropagationTypes.h"
#include "FileAccesser.h"
#include "AppEngine.h"
#include "PathLossCalcParams.h"
#include "IIndoorGeoInfo.h"
#include "IGeoBuildingFloor.h"
#include "DiffractionTreeManager.h"
#include "LiShuttleIndoorCalculator.h"

class PROPFRAME_EXPORT_CLASS PathLossofIndoorEngine : public AppEngine
{
public:
	PathLossofIndoorEngine(IServiceHelper* pServiceHelper);
	virtual ~PathLossofIndoorEngine();

	virtual void Run(const char* taskId);

	virtual const char* GetResultFolder() const;

	bool GetApPathLoss(APPathLossInBuilding& apPathLoss) const;

protected:
	virtual bool Init();

	void SetRayCalcParams(IndoorAPRayCalcParams& apRayParams, const IGeoBuildingFloorPtr pBuildingFloor, const std::shared_ptr<UAp> ap) const;

	void Calculate(const ApList& apList) const;

	void CalculateApRay(const IndoorAPRayCalcParams& apRayParams, \
						const GeoBuilding* pBuilding, IDiffTreeManager* pDiffTreeManager) const;

public:
	bool m_stopFlag;
	PathlossofIndoorParams m_calcParams;
	LiShuttleIndoorCalculator* m_raySearcher;
	FileAccesser m_fileAccesser;
	std::string m_taskId;
};