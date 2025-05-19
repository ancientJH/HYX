#pragma once

#include "PropInterface.h"
#include "GisPreLoadData.h"
#include "GeoBuildingPolygon.h"
#include "GeoXYRect.h"
#include "IGeoBlockAssist.h"
#include "IGeoWallBlock.h"
#include "IClutterParamsManager.h"
#include "GeoInfo.h"
#include <list>
#include <vector>

class PROPINF_EXPORT_CLASS BuildingPolyPreLoadData : public GisPreLoadData
{
public:
	BuildingPolyPreLoadData(const GeoRasterRect& bound);
	virtual ~BuildingPolyPreLoadData();

	virtual short GetValue(double x, double y) const;

	virtual GeoBuildingPtr GetBuildingHeight(double x, double y, short& height) const;
	
	virtual void GetBuildingHeightsByRay(short* pBuildHeight, \
			std::vector<GeoBuildingPtr>& geoBuildings, \
			double centerX, double centerY, double deltaX, double deltaY, int count) const;

	/*void LoadData(std::unique_ptr<IGeoBlockAssist> geoBlockAssist,\
					const IClutterParamsManager* pClutterParams);*/
	void LoadData(IGeoInfo* pGeoInfo, const IClutterParamsManager* pClutterParams, size_t threadCount = 0);

	void ExportAllPolygon(const char* fileName) const;

	virtual bool Indoor(double x, double y) const;
private:
	int GetIndex(int row, int col, int no) const;
	
	int	 FindBlockNo(double x, double y, std::vector<GeoXYRect>& blockBounds) const;

private:
	/*std::vector<GeoBuildingPolygonList> m_allBlockPolygons;
	std::vector<GeoXYRect> m_blockBounds;*/

	std::vector<GeoBuildingPolygonList> m_polygonLocation;
};
