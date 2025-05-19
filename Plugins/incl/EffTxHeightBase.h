#pragma once

#include "PathLossCalcParams.h"
#include <memory>

class EffTxHeightBase
{
public:
	virtual void CalcRayEffTxHt(double* pEffTxHeight, \
		std::shared_ptr<SectionPathLossCalcParam> param, \
		const short* pAltitudes, int startIndex, int endIndex);

	virtual const char* EffectTxHeightMethodName() const = 0;

	void JudgeEffHeight(double& midEffHeights) const;

public:
	EffTxHeightBase();
	virtual ~EffTxHeightBase();

protected:
	double m_TxHeight;
};
