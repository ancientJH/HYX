#pragma once

#include "PropFrame.h"
#include "PropagationTypes.h"
#include "CalibrationParams.h"
#include "LiShuttleTypes.h"
#include "CalcParamFactory.h"
#include "FileOperator.h"
#include "CellLoader.h"
#include "NETypes.h"
#include "PMCorrectionData.h"
#include "GeoCoordinate.h"
#include <list>
#include <string>
#include <map>
#include "AppEngine.h"
#include "CorrectCalculator.h"
#include "PMCorrectionGrid.h"
#include "CellCorrectionData.h"
#include "MeasuredDataAnalyzer.h"
#include "IPathLossAccesser.h"
#include <LiShuttleCellModel.h>
#include "tinyxml2.h"

class PROPFRAME_EXPORT_CLASS PMCorrectionEngine : public AppEngine
{
public:
	PMCorrectionEngine(IServiceHelper* pServiceHelper);
	virtual ~PMCorrectionEngine();

	void PrintDebugInfo();

	virtual void Run(const char* taskId);

	virtual const char* GetResultFolder() const;

	//滤除背向天线及无主径数据的栅格
	//若diffloss和refloss为1 则转为直射径
	void FilterGridData();

	//校正结果保存 
	void SaveModels();
	void SaveCellModels();
	void SaveCellMeanVariance();
	void SaveCommonModel();		//通用模型  保存至models文件夹内
	
	//模型保存
	size_t GetModelDirectory(char* szModelDir, size_t len);
	int AddNewModelToXMLFile(const char* xmlFileName, const char* modelName, std::shared_ptr<LiShuttlePropagationModel> model);
	XMLElement* CreateNewNode(tinyxml2::XMLDocument* doc, const std::shared_ptr<LiShuttlePropagationModel> model, int id, const char* modelName) const;
	XMLElement* CreateSceneNewNode(tinyxml2::XMLDocument* doc, std::shared_ptr<LiShuttleSceneSubModel> model, XMLElement* newModel) const;
	bool IsNeededAddModel(const std::list<std::shared_ptr<PropagationModel>>& models, std::shared_ptr<LiShuttlePropagationModel> model) const;
	bool IsModelEqual(std::shared_ptr<LiShuttlePropagationModel> model1, std::shared_ptr<LiShuttlePropagationModel> model2) const;
	void GetPathLossModelName(char* fileName, size_t len);

	void GetAllFiles(const char* dtDir);

	//分析结果输出（内部使用，非必须）
	void ExportCellSummary() const;
	void ExportSceneSummary() const;
	void ExportAllRayStrength() const;
	void ExportProcessData() const;//导出所所有预处理之后的路测数据

protected:
	virtual bool Init();

protected:
	PMCorrectionCalcParam m_calcParams;
	std::vector<std::string> m_dtDataFiles;
	PMCorrectionData* m_pPMCorrData;		/*校正数据集*/
	std::string m_taskId;
	CorrectCalculator* m_pCorCalculator;  /*校正计算器*/
};