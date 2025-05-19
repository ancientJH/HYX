#pragma once

#include "PropIndoor.h"
#include "GeoWallVertex.h"
#include "VSNode.h"

class IDiffTreeManager
{
public:
	virtual VSNode* GetVertDiffTreeRoot(int floorNo, double height, const GeoWallVertex* vertex, \
		RayTracingCounter tracingCounter, RayTracingCounter maxTracingNum) = 0;
};