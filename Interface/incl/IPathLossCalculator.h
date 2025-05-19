#pragma once

#include "SectorPathlossData.h"
#include "PropagationModel.h"
#include <memory>

class PROPINF_EXPORT_CLASS IPathLossCalculator
{
public:
	virtual ~IPathLossCalculator() {};

	virtual float CalcDeltaLossFrmDLToUL(const std::shared_ptr<PropagationModel> prop, \
		float dlFreq, float ulFreq, double rxHeight) const = 0;

	virtual void CalculateSectionPathLoss(SectorPathLossData* pLosData, \
		const std::shared_ptr<SectionPathLossCalcParam> sectionParam, \
		bool* pStopCalculator) = 0;

	virtual const char* CalculatorName() const = 0;
};
