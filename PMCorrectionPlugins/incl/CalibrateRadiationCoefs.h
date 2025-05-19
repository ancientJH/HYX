#pragma once

#include "ParamCorrectBase.h"
#include <list>

//校正电磁波扩散系数，即K0, Knear, Kfar
class CalibrateRadiationCoefs : public ParamCorrectBase
{
public:
	CalibrateRadiationCoefs();
	~CalibrateRadiationCoefs();

	virtual void Init(InterParam& interParam, std::shared_ptr<LiShuttlePropagationModel> LiModel, bool isCaliScene);

	virtual int CalInterParam(InterParam& interParam, CellCorrectionData* cellData, std::shared_ptr<LiShuttlePropagationModel> LiModel);

	virtual CALRESULT_TYPE UpdateModel(InterParam& interParam, \
		std::shared_ptr<LiShuttlePropagationModel> LiModel, const ModelRestrictions& modelRes,  \
		bool isCaliScene, bool forceCal = false);

protected:
	void CalculateCorrelations(std::shared_ptr<LiShuttlePropagationModel> LiModel, const InterParam& interParam) const;

private:
	size_t m_farCount;
	double m_sumOfY2;
	double m_squareSumOfY2;
};

