#pragma once

#include "PropInterface.h"
#include <math.h>

typedef struct tagAroundSampleNo {
	int left, right, below, up;
	float weightBelow, weightLeft;
}AroundSampleNo;

struct PROPINF_EXPORT_CLASS RadialRegionData {
	double cx;
	double cy;
	double startDistance;
	double endDistance;
	double resolution;
	double angleResolution;
	double startAngle;
	double endAngle;
	int scanningLineNumber;
	int sampleNumberOnOneLine;

	size_t id;
	void* pData;
	int unitSize;

	RadialRegionData(int _unitSize);

	void TransXYToPolar(double x, double y, double& radius, double& radAngle) const;

	void TransPolarToXY(double radius, double radAngle, double& x, double& y) const;

	void TransIndexToXY(int rayIndex, int radiusIndex, double& x, double& y) const;

	void TransIndexToPolar(int rayIndex, int radiusIndex, double& radius, double& rad) const;

	void TransPolarToIndex(double radius, double rad, int& rayIndex, int& radiusIndex) const;

	bool GetAroundSampleNo(AroundSampleNo& aroundSample, double radius, double radAngle) const;

	bool GetData(double radius, double radAngle, AroundSampleNo& aroundSampleNo, \
		void** ppLeftBelow, void** ppRightBelow, void** ppLeftUp, void** ppRightUp) const;
};