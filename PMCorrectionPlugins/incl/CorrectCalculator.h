#pragma once
#include "PMCorrPlugins.h"
#include "PMCorrectionData.h"
#include "PropagationModel.h"
#include "LiShuttlePropagationModel.h"
#include "PropagationModelFactory.h"
#include "PathLossCalcParams.h"
#include "ParamCorrectFactory.h"
#include "CellLoader.h"
#include "LsThreadPool.h"
#include <vector>
#include <list>
#include <string>
#include <numeric>
#include <cmath>
#include "Matrix.h"

class PMCORR_EXPORT_CLASS CorrectCalculator
{
public:
	CorrectCalculator();
	~CorrectCalculator();

	inline const LiShuttleCellModelMap& GetCorModels() const { return m_CorModels; };

	inline const LiShuttleSceneModelMap* GetSceneModels() const { return &m_sceneModels; };
	
	inline std::shared_ptr<LiShuttlePropagationModel> GetGeneralModel() const { return m_GeneralModel; };
	
	void Init(const PMCorrectionCalcParam& pmCorrectionCalcParam, AntCalcGain* antCalc, CellLoader* pCellLoader);
	
	void Run(PMCorrectionData* pmCorrectionData);

	void PrintProcessedInfo() const;

	//���㷽��;�ֵ
	static void CalculateMeanVariance(const std::list<CellCorrectionData*>& cellDatas, double errorThreshold, \
		std::list<double>& means, std::list<double>& stdevs, \
		std::list<double>& r2s, std::list<double>& predAccRatios);

	inline bool IsCalExcu() const { return m_isCalExcu; };
	inline bool IsCalScene() const { return m_isCalScene; };

private:
	using CalFuncInfo = struct tagCalFuncInfo {
		CorrectionType type;
		CALRESULT_TYPE ret;

		tagCalFuncInfo(CorrectionType _t, CALRESULT_TYPE _ret) : type(_t), ret(_ret) {};
	};

	static std::string PrintIndicators(const std::list<double>& means, const std::list<double>& stdevs);
	static void GetIndicatorText(std::string& indicators, const std::list<double>& means, const std::list<double>& stdevs);

	//ͨ��ģ��У��
	void CaliGeneralModel(const std::map<int, std::list<CellCorrectionData*>>& siteCellMap);

	//һվһģУ��
	void CaliAllCellModels(const std::map<int, std::list<CellCorrectionData*>>& siteCellMap);
	void CaliOneSite(int siteId, const std::list<CellCorrectionData*>& pCellList);
	
	//����ģ��У��
	void CaliSceneModel(const std::map<int, std::list<CellCorrectionData*>>& siteCellMap);

	//У��
	bool Calibrate(const std::list<CellCorrectionData*>& cellDatas, \
		std::shared_ptr<LiShuttlePropagationModel> model, bool isCaliScene = false, bool forceCal = false, \
		std::list<std::string>* logs = nullptr) const;

	bool CorrectModel(CALRESULT_TYPE& ret, const std::list<CellCorrectionData*>& cellDatas, std::shared_ptr<LiShuttlePropagationModel> model, \
		std::list<double>& means, std::list<double>& stdevs, std::list<double>& r2s, std::list<double>& predAccRatios, \
		bool isCaliScene = false, bool forceCal = false) const;

	CALRESULT_TYPE GetCalibrationType(double& accuracy_rate, const std::list<CalFuncInfo>& calFuncInfos, \
		const std::list<double>& means, const std::list<double>& stdevs, \
		const std::list<double>& r2s, const std::list<double>& predAccRatios, bool print = false) const;

	//�Ƿ�ֹͣ����
	bool IsStopCycle(double lastMean, double nowMean, double lastStd, double nowStd);
	
	//ţ�ٷ�
	void NewtonRun(PMCorrectionData* pmCorrectionData);
	void CalPartialMatrix(Matrix& g, PMCorrectionData* pmCorrectionData, std::shared_ptr<LiShuttlePropagationModel> model);
	double CalStep(Matrix& X, Matrix& d, PMCorrectionData* pmCorrectionData, std::shared_ptr<LiShuttlePropagationModel> model);
	void UpdateModel(Matrix& X, std::shared_ptr<LiShuttlePropagationModel> model);
	bool IsStop(Matrix& g);
	double CalculateGradientDescent(Matrix& X, Matrix& d, PMCorrectionData* pmCorrectionData, double h);

private:
	bool m_isCalExcu = false;			//�Ƿ����һվһģ
	bool m_isCalScene = false;			//�Ƿ���㳡��ģ��
	bool m_rebuildScene = false;		//�Ƿ��ؽ�����ģ�ͣ������̳���ǰ��У�������
	bool m_forceCalibration = false;	//ǿ��У��
	double m_maxMeanError;				//����ֵ������ֹͣ������
	double m_maxStdError;				//����׼�����ֹͣ������
	double m_errorThreshold;			//����դ���������ޣ�dB��
	double m_minAccRate;				//�ۺϿ��Ŷ����ޣ�0~1��
	int	m_maxEpoch;						//����������
	int m_threadCount;					//�����߳���

	ModelRestrictions m_modelRestrictions;	//ģ��У���߽�����
	AntCalcGain* m_AntCalc;
	CellLoader* m_pCellLoader;
	std::string m_dtFolder;
	bool m_caliLos;
	bool m_caliNLos;
	ParamCorrectFactory m_paramCorrectFactory;
	LiShuttleSceneModelMap m_sceneModels;						//����ģ��У�����
	LiShuttleCellModelMap m_CorModels;							//һվһģУ�����
	std::shared_ptr<LiShuttlePropagationModel> m_GeneralModel;	//ͳһģ��
	std::list<CalFuncInfo> m_Functions;							//��ҪУ���Ĳ���
	std::list<std::string> m_wrongSites;		//���ο��ܳ���Ļ�վ

	std::mutex m_wrongSiteMutex;
	std::mutex m_corModelMutex;
	std::mutex m_printMutex;
};

