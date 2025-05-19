#pragma once

#include "PropInterface.h"
#include "GeoXYRect.h"
#include "GeoBuildingPolygon.h"
#include "IGeoBlockAssist.h"
#include <memory>

class PROPINF_EXPORT_CLASS GisPreLoadData 
{
public:
	GisPreLoadData();
	GisPreLoadData(const GeoRasterRect& _bound);
	virtual ~GisPreLoadData();

	virtual void InitDataBuffer(const GeoRasterRect& _bound);
	void DestroyDataBuffer();

	short* Buffer();
	const short* Buffer() const;

	const GeoRasterRect& Bound() const;
	void SetBound(const GeoRasterRect& bound);

	int GetIndex(double x, double y) const;
	virtual short GetValue(double x, double y) const;

	static int GetUseCount();

private:
	GeoRasterRect m_bound;
	short* m_buffer;
};
