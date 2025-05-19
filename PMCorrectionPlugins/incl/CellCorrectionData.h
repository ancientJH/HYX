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
	//������������ߵķ�λ��
	static void CalculateAngle(const AntInstParam& antenna, double& azimuth, double& elevation, double msAzimuth, double msElevation);

	void MergeDt(PMCorrectionGrid_Map* pGridMap);

	//���ݹ��κ���������Ϣ����ʼ��С���ĸ�����
	void Init(std::shared_ptr<Transceiver> transceiver, RadialRaySpectrum* pRaySpec);

	//װ��ʵ������,������Ҫ�޸�
	//void MountMeasuredData(PMCorrectionCalcParam calParams, std::vector<dtData> driveTestData);
	bool MountMeasuredData(const PMCorrectionCalcParam& calParams, const MeasuredData& measData);

	//װ��С���������ס�
	//����������֮ǰ���ȼ���·�����ݣ�û��·�����ݵĸ��Ҳû��Ҫ����������
	void MountRaySpectrum(RadialRaySpectrum* pRaySpec);

	//�ҵ�����ָ��λ������������׸��, ͬʱ���ؾ��룻���Ҳ������򷵻�NULLָ��
	//distLimit�������ƾ��룬�����þ���ĸ�㲻����
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
	inline double GetDistRange() const { return m_rangeofDist; }	//���뼫��
	inline double GetDistGini() const { return m_giniofDist; }		//�������ϵ��
	inline double GetRsrpDistCoef() const { return m_corrCoef; };	//rsrp���������ϵ��
	//inline double GetPlDeviation() const { return m_plDeviation; };
	inline CALRESULT_TYPE GetFilterRet() const { return m_filterRet; };
	inline bool IsValidCorrCell() const { return m_isValid; };
	inline void SetInvalid(bool _invalid = true) { m_isValid = !_invalid; };
	inline bool GetLosValid() const { return m_isLosValid; };
	inline bool GetNLosValid() const { return m_isNLosValid; };
	inline bool IsModelCalibrated() const{ return m_isModelUpdate; };

	const char* GetAntModel() const;
	const char* GetCity() const;

	//�˳���Чդ��
	void Filter();

	void SetFilterRet(CALRESULT_TYPE ret);
	void SetFilterValue(double range, double gini, double distRsrp, CALRESULT_TYPE ret);

	void SetLosValid(bool corr);
	void SetNLosValid(bool corr);

	void ClearAllGrids();
	void ClearNlosGrids();

	//����ģ�� ��������դ����·��ֵ
	void UpdateAllGridPathLoss(std::shared_ptr<LiShuttlePropagationModel> model);
	//void UpdateAllGridPathLoss(std::shared_ptr<LiShuttlePropagationModel> model, double plDeviation);
	//void SetPathlossDeviation(double plDev);

	//����·������ê��
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

	//��������ת��Ϊ�����������ʧ���򷵻�-1
	int TransPolarToIndex(double radius, double rad) const;
	//���������ת��Ϊ������
	bool TransIndexToPolar(int index, double& radius, double& rad) const;

	//��������
	bool AreReservedRays(std::shared_ptr<RayInfoList> rays);

	//��ʼ����������������ת����
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
	bool m_isLosValid;					//LOS��������Ч��
	bool m_isNLosValid;					//NLOS��������Ч��
	bool m_isModelUpdate;				//����ģ��У�����Ƿ����
	double m_minDtValidDistance;		//��С��Ч·����룬��λ��m
	CALRESULT_TYPE m_filterRet;			//���ݹ��˷�����

	//double m_plDeviation;				//·��ƫ��

	/*dtָ��*/
	int	m_countofLosDt = 0;
	int m_countofNLosDt = 0;
	double m_rangeofDist = 0;	//���뼫��
	double m_giniofDist = 0;	//�������ϵ��
	double m_corrCoef = 0;		//rsrp���������ϵ��
	
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