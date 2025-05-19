#include "PropagationSubSys.h"
#include "LiShuttlePathLossFactory.h"
#include "PropagationModelFactory.h"
#include "LiShuttlePropagationModel.h"
#include "LiShuttleAIModel.h"
#include "LiShuttleSceneModel.h"
#include "PropModelAnalyzer.h"

PropagationSubSys::PropagationSubSys() : m_pServiceHelper(NULL), \
	m_pPathLossCalc(NULL), m_pCalibratingCalc(NULL), m_pPMAICalc(NULL), m_pPathLossOfIndoorCalc(NULL), \
	m_pDtMeasEngine(nullptr), m_pSceneClassfier(nullptr)
{
	PropagationModelFactory* propFactory = PropagationModelFactory::instance();
	if (propFactory) {
		propFactory->AddModelCreater(LiShuttlePropagationModel::CategoryName, \
			&(LiShuttlePropagationModel::CreatePropagationModel));

		propFactory->AddModelCreater(LiShuttleAIModel::CategoryName, \
			& (LiShuttleAIModel::CreatePropagationModel));
	}
}

PropagationSubSys::~PropagationSubSys()
{
	Exit();
}

void PropagationSubSys::ClearEngines()
{
	if (m_pPathLossCalc) {
		delete m_pPathLossCalc;
		m_pPathLossCalc = NULL;
	}

	if (m_pCalibratingCalc) {
		delete m_pCalibratingCalc;
		m_pCalibratingCalc = NULL;
	}

	if (m_pPMAICalc) {
		delete m_pPMAICalc;
		m_pPMAICalc = NULL;
	}

	if (m_pPathLossOfIndoorCalc) {
		delete m_pPathLossOfIndoorCalc;
		m_pPathLossOfIndoorCalc = NULL;
	}

	if (m_pDtMeasEngine) {
		delete m_pDtMeasEngine;
		m_pDtMeasEngine = nullptr;
	}

	if (m_pSceneClassfier) {
		delete m_pSceneClassfier;
		m_pSceneClassfier = nullptr;
	}
}

bool PropagationSubSys::Init2()
{
	ClearEngines();

	IGeoInfo* pGeoInfo = (IGeoInfo*)(m_pServiceHelper->GetService(GEOGRAPHIC_SERVICE_NAME));
	if (pGeoInfo) {
		//加载一站一模
		if (m_cellModel)
			m_cellModel->Clear();
		else {
			char szModelFile[512];
			PropagationModelFactory::GetCellModelFile(szModelFile, 512, pGeoInfo->GetMapName());
			m_cellModel = std::make_shared<LiShuttleCellModel>(szModelFile);
		}
		PropagationModelFactory::instance()->LoadCellModels(m_cellModel, pGeoInfo->GetMapName());

		CalibrateCellParams();
	}

	m_pPathLossCalc = new PathLossCalcEngine(m_pServiceHelper);
	
	m_pCalibratingCalc = new PMCorrectionEngine(m_pServiceHelper);
	
	m_pPMAICalc = new PMAITrainingEngine(m_pServiceHelper);

	m_pPathLossOfIndoorCalc = new PathLossofIndoorEngine(m_pServiceHelper);

	m_pDtMeasEngine = new MeasuredDataEngine(m_pServiceHelper);

	m_pSceneClassfier = new SceneClassfier(m_pServiceHelper);
	
	return true;
}

void PropagationSubSys::CalibrateCellParams() const
{
	//校正工参
	CellLoader* pCellLoader = ((CellLoader*)(m_pServiceHelper->GetService(BS_ENGINEERING_PARAMS_SERVICE)));
	if (!pCellLoader)
		return;

	std::list<std::shared_ptr<Transceiver>>&  allTrans = pCellLoader->getTranscvList();

	for (std::shared_ptr<Transceiver> tran : allTrans) {
		for (AntInstParam& antInst : tran->antInstParams) {
			antInst.dlEstimatedExtraLoss = 0;
			std::shared_ptr<PropagationModel> propModel = nullptr;
			if (m_cellModel) {
				propModel = m_cellModel->GetCellModel(tran->cells.front()->cellID.c_str(), false);
				if (propModel && (propModel->AntInstHashValue == antInst.GetHash()))
					antInst.dlEstimatedExtraLoss = propModel->DlExtraLoss;
			}

			if ( (0 == antInst.dlEstimatedExtraLoss) && m_pPropModelAnalyzer )
				antInst.dlEstimatedExtraLoss = -m_pPropModelAnalyzer->GetDlLossDevation(tran->cells.front(), antInst);

			if ((0 == antInst.dlEstimatedExtraLoss) && m_cellModel) {
				propModel = m_cellModel->GetDefaultModel();
				if (propModel)
					antInst.dlEstimatedExtraLoss = propModel->DlExtraLoss;
			}
		}
	}
}

bool PropagationSubSys::Init(IServiceHelper* pAppHelper)
{
	m_pServiceHelper = pAppHelper;
	ClearEngines();

	LiShuttlePathLossFactory::instance();
	LoadModels();

	return true;
}

void PropagationSubSys::Exit()
{
	LiShuttlePathLossFactory::destroy();
	PropagationModelFactory::destroy();

	if (m_cellModel)
		m_cellModel.reset();

	if (m_sceneModel)
		m_sceneModel.reset();

	ClearEngines();

	if (m_pServiceHelper)
		m_pServiceHelper->RemoveService(PROPAGATION_MODEL_ANALYZER);
	delete m_pPropModelAnalyzer;
	m_pPropModelAnalyzer = nullptr;

	m_pServiceHelper = NULL;
}

void PropagationSubSys::LoadModels()
{
	//加载模型分析器
	char cellModelFolder[256];
	PropagationModelFactory::GetCellModelFolder(cellModelFolder, 256);
	m_pPropModelAnalyzer = new PropModelAnalyzer(cellModelFolder);
	if (m_pServiceHelper)
		m_pServiceHelper->AddService(PROPAGATION_MODEL_ANALYZER, (void*)m_pPropModelAnalyzer);

	PropagationModelFactory* propFactory = PropagationModelFactory::instance();
	if (!propFactory)
		return;

	propFactory->LoadModels();
	
	//创建场景模型，加载场景模型文件
	char szModelFile[512];
	PropagationModelFactory::GetSceneModelFile(szModelFile, 512);
	if (m_sceneModel)
		m_sceneModel.reset();
	m_sceneModel = std::make_shared<LiShuttleSceneModel>(szModelFile);
	propFactory->LoadSceneModels(m_sceneModel);

	//加载自定义模型映射
	PropagationModelFactory::GetCustomSceneMapFile(szModelFile, 512);
	m_sceneModel->LoadCustomMap(szModelFile);
}
