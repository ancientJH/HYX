#pragma once

#include "PMCorrPlugins.h"
#include "PMCorrectionGrid.h"
#include "IDataSerialize.h"
#include "NETypes.h"
#include "RadialRaySpectrum.h"
#include "CalibrationParams.h"
#include "StraightPathLoss.h"
#include "VertDiffractionRay.h"
#include "PMCorrectionGrid.h"
#include "Matrix.h"
#include "AntCalcGain.h"
#include "IGeoInfo.h"
#include "MeasuredDataAnchors.h"

class PMCORR_EXPORT_CLASS CellCorrectionData : public IDataSerialize
{
public:
	CellCorrectionData(const PMCorrectionCalcParam&, AntCalcGain* pAntCalcGain, IGeoInfo* pGeoInfo);
	virtual ~CellCorrectionData();

public:
	//计算相对于天线的方位角
	static void CalculateAngle(const AntInstParam& antenna, double& azimuth, double& elevation, double msAzimuth, double msElevation);

	void MergeDt(PMCorrectionGrid_Map* pGridMap);

	//根据工参和射线谱信息，初始化小区的格点参数
	void Init(std::shared_ptr<Transceiver> transceiver, RadialRaySpectrum* pRaySpec);

	//装载实测数据,根据需要修改
	//void MountMeasuredData(PMCorrectionCalcParam calParams, std::vector<dtData> driveTestData);
	bool MountMeasuredData(const PMCorrectionCalcParam& calParams, const MeasuredData& measData);

	//装载小区的射线谱。
	//加载射线谱之前，先加载路测数据，没有路测数据的格点也没必要保存射线谱
	void MountRaySpectrum(RadialRaySpectrum* pRaySpec);

	//找到距离指定位置最近的射线谱格点, 同时返回距离；若找不到，则返回NULL指针
	//distLimit给出限制距离，超出该距离的格点不返回
	PMCorrectionGrid* FindGrid(double x, double y, double distLimit, double& distance);

	inline std::shared_ptr<Transceiver> GetTrasceiver()  const { return m_transceiver; }
	inline PMCorrectionGrid_Map* GetGrid_Map()  const { return m_pGridMap; };
	inline std::shared_ptr<LiShuttlePropagationModel> GetCellModel() const { return m_cellModel; };
	inline const PathlossSceneFeature& GetPlSceneFeature() const { return m_plSceneFeature; };
	inline AntennaParams* GetAntParam() const { return m_pAntParams; };
	inline double GetMinDtValidDistance() const { return m_minDtValidDistance; };
	inline size_t GetGridCount() const { return m_pGridMap ? m_pGridMap->size() : 0; };
	inline int GetLosGridCount() const { return m_countofLosDt; };
	inline int GetNLosGridCount() const { return m_countofNLosDt; };
	inline double GetDistRange() const { return m_rangeofDist; }	//距离极差
	inline double GetDistGini() const { return m_giniofDist; }		//距离基尼系数
	inline double GetRsrpDistCoef() const { return m_corrCoef; };	//rsrp与距离的相关系数
	//inline double GetPlDeviation() const { return m_plDeviation; };
	inline CALRESULT_TYPE GetFilterRet() const { return m_filterRet; };
	inline bool IsValidCorrCell() const { return m_isValid; };
	inline void SetInvalid(bool _invalid = true) { m_isValid = !_invalid; };
	inline bool GetLosValid() const { return m_isLosValid; };
	inline bool GetNLosValid() const { return m_isNLosValid; };
	inline bool IsModelCalibrated() const{ return m_isModelUpdate; };

	const char* GetAntModel() const;
	const char* GetCity() const;

	//滤除无效栅格
	void Filter();

	void SetFilterRet(CALRESULT_TYPE ret);
	void SetFilterValue(double range, double gini, double distRsrp, CALRESULT_TYPE ret);

	void SetLosValid(bool corr);
	void SetNLosValid(bool corr);

	void ClearAllGrids();
	void ClearNlosGrids();

	//输入模型 更新所有栅格点的路损值
	void UpdateAllGridPathLoss(std::shared_ptr<LiShuttlePropagationModel> model);
	//void UpdateAllGridPathLoss(std::shared_ptr<LiShuttlePropagationModel> model, double plDeviation);
	//void SetPathlossDeviation(double plDev);

	//生成路测数据锚点
	void CreateMeasAnchors(MeasuredDataAnchors* pMeasAnchors) const;

	void CalcRealPathLoss();
	void SetCellModel(std::shared_ptr<LiShuttlePropagationModel> model, bool update = false);
	
	virtual void Serialize(StreamFile& streamFile) const;
	virtual void Deserialize(StreamFile& streamFile);
	virtual size_t GetHash() const;

	void CalcPartialMatrix(Matrix& g, std::shared_ptr<LiShuttlePropagationModel> model, bool antGainCalced);
	double CalcStepPartial(Matrix& X, Matrix& d, Matrix& S, bool antGainCalced, double h);

	static std::string GetFeatureTitle();
	std::string ExtractFeatures() const;

	bool TransIndexToPolarIndex(int index, int& radiusIndex, int& radIndex) const;

	bool TransIndexToXY(double index, double& x, double& y) const;

protected:
	void ClearGridMap();

	void TransXYToPolar(double x, double y, double& radius, double& rad) const;
	void TransPolarToXY(double radius, double rad, double& x, double& y) const;

	//将极坐标转换为格点索引，若失败则返回-1
	int TransPolarToIndex(double radius, double rad) const;
	//将格点索引转换为极坐标
	bool TransIndexToPolar(int index, double& radius, double& rad) const;

	//过滤射线
	bool AreReservedRays(std::shared_ptr<RayInfoList> rays);

	//初始化射线形数组坐标转换器
	void InitRegionAssist();

private:
	double CalcRadius = 0;
	double Resolution = 0;
	double AngleResolution = 0;
	double StartAngle = 0;
	double EndAngle = 0;
	int SampleNumberOnOneLine = 0;
	int ScanningLineNumber = 0;
	bool m_isValid;
	bool m_isLosValid;					//LOS场景的有效性
	bool m_isNLosValid;					//NLOS场景的有效性
	bool m_isModelUpdate;				//传播模型校正后，是否更新
	double m_minDtValidDistance;		//最小有效路测距离，单位：m
	CALRESULT_TYPE m_filterRet;			//数据过滤返回码

	//double m_plDeviation;				//路损偏差

	/*dt指标*/
	int	m_countofLosDt = 0;
	int m_countofNLosDt = 0;
	double m_rangeofDist = 0;	//距离极差
	double m_giniofDist = 0;	//距离基尼系数
	double m_corrCoef = 0;		//rsrp与距离的相关系数
	
	RadialRegionData m_regionAssist;
	PMCorrectionCalcParam m_corrCalcParam;

	std::shared_ptr<LiShuttlePropagationModel> m_cellModel;
		
	AntCalcGain* m_pAntCalcGain;
	AntennaParams* m_pAntParams;
	IGeoInfo* m_pGeoInfo;

	std::shared_ptr<Transceiver> m_transceiver;
	
	PathlossSceneFeature m_plSceneFeature;
	PMCorrectionGrid_Map* m_pGridMap;
};