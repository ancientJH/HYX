#pragma once
#include "ParamCorrectBase.h"


class CorrNear_Los:ParamCorrectBase
{
public:
	CorrNear_Los();
	~CorrNear_Los();

	virtual void Init(InterParam& interParam, std::shared_ptr<LiShuttlePropagationModel> LiModel, bool isCaliScene);

private:
	virtual int CalInterParam(InterParam& interParam, CellCorrectionData* cellData, std::shared_ptr<LiShuttlePropagationModel> LiModel);

	virtual CALRESULT_TYPE UpdateModel(InterParam& interParam, \
		std::shared_ptr<LiShuttlePropagationModel> LiModel, const ModelRestrictions& modelRes,  \
		bool isCaliScene, bool forceCal = false);
};

