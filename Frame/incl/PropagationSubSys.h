#pragma once

#include "PropFrame.h"
#include "UPlanCommon.h"
#include "PropagationModel.h"
#include "LiShuttleCellModel.h"
#include "IAppEngine.h"
#include "PathLossCalcEngine.h"
#include "PMAITrainingEngine.h"
#include "PMCorrectionEngine.h"
#include "PathLossofIndoorEngine.h"
#include "MeasuredDataEngine.h"
#include "ICellParamsAnalyzer.h"
#include "SceneClassfier.h"
#include <memory>
#include <map>
#include <string>

class PROPFRAME_EXPORT_CLASS PropagationSubSys : public ILiShuttleSubSys
{
public:
	PropagationSubSys();
	virtual ~PropagationSubSys();

public:
	virtual bool Init(IServiceHelper* pServiceHelper);

	virtual bool Init2();

	virtual void Exit();

protected:
	void LoadModels();

	void ClearEngines();

	void CalibrateCellParams() const;

private:
	IServiceHelper* m_pServiceHelper;
	PathLossCalcEngine* m_pPathLossCalc;
	PMCorrectionEngine* m_pCalibratingCalc;
	PMAITrainingEngine* m_pPMAICalc;
	PathLossofIndoorEngine* m_pPathLossOfIndoorCalc;
	MeasuredDataEngine* m_pDtMeasEngine;
	ICellParamsAnalyzer* m_pPropModelAnalyzer;
	SceneClassfier* m_pSceneClassfier;

	std::shared_ptr<LiShuttleCellModel> m_cellModel;
	std::shared_ptr<LiShuttleSceneModel> m_sceneModel;
};