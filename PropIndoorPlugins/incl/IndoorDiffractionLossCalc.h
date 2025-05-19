#pragma once

#include "PropIndoor.h"
#include "IMaterialFinder.h"
#include <GeoExports.h>

class IndoorDiffractionLossCalc
{
public:
	IndoorDiffractionLossCalc() = default;
	virtual ~IndoorDiffractionLossCalc() = default;

	static Vector3D GetEField(const Vector3D& E_in, const Geo3DPoint& lastPt, const Geo3DPoint& diffPt, \
		const Geo3DPoint& nextPt, const Geo3DPoint normalLines[2], double f_ghz, const MaterialInformation* pMatInfo);
};
