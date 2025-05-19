#pragma once

#include "PropInterface.h"
#include "SectionSpotData.h"
#include "PathLossCalcParams.h"
#include <memory>

class PROPINF_EXPORT_CLASS RadialSectionSpotData : public SectionSpotData, public IDataSerialize
{
public:
	RadialSectionSpotData(const std::shared_ptr<SectionPathLossCalcParam> sectionParam, bool allocatedMem = true);
	virtual ~RadialSectionSpotData();
	
	virtual SamplingSpot* GetSamplingSpotByXY(double x, double y) const;
	virtual bool GetIndex(double x, double y, int& row, int& col) const;
	const char* getCellID() const;

	bool GetGridCord(int row, int col, double& x, double& y) const;

	virtual void Serialize(StreamFile& streamFile) const;
	virtual void Deserialize(StreamFile& streamFile);
	virtual size_t GetHash() const;

protected:
	void TransXYToPolar(double x, double y, double& radius, double& rad) const;
	void TransPolarToXY(double radius, double rad, double& x, double& y) const;

	//将极坐标转换为格点索引，若失败则返回-1
	int TransPolarToIndex(double radius, double rad, int& spotNo) const;

protected:
	double CalcRadius;
	double Resolution;
	double AngleResolution;
	double StartAngle;
	double EndAngle;
	std::string m_cellID;
private:
	size_t m_hashCode;
	
	std::shared_ptr<SectorPathLossCalcParam> m_sectorParam;
};
