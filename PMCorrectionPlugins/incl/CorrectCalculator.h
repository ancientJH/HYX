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

	//计算方差和均值
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

	//通用模型校正
	void CaliGeneralModel(const std::map<int, std::list<CellCorrectionData*>>& siteCellMap);

	//一站一模校正
	void CaliAllCellModels(const std::map<int, std::list<CellCorrectionData*>>& siteCellMap);
	void CaliOneSite(int siteId, const std::list<CellCorrectionData*>& pCellList);
	
	//场景模型校正
	void CaliSceneModel(const std::map<int, std::list<CellCorrectionData*>>& siteCellMap);

	//校正
	bool Calibrate(const std::list<CellCorrectionData*>& cellDatas, \
		std::shared_ptr<LiShuttlePropagationModel> model, bool isCaliScene = false, bool forceCal = false, \
		std::list<std::string>* logs = nullptr) const;

	bool CorrectModel(CALRESULT_TYPE& ret, const std::list<CellCorrectionData*>& cellDatas, std::shared_ptr<LiShuttlePropagationModel> model, \
		std::list<double>& means, std::list<double>& stdevs, std::list<double>& r2s, std::list<double>& predAccRatios, \
		bool isCaliScene = false, bool forceCal = false) const;

	CALRESULT_TYPE GetCalibrationType(double& accuracy_rate, const std::list<CalFuncInfo>& calFuncInfos, \
		const std::list<double>& means, const std::list<double>& stdevs, \
		const std::list<double>& r2s, const std::list<double>& predAccRatios, bool print = false) const;

	//是否停止迭代
	bool IsStopCycle(double lastMean, double nowMean, double lastStd, double nowStd);
	
	//牛顿法
	void NewtonRun(PMCorrectionData* pmCorrectionData);
	void CalPartialMatrix(Matrix& g, PMCorrectionData* pmCorrectionData, std::shared_ptr<LiShuttlePropagationModel> model);
	double CalStep(Matrix& X, Matrix& d, PMCorrectionData* pmCorrectionData, std::shared_ptr<LiShuttlePropagationModel> model);
	void UpdateModel(Matrix& X, std::shared_ptr<LiShuttlePropagationModel> model);
	bool IsStop(Matrix& g);
	double CalculateGradientDescent(Matrix& X, Matrix& d, PMCorrectionData* pmCorrectionData, double h);

private:
	bool m_isCalExcu = false;			//是否计算一站一模
	bool m_isCalScene = false;			//是否计算场景模型
	bool m_rebuildScene = false;		//是否重建场景模型（即不继承以前的校正结果）
	bool m_forceCalibration = false;	//强制校正
	double m_maxMeanError;				//最大均值（迭代停止条件）
	double m_maxStdError;				//最大标准差（迭代停止条件）
	double m_errorThreshold;			//单个栅格的误差门限（dB）
	double m_minAccRate;				//综合可信度门限（0~1）
	int	m_maxEpoch;						//最大迭代次数
	int m_threadCount;					//并发线程数

	ModelRestrictions m_modelRestrictions;	//模型校正边界条件
	AntCalcGain* m_AntCalc;
	CellLoader* m_pCellLoader;
	std::string m_dtFolder;
	bool m_caliLos;
	bool m_caliNLos;
	ParamCorrectFactory m_paramCorrectFactory;
	LiShuttleSceneModelMap m_sceneModels;						//场景模型校正结果
	LiShuttleCellModelMap m_CorModels;							//一站一模校正结果
	std::shared_ptr<LiShuttlePropagationModel> m_GeneralModel;	//统一模型
	std::list<CalFuncInfo> m_Functions;							//需要校正的参数
	std::list<std::string> m_wrongSites;		//工参可能出错的基站

	std::mutex m_wrongSiteMutex;
	std::mutex m_corModelMutex;
	std::mutex m_printMutex;
};

