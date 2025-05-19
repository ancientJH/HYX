#pragma once

#include <list>
#include <memory>
#include <math.h>
#include "UPlanCommonExport.h"
#include "GeoExports.h"
#include "MemStream.h"
#include "PropInterface.h"

typedef enum tagRayCategory
{
	STRAIGHT = 0,
	
	HORZRAY,
	
	VERT_DIFFRACTION,
	
	PENETRATION,

	UnderStation,

	RAY_UNDEFINED,
}RayCategory;

typedef struct tagTRParam
{
	float Dist2DToRoot;	/*与基站的2D距离*/
	float BsHeights;	/*基站有效高度*/
	float UeHeights;	/*用户有效高度*/
	float RealTxH;		/*基站绝对高度*/
	float RealRxH;		/*用户绝对高度*/
	float Fc;			/*中心频率MHz*/

	tagTRParam() {
		upl_bzero(this, sizeof(tagTRParam));
	};

	tagTRParam(float dist) : tagTRParam() {
		Dist2DToRoot = dist;
	};

	tagTRParam(const tagTRParam& trInfo) {
		upl_memcpy(this, &trInfo, sizeof(tagTRParam));
	};

	tagTRParam& operator=(const tagTRParam& obj) {
		upl_memcpy(this, &obj, sizeof(tagTRParam));
		return *this;
	};

	float Distance3d() const {
		return sqrt((RealTxH - RealRxH) * (RealTxH - RealRxH) + Dist2DToRoot * Dist2DToRoot);
	};

	float DistanceBp() const {
		return (float)13.33 * BsHeights * UeHeights * Fc * 0.001;
	}
}TRParam;

struct tagStraightRay;
struct tagVertDiffractionRay;
struct tagPenetrationRay;
struct tagHorzRay;

typedef struct PROPINF_EXPORT_CLASS tagRayInfo
{
	RayCategory Category;

	RayCategory EquivalentCate;

	unsigned char bCalced = 0;	/*是否进行过路损计算，0为没有*/
	unsigned char bMainRay = 0;	/*是否主径，0为否*/

	float Azimuth;			/*方位角*/

	float Elevation;		/*下倾角*/

	short ClutterCode;		/*地物类型*/

	short BuildingHeight;	/*建筑物高度*/

	float PowerWithoutAt;	/****给ACP功能提供的power 新添加的注意****/

	TRParam TRInfo;			/*收发基本参数*/

	std::list<Geo3DPoint> MultiHops;	/*多跳节点*/

	static int getUseCount();

	tagRayInfo();

	tagRayInfo(const tagRayInfo& ray);

	tagRayInfo(RayCategory category, float dist2DToRoot, float azimuth, float elevation, \
		short _clutter, short _bHeight);

	virtual ~tagRayInfo();

	virtual std::shared_ptr<tagRayInfo> Clone();

	virtual bool Serialize(MemStream& memStream);

	virtual bool Deserialize(MemStream& memStream);

}RayInfo;

#define RayInfoStaticEndFlag size_t(&(((RayInfo*)0)->MultiHops))


typedef std::list< std::shared_ptr<RayInfo> > RayInfoList;

PROPINF_EXPORT_FUNC std::shared_ptr<RayInfo> DeserializeRay(MemStream& memStream);

