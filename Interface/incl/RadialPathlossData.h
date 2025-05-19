#pragma once

#include "PropInterface.h"
#include "PathLossCalcParams.h"
#include "SectionPathlossData.h"
#include "RadialRaySpectrum.h"
#include "RaySpectrum.h"
#include "RadialRegionData.h"
#include "UStreamFile.h"
#include <memory>

class PROPINF_EXPORT_CLASS RadialPathLossData : public SectionPathLossData
{
public:
	RadialPathLossData(const std::shared_ptr<SectionPathLossCalcParam> sectionParam);

	//int GetDataIndex(int rayIndex);

	//计算栅格级别路损过程的一部分 通过输入栅格中心点的xy值 具体输出该点路损的计算方法
	virtual losstype GetPathLossValueByXY(double x, double y, float freq) const;

	//用误差数据修正路损值
	virtual void CorrectPathloss(std::list<PathlossLabel>*);

	losstype GetPathLossValue(int rayNo, int sampleNo, float& linearLoss, SpotType& outdoor) const;

	bool SetPathLossValue(int rayIndex, int sampleIndex, const PathLossValue& value);

	void AssignRegionData(RadialRegionData* pRegionData) const;

	static losstype GetInterpolationValue(losstype valueA, losstype valueB, float weightA);
	static losstype GetQuadInterpolationValue(losstype leftBelow, losstype rightBelow, \
		losstype leftUp, losstype rightUp, float weightLeft, float weightBelow);

	//ACP添加  通过栅格中心点坐标获取对应的射线列表
	//std::shared_ptr<RayInfoList> GetRayListByXY(float x, float y, const std::shared_ptr<RadialRaySpectrum> raySpectrum);

	virtual void Serialize(StreamFile& streamFile) const;

protected:
	/*void TransXYToPolar(float x, float y, float& radius, float& angle) const;
	void TransPolarToXY(float radius, float angle, float& x, float& y) const;
	void TransIndexToXY(int rayIndex, int radiusIndex, float& x, float& y) const;*/

	//在计算栅格中心点路损过程中 在极坐标下 通过中心点极坐标位置来获取临近已经计算过路损的采样点 从而能够进行路损整合
	//bool GetAroundSampleNo(AroundSampleNo& aroundSample, float radius, float radAngle) const;

	short GetBuildingHeight(int rayIndex, int radiusIndex, double& x, double& y) const;

	//在计算栅格中心点路损过程中 将栅格中心点转换成极坐标后获取路损的具体计算方法
	losstype GetPathLossValueByPolar(double radius, double radAngle, GeoBuildingPtr building, float freq) const;

	//通过临近点来获取栅格中心点路损值的具体方法 是从射线采样点路损转换到栅格单位路损的最底层步骤
	losstype GetPathLossValueByAroundSamples(std::shared_ptr<GeoBuildingPolygon> building, float freq, const AroundSampleNo&, float& deep) const;

	//ACP添加  极坐标索引获取射线
	std::shared_ptr<RayInfoList> GetRayListByPolar(double x, double y, double radius, double angle, std::shared_ptr<GeoBuildingPolygon> building, const std::shared_ptr<RadialRaySpectrum> raySpectrum);

	//ACP添加  周围点获取射线
	std::shared_ptr<RayInfoList> GetRayListByAroundSamples(double x, double y, std::shared_ptr<GeoBuildingPolygon> building, const AroundSampleNo& aroundSamples, const std::shared_ptr<RadialRaySpectrum> raySpectrum);

private:
	float AngleResolution;
	short SampleNumberOnOneLine;
	short ScanningLineNumber;
	double m_gaussianBandWidth;

	RadialRegionData m_regionAssist;
};
