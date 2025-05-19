#pragma once
#include <vector>
#include <string>
#include "PathLossCalcParams.h"
#include "EffTxHeightBase.h"
#include <stdio.h>
#include <math.h>
#include "GeoTypes.h"
using namespace std;
class EffectTxHeightHtAbsSpot : public EffTxHeightBase {
public:
	void CalcRayEffTxHt(double* pEffTxHeight, std::shared_ptr<SectionPathLossCalcParam> param, const short* pAltitudes, int startIndex, int endIndex);
	const char* EffectTxHeightMethodName() const;
};
