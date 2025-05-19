#pragma once

#include "PropIndoor.h"
#include "VirtualSource.h"
#include "GeoWallBase.h"
#include "IGeoBuildingFloor.h"
#include <GeoExports.h>

//ֱ��Դ
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

	//floorHeightΪ¥��߶�
	static VirtualSource* CreateSource(double floorHeight, GeoXYPoint sourcePt, VirtualSource* preSource, const GeoWallVertex* vertex, const WallSegment* wallSegment, \
		IGeoBuildingFloor* pBuildingFloor, double minErr);

protected:
	//heightΪ¥��߶�
	HorzPenetrationSource(int floor, double height, double heightToGround, VirtualSource* preSource);

private:
	double m_heightToGround;			//ˮƽ¥����ظ߶ȣ�m��
	VirtualSource* m_parentSource;
};