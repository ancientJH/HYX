#pragma once
#include <vector>
#include <string>
#include "PathLossCalcParams.h"
#include "EffTxHeightBase.h"
#include "GeoTypes.h"
using namespace std;

class EffectTxAboveTheAverageGround:public EffTxHeightBase {
	public:
		void CalcRayEffTxHt(double* pEffTxHeight, std::shared_ptr<SectionPathLossCalcParam> param, const short* pAltitudes, int startIndex, int endIndex);
		const char* EffectTxHeightMethodName() const;
};
