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

	//����դ�񼶱�·����̵�һ���� ͨ������դ�����ĵ��xyֵ ��������õ�·��ļ��㷽��
	virtual losstype GetPathLossValueByXY(double x, double y, float freq) const;

	//�������������·��ֵ
	virtual void CorrectPathloss(std::list<PathlossLabel>*);

	losstype GetPathLossValue(int rayNo, int sampleNo, float& linearLoss, SpotType& outdoor) const;

	bool SetPathLossValue(int rayIndex, int sampleIndex, const PathLossValue& value);

	void AssignRegionData(RadialRegionData* pRegionData) const;

	static losstype GetInterpolationValue(losstype valueA, losstype valueB, float weightA);
	static losstype GetQuadInterpolationValue(losstype leftBelow, losstype rightBelow, \
		losstype leftUp, losstype rightUp, float weightLeft, float weightBelow);

	//ACP���  ͨ��դ�����ĵ������ȡ��Ӧ�������б�
	//std::shared_ptr<RayInfoList> GetRayListByXY(float x, float y, const std::shared_ptr<RadialRaySpectrum> raySpectrum);

	virtual void Serialize(StreamFile& streamFile) const;

protected:
	/*void TransXYToPolar(float x, float y, float& radius, float& angle) const;
	void TransPolarToXY(float radius, float angle, float& x, float& y) const;
	void TransIndexToXY(int rayIndex, int radiusIndex, float& x, float& y) const;*/

	//�ڼ���դ�����ĵ�·������� �ڼ������� ͨ�����ĵ㼫����λ������ȡ�ٽ��Ѿ������·��Ĳ����� �Ӷ��ܹ�����·������
	//bool GetAroundSampleNo(AroundSampleNo& aroundSample, float radius, float radAngle) const;

	short GetBuildingHeight(int rayIndex, int radiusIndex, double& x, double& y) const;

	//�ڼ���դ�����ĵ�·������� ��դ�����ĵ�ת���ɼ�������ȡ·��ľ�����㷽��
	losstype GetPathLossValueByPolar(double radius, double radAngle, GeoBuildingPtr building, float freq) const;

	//ͨ���ٽ�������ȡդ�����ĵ�·��ֵ�ľ��巽�� �Ǵ����߲�����·��ת����դ��λ·�����ײ㲽��
	losstype GetPathLossValueByAroundSamples(std::shared_ptr<GeoBuildingPolygon> building, float freq, const AroundSampleNo&, float& deep) const;

	//ACP���  ������������ȡ����
	std::shared_ptr<RayInfoList> GetRayListByPolar(double x, double y, double radius, double angle, std::shared_ptr<GeoBuildingPolygon> building, const std::shared_ptr<RadialRaySpectrum> raySpectrum);

	//ACP���  ��Χ���ȡ����
	std::shared_ptr<RayInfoList> GetRayListByAroundSamples(double x, double y, std::shared_ptr<GeoBuildingPolygon> building, const AroundSampleNo& aroundSamples, const std::shared_ptr<RadialRaySpectrum> raySpectrum);

private:
	float AngleResolution;
	short SampleNumberOnOneLine;
	short ScanningLineNumber;
	double m_gaussianBandWidth;

	RadialRegionData m_regionAssist;
};
