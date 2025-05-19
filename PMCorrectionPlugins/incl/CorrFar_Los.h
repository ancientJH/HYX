#pragma once
#include "ParamCorrectBase.h"

class CorrFar_Los:ParamCorrectBase
{
public:
	CorrFar_Los();
	~CorrFar_Los();

	virtual void Init(InterParam& interParam, std::shared_ptr<LiShuttlePropagationModel> LiModel, bool isCaliScene);

private:
	virtual int CalInterParam(InterParam& interParam, CellCorrectionData* cellData, std::shared_ptr<LiShuttlePropagationModel> LiModel);

	virtual CALRESULT_TYPE UpdateModel(InterParam& interParam, \
		std::shared_ptr<LiShuttlePropagationModel> LiModel, const ModelRestrictions& modelRes, \
		bool isCaliScene, bool forceCal = false);
};