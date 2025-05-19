#pragma once

#include "PropIndoor.h"
#include "VirtualSource.h"
#include "GeoWallBase.h"
#include "IGeoBuildingFloor.h"

//÷±…‰‘¥
class EmissionSource : public VirtualSource
{
public:
	virtual ~EmissionSource();

	static VirtualSource* CreateSource(double height, GeoXYPoint sourcePt, VirtualSource* preSource, const GeoWallVertex* vertex, const WallSegment* wallSegment, \
		IGeoBuildingFloor* pBuildingFloor, double minErr);

protected:
	EmissionSource(int floor, double height, GeoXYPoint origPoint);
	EmissionSource(int floor, double height, GeoXYPoint origPoint, GeoXYPoint startRayPt, GeoXYPoint endRayPt);
};