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
	float Dist2DToRoot;	/*���վ��2D����*/
	float BsHeights;	/*��վ��Ч�߶�*/
	float UeHeights;	/*�û���Ч�߶�*/
	float RealTxH;		/*��վ���Ը߶�*/
	float RealRxH;		/*�û����Ը߶�*/
	float Fc;			/*����Ƶ��MHz*/

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

	unsigned char bCalced = 0;	/*�Ƿ���й�·����㣬0Ϊû��*/
	unsigned char bMainRay = 0;	/*�Ƿ�������0Ϊ��*/

	float Azimuth;			/*��λ��*/

	float Elevation;		/*�����*/

	short ClutterCode;		/*��������*/

	short BuildingHeight;	/*������߶�*/

	float PowerWithoutAt;	/****��ACP�����ṩ��power ����ӵ�ע��****/

	TRParam TRInfo;			/*�շ���������*/

	std::list<Geo3DPoint> MultiHops;	/*�����ڵ�*/

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

