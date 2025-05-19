#pragma once

#include "PropagationTypes.h"
#include "CalcParamFactory.h"
#include "PropagationCalculator.h"
#include "PMAITrainingParams.h"
#include "CellLoader.h"
#include "AIGridFeatureProbe.h"
#include "CalibrationParams.h"
#include "AppEngine.h"
#include <list>
#include <memory>
#include <string>

class PROPFRAME_EXPORT_CLASS PMAITrainingEngine : public AppEngine
{
public:
	PMAITrainingEngine(IServiceHelper* pServiceHelper);
	virtual ~PMAITrainingEngine();

	void CreateFeature();

	virtual void Run(const char* taskId);

	virtual const char* GetResultFolder() const;

	void PrintDebugInfo();
	
	void GetFullPathFiles(std::string& fullPath, const char* relativePath) const;

	const char* GetFeaturePath() const;
	const char* GetPythonSchema() const;

protected:
	virtual bool Init();

	void SaveAIFeature();
	void CalculateRangeCount();
	void GetAllFiles(std::string pathDir, std::vector<std::string>& files);

public:
	PMAITrainingParams m_calcParams;
	CalcParamFactory* m_pCalcParamFactory;
	AIGridFeatureProbe* m_pAIGridFeatureProbe;
	LiShuttleAICalculator* m_pCalculator;
	std::string m_taskId;
};