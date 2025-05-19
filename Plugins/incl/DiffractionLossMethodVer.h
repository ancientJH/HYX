#pragma once

#include "PropPlugins.h"
#include "LiShuttlePathLossBase.h"
#include "LiShuttlePropagationModel.h"

class PROPPLUGINS_EXPORT_CLASS DiffractionLossMethodVer : LiShuttlePathLossBase
{
public:
	DiffractionLossMethodVer();

	/*static double CalcStrength(const TRParam& trParam, double diffLoss, \
						const LiShuttleModelParams& propModel);*/

	static double CalcPower(const TRParam& trParam, double diffCoeff, \
		const LiShuttleModelParams& propModel);

	//virtual double CalcUnitStrength(CalcPathLossParam& calcParam) const ;

	virtual double CalcUnitPower(CalcPathLossParam& calcParam) const;

private:
	double GetPTPDiffractionLoss(double weveLen,double txHeight, double rxHeight, \
		std::list<PeakParam>& peaks, const LiShuttleModelParams& propModel) const;

private:
	DiffractionLossFactory* m_pDiffLossFactory;
};