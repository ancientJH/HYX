#pragma once
#include "ParamCorrectBase.h"

class CorrFar_nLos : public ParamCorrectBase
{
public:
	CorrFar_nLos();
	~CorrFar_nLos();

	virtual void Init(InterParam& interParam, std::shared_ptr<LiShuttlePropagationModel> LiModel, bool isCaliScene);

	virtual CALRESULT_TYPE CorrectModel(const std::list<CellCorrectionData*>& cellDatas, \
		std::shared_ptr<LiShuttlePropagationModel> LiModel, const ModelRestrictions& modelRes, bool isCaliScene = false, bool forceCal = false);

private:
	virtual int CalInterParam(InterParam& interParam, CellCorrectionData* cellData, std::shared_ptr<LiShuttlePropagationModel> LiModel);

	virtual CALRESULT_TYPE UpdateModel(InterParam& interParam, \
		std::shared_ptr<LiShuttlePropagationModel> LiModel, const ModelRestrictions& modelRes,  \
		bool isCaliScene, bool forceCal = false);
};