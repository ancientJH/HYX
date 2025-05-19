#pragma once

#include "RayInfo.h"
#include "VertDiffractionRay.h"
#include "StraightRay.h"
#include "HorzRay.h"
#include <memory>

typedef struct tagPenetrationRay : public RayInfo
{
	float DeepDistance;
	double LinearLoss = 0;				/*室内线性损耗（dB）*/
	double OutdoorPower = 0;			/*归一化室外功率（线性值）*/
	double PenetrationPower = 0;		/*归一化穿透功率（线性值）*/
	double PowerByDistance = 0;			/*归一化距离扩散功率（线性值）*/
	std::shared_ptr<RayInfo> OutdoorRay;
	
	tagPenetrationRay(float dist2DToRoot, float azimuth, float elevation,\
		short _clutter, short _bHeight)
		:RayInfo(RayCategory::PENETRATION, dist2DToRoot, azimuth, elevation, _clutter, _bHeight), \
		 DeepDistance(0)
	{
	}

	tagPenetrationRay(float dist2DToRoot, float azimuth, float elevation, \
		short _clutter, short _bHeight,	std::shared_ptr<RayInfo> outdoorRay, float deep)\
		:RayInfo(RayCategory::PENETRATION, dist2DToRoot, azimuth, elevation, _clutter, _bHeight),\
		OutdoorRay(outdoorRay), DeepDistance(deep)
	{	
	}

	tagPenetrationRay(const tagPenetrationRay& ray) : RayInfo((const tagRayInfo&)ray), OutdoorRay(ray.OutdoorRay)
	{
		DeepDistance = ray.DeepDistance;
		LinearLoss = ray.LinearLoss;
		OutdoorPower = ray.OutdoorPower;
		PenetrationPower = ray.PenetrationPower;
		PowerByDistance = ray.PowerByDistance;
	};

	~tagPenetrationRay() {
		OutdoorRay.reset();
	}

	virtual std::shared_ptr<tagRayInfo> Clone() {
		return std::make_shared<tagPenetrationRay>(*this);
	};

	virtual bool Serialize(MemStream& memStream) {
		if (!RayInfo::Serialize(memStream))
			return false;

		memStream << DeepDistance << LinearLoss << OutdoorPower << PenetrationPower << PowerByDistance;
		
		/*if (!OutdoorRay) {
			RayCategory rayType = RayCategory::UNDEFINED;
			memStream << rayType;
		}
		else {
			OutdoorRay->Serialize(memStream);
		}*/
		
		return true;
	};

	virtual bool Deserialize(MemStream& memStream) {
		if (!RayInfo::Deserialize(memStream))
			return false;

		memStream >> DeepDistance >> LinearLoss >> OutdoorPower >> PenetrationPower >> PowerByDistance;
		
		/*OutdoorRay.reset();
		OutdoorRay = DeserializeRay(memStream);*/

		return true;
	};
}PenetrationRay;
