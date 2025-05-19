#pragma once

#include "PropAIPlugins.h"
#include "IDataSerialize.h"
#include "NETypes.h"
#include "PathLossCalcParams.h"
#include "CalibrationParams.h"
#include "SamplingGridFeature.h"
#include "SectionSpotData.h"
#include "LiShuttleAICalculator.h"

class PROPAIPLUGINS_EXPORT_CLASS CellAIGridFeature : public IDataSerialize
{
public:
	CellAIGridFeature(const std::shared_ptr<SectorPathLossCalcParam> sectorParam, LiShuttleAICalculator* pAICalculator);
	virtual ~CellAIGridFeature();

public:
	void SetPathLoss(MeasuredData* mrData);
	
	SamplingGridFeature* FindGridFeature(double x, double y);

	const char* GetCellId() const;

	SectionSpotData* GetSpotFeature() const;
	void SetSpotFeature(SectionSpotData* pSpotData) { m_pSpotData = pSpotData; };

	std::shared_ptr<SectorPathLossCalcParam> GetSectorParam() const;
	
	int GetCount()  const { return m_sumData; };
	int GetRange60() const { return m_range60; };
	int GetRange90()  const { return m_range90; };
	void SetMDTsum() { m_MDTsum++; };
	int GetMDTsum()  const { return m_MDTsum; }
	double GetMaxDistance() const { return m_maxDistOfSignal; };
	void CountHeightAndClutter(short clutter, short height);
	void CalculateStatisticsFeature();

	void StatFeatures();

	virtual void Serialize(StreamFile& streamFile) const;
	virtual void Deserialize(StreamFile& streamFile);
	virtual size_t GetHash() const;

	void RangeCount();//统计小区内有效栅格点数

	void GetCellFeature(char* feature, size_t bufSize) const;

	std::vector<int> m_heights;
	std::vector<int> m_clutters;
private:
	void Init();

private:
	size_t m_hashCode;
	std::shared_ptr<SectorPathLossCalcParam> m_sectorParam;
	
	size_t m_lineNumber;
	size_t m_spotNumberOnLine;
	double m_maxDistOfSignal;
	SectionSpotData* m_pSpotData;
	SignalOnGrid* m_pSignalGrids;
	LiShuttleAICalculator* m_pAICalculator;
	std::string m_outputStatInfo;

	int m_MDTsum = 0;
	int m_sumData = 0;
	int m_range60 = 0;
	int m_range90 = 0;
	
};