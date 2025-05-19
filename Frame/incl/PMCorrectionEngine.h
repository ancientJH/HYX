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

	//�˳��������߼����������ݵ�դ��
	//��diffloss��reflossΪ1 ��תΪֱ�侶
	void FilterGridData();

	//У��������� 
	void SaveModels();
	void SaveCellModels();
	void SaveCellMeanVariance();
	void SaveCommonModel();		//ͨ��ģ��  ������models�ļ�����
	
	//ģ�ͱ���
	size_t GetModelDirectory(char* szModelDir, size_t len);
	int AddNewModelToXMLFile(const char* xmlFileName, const char* modelName, std::shared_ptr<LiShuttlePropagationModel> model);
	XMLElement* CreateNewNode(tinyxml2::XMLDocument* doc, const std::shared_ptr<LiShuttlePropagationModel> model, int id, const char* modelName) const;
	XMLElement* CreateSceneNewNode(tinyxml2::XMLDocument* doc, std::shared_ptr<LiShuttleSceneSubModel> model, XMLElement* newModel) const;
	bool IsNeededAddModel(const std::list<std::shared_ptr<PropagationModel>>& models, std::shared_ptr<LiShuttlePropagationModel> model) const;
	bool IsModelEqual(std::shared_ptr<LiShuttlePropagationModel> model1, std::shared_ptr<LiShuttlePropagationModel> model2) const;
	void GetPathLossModelName(char* fileName, size_t len);

	void GetAllFiles(const char* dtDir);

	//�������������ڲ�ʹ�ã��Ǳ��룩
	void ExportCellSummary() const;
	void ExportSceneSummary() const;
	void ExportAllRayStrength() const;
	void ExportProcessData() const;//����������Ԥ����֮���·������

protected:
	virtual bool Init();

protected:
	PMCorrectionCalcParam m_calcParams;
	std::vector<std::string> m_dtDataFiles;
	PMCorrectionData* m_pPMCorrData;		/*У�����ݼ�*/
	std::string m_taskId;
	CorrectCalculator* m_pCorCalculator;  /*У��������*/
};