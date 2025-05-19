#pragma once

#include "VertDiffractionRay.h"

class BuildingPeak {
public:
	BuildingPeak(int souceIndex, short sourceHeight, double resolution);
	
	void Init();
	void SetTerrain(int index, bool los, double distance, double currSlope, \
				short currAltitude, short currBuildingHeight);

	Obstacle GetObstacleInfo() const;

public:
	double Distance1;
	double Height1;
	int		Index1;
	double Distance2;
	double Height2;
	int		Index2;

private:
	double m_minSlope1;
	double m_minSlope2;
	double m_resolution;
	int m_sourceIndex;
	short m_sourceHeight;
};