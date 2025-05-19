#pragma once

#include "PropAIPlugins.h"
#include "PathLossCalcParams.h"
#include "PropagationCalculator.h"
#include "PropagationModel.h"
#include "RadialSectionSpotData.h"
#include "RadialPathlossData.h"
#include "FileAccesser.h"
#include <vector>
#include <memory>
#include <string>

class PROPAIPLUGINS_EXPORT_CLASS LiShuttleAICalculator : public PropagationCalculator
{
public:
	
	LiShuttleAICalculator();
	virtual ~LiShuttleAICalculator();

	virtual float CalcDeltaLossFrmDLToUL(const std::shared_ptr<PropagationModel> prop, \
		float dlFreq, float ulFreq, double rxHeight) const;

	virtual const char* CalculatorName() const;

	virtual void CalculateSectionPathLoss(SectorPathLossData* pLosData, \
		std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
		bool* pStopCalculator);

	CellCategory CalculateSectionFeature(RadialSectionSpotData* pSpotData, std::shared_ptr<SectionPathLossCalcParam> sectionParam);

protected:
	virtual void InitScanningLine(ScanningLineInfo& scanningLineInfo, \
			const std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
			const std::map<DemDataType, short*>& gisDataOnLine, \
			const std::vector<GeoBuildingPtr>& buidlingPolygons,\
			GeoXYLine& res2D) const;
	void SaveFeature(RadialSectionSpotData* pSpotData, const char* featurepath);
	void CalculateLinePathLoss(std::shared_ptr<RadialPathLossData> sectionPathLoss, \
	const std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
	const ScanningLineInfo* pScanningLine, int rayIndex, float** predictedPL) const;

	losstype GetPathLossOnSpot(char* gridFeature) const;

	void GetSectionPathloss(std::shared_ptr<SectionPathLossCalcParam> sectionParam, const char* featurepath, float** result) const;

	bool InitializePy(const char* feature, const char* cellId);
	void FinalizePy();

private:
	void* m_pPyModule;
	void* m_pPyFunc;
	std::string m_modelName;
};

void PROPAIPLUGINS_EXPORT_FUNC executeCMD(const char* cmd, float** result, \
	std::shared_ptr<SectionPathLossCalcParam> sectionParam);

void PROPAIPLUGINS_EXPORT_FUNC executeCMD(const char* cmd, char* result, int len);