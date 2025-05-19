#pragma once

#include "PropPlugins.h"
#include"DiffractionLossBase.h"

class AtlasDiffractionLoss :public DiffractionLossBase
{
public :
	AtlasDiffractionLoss();
		
    double DoublePeakLoss( const MultiPeakCalcParam& calcPara) const;

	double TriplePeakLoss(const MultiPeakCalcParam& calcuParam) const;

	const char* DifLosMethodName() const;
	
};
