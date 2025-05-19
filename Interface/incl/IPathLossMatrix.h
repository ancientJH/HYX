#pragma once

#include "NETypes.h"
#include "AntennaTypes.h"
#include <GeoExports.h>

class IPathLossMatrix {
public:
	virtual ~IPathLossMatrix() = default;

	virtual const GeoRasterRect& GetBound() const = 0;

	virtual float GetAt(int i) const = 0;
};

