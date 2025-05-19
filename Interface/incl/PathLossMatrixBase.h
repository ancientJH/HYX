#pragma once

#include "PropInterface.h"
#include "NETypes.h"
#include "AntennaTypes.h"
#include "GridDataService.h"
#include <GeoExports.h>

class PROPINF_EXPORT_CLASS PathLossMatrixBase : public GridDataService {
public:
	PathLossMatrixBase(const GeoRasterRect& dataBound, const char* szDataName, size_t unitSize);
	PathLossMatrixBase(const char* szDataName, size_t unitSize);

	virtual ~PathLossMatrixBase();

	virtual float GetAt(int index) const = 0;
	virtual void  SetAt(int index, float value) const = 0;
};

