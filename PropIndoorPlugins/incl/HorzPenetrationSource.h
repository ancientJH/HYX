#pragma once

#include "PropIndoor.h"
#include "VirtualSource.h"
#include "GeoWallBase.h"
#include "IGeoBuildingFloor.h"
#include <GeoExports.h>

//直射源
class HorzPenetrationSource : public VirtualSource
{
public:
	virtual ~HorzPenetrationSource();

	virtual double GetMinH() const;
	virtual double GetMaxH() const;

	bool GetPenePoint(GeoXYPoint& penePt, GeoXYPoint recvPt) const;

	virtual GeoXYPoint OrigPoint() const;
	virtual SectorType GetSectorType() const;
	virtual const GeoRadialAreaEx* GetCovSector() const;

	//floorHeight为楼层高度
	static VirtualSource* CreateSource(double floorHeight, GeoXYPoint sourcePt, VirtualSource* preSource, const GeoWallVertex* vertex, const WallSegment* wallSegment, \
		IGeoBuildingFloor* pBuildingFloor, double minErr);

protected:
	//height为楼层高度
	HorzPenetrationSource(int floor, double height, double heightToGround, VirtualSource* preSource);

private:
	double m_heightToGround;			//水平楼板离地高度（m）
	VirtualSource* m_parentSource;
};