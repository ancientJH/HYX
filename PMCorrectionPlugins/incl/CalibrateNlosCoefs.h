#pragma once

#include "ParamCorrectBase.h"
#include <list>

//У����Ų���ɢϵ������K0, Knear, Kfar
class CalibrateNlosCoefs : public ParamCorrectBase
{
public:
	CalibrateNlosCoefs();
	~CalibrateNlosCoefs();

	virtual void Init(InterParam& interParam, std::shared_ptr<LiShuttlePropagationModel> LiModel, bool isCaliScene);

	virtual int CalInterParam(InterParam& interParam, CellCorrectionData* cellData, std::shared_ptr<LiShuttlePropagationModel> LiModel);

	virtual CALRESULT_TYPE UpdateModel(InterParam& interParam, \
		std::shared_ptr<LiShuttlePropagationModel> LiModel, const ModelRestrictions& modelRes,  \
		bool isCaliScene, bool forceCal = false);

protected:
	void CalculateCorrelations(std::shared_ptr<LiShuttlePropagationModel> LiModel, const InterParam& interParam) const;

private:
	size_t m_diffCount;
	double m_sumOfYDiff;
	double m_squareSumOfYDiff;
	size_t m_reflCount;
	double m_sumOfYRefl;
	double m_squareSumOfYRefl;
};

