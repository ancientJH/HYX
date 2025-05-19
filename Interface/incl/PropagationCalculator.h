#pragma once

#include "PropInterface.h"
#include "IPathLossCalculator.h"
#include "AntCalcGain.h"
#include "IGeoInfo.h"
#include "PathLossCalcParams.h"
#include "SamplingSpot.h"
#include "ScanningLineInfo.h"
#include "GisPreLoadData.h"
#include "GeoBuildingPolygon.h"
#include <memory>
#include <map>

class PROPINF_EXPORT_CLASS PropagationCalculator : public IPathLossCalculator
{
public:
	virtual ~PropagationCalculator();

	virtual float CalcDeltaLossFrmDLToUL(const std::shared_ptr<PropagationModel> prop, \
		float dlFreq, float ulFreq, double rxHeight) const = 0;

	virtual void CalculateSectionPathLoss(SectorPathLossData* pLosData, \
		const std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
		bool* pStopCalculator) = 0;

	virtual const char* CalculatorName() const = 0;

protected:
	PropagationCalculator();
	
	virtual void InitScanningLine(ScanningLineInfo& scanningLineInfo, \
		const std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
		const GeoXYLine& res2D) const;

	void SetSpotInfo(SamplingSpot* pSpot, int index, \
		const short* pAltitudes, const short* pClutters, const short* pBuildHeigts, \
		const std::vector<GeoBuildingPtr>& buidlingPolygons) const;

	std::shared_ptr<GisPreLoadData> PreLoadGisData(\
		const std::shared_ptr<SectionPathLossCalcParam> sectionParam, DemDataType dataType) const;

	void LoadGisDataOnLine(std::map<DemDataType, short*>& gisDataOnLine,\
		std::vector<GeoBuildingPtr>& buildingPolygons, \
		const std::map<DemDataType, std::shared_ptr<GisPreLoadData>>& gisData, \
		const std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
		const GeoXYPoint& deltaPt) const;

	bool JudgeBinValid(const SamplingSpot* spotInfo) const;

	float GetPTPAntGain(const std::shared_ptr<SectionPathLossCalcParam> sectionCalcParam,\
		double msAzimuth, double msElevation) const;

	void CorrectAltitudes(std::map<DemDataType, short*>& gisDataOnLine, int length);

public:
	AntCalcGain* m_pAntCalcGain;
	IGeoInfo* m_pGeoInfo;
	PathlossCalcParams m_calcParams;
	bool m_hasInvalidGrid;
};