#pragma once

#include "GisInterface.h"
#include "PropInterface.h"
#include "GeoXYPointAssist.h"
#include "RayInfo.h"
#include "vector"
#include "MemStream.h"
#include "UPlanCommon.h"

#include <list>

typedef struct tagReflection
{
	float IncomingAngle;
	GeoXYPoint ReflPoint;
	short ClutterCode;
	short SceneCode;
	tagReflection() : IncomingAngle(0), ReflPoint(0,0), ClutterCode(0), SceneCode(-1){};

	tagReflection(float _refAngle, GeoXYPoint _refPoint,short _clutterCode,short _sceneCode) :\
		IncomingAngle(_refAngle), ReflPoint(_refPoint), ClutterCode(_clutterCode), SceneCode(_sceneCode){};

	tagReflection(const tagReflection& refl) {
		upl_memcpy(this, &refl, sizeof(tagReflection));
	}

	virtual bool Serialize(MemStream& memStream) {
		int count = memStream.Write(this, sizeof(tagReflection));
		if (count != sizeof(tagReflection)) {
			ULog_Write(ULOG_ERROR, "The size of the reflection data is %u bytes, \
				and %d bytes have been write", sizeof(*this), count);
			return false;
		}

		return true;
	};

	virtual bool Deserialize(MemStream& memStream) {
		int count = memStream.Read(this, sizeof(tagReflection));
		if (count != sizeof(tagReflection)) {
			ULog_Write(ULOG_ERROR, "The size of the reflection data is %u bytes, \
				and %d bytes have been read", sizeof(*this), count);
			return false;
		}

		return true;
	}
}Reflection;

typedef struct tagDiffraction
{
	GeoXYPoint StartPoint;
	GeoXYPoint EndPoint;
	GeoXYPointList DiffPoints;

	tagDiffraction() {};

	tagDiffraction(GeoXYPoint _startPoint, GeoXYPoint _endPoint, GeoXYPointList _diffPoints) :\
		StartPoint(_startPoint),EndPoint(_endPoint), DiffPoints(_diffPoints){};	

	tagDiffraction(const tagDiffraction& diffRay) {
		upl_memcpy(this, &diffRay, upl_getoffset(tagDiffraction, DiffPoints));
		this->DiffPoints.assign(diffRay.DiffPoints.begin(), diffRay.DiffPoints.end());
	}

	virtual bool Serialize(MemStream& memStream) {
		memStream << StartPoint << EndPoint << DiffPoints.size();

		for (auto pt : DiffPoints) {
			memStream << pt;
		}

		return true;
	};

	virtual bool Deserialize(MemStream& memStream) {
		size_t ptNumber = 0;
		memStream >> StartPoint >> EndPoint >> ptNumber;

		DiffPoints.clear();
		GeoXYPoint pt;
		for (size_t i = 0; i < ptNumber; i++) {
			memStream >> pt;
			DiffPoints.push_back(pt);
		}

		return true;
	};

}Diffraction;

typedef struct tagHorzRay : RayInfo
{
	float PolarAngle;
	double DiffStrength;	/*衍射强度*/
	double RefStrength;		/*反射强度*/

	std::list<Reflection> Reflections;
	std::list<Diffraction> Diffractions;

	tagHorzRay(float dist2DToRoot, float azimuth, float elevation, \
		short _clutter, short _bHeight)\
		: RayInfo(RayCategory::HORZRAY, dist2DToRoot, azimuth, elevation, _clutter, _bHeight), \
		PolarAngle(0), DiffStrength(0), RefStrength(0) {};

	tagHorzRay(const tagHorzRay& horzRay) : RayInfo((const tagRayInfo&)horzRay)
	{
		PolarAngle = horzRay.PolarAngle;
		DiffStrength = horzRay.DiffStrength;
		RefStrength = horzRay.RefStrength;
		this->Reflections.assign(horzRay.Reflections.begin(), horzRay.Reflections.end());
		this->Diffractions.assign(horzRay.Diffractions.begin(), horzRay.Diffractions.end());
	}
	
	virtual ~tagHorzRay() {};

	virtual std::shared_ptr<RayInfo> Clone() {
		return std::dynamic_pointer_cast<RayInfo>(std::make_shared<tagHorzRay>(*this));
	};

	virtual bool Serialize(MemStream& memStream) {
		if (!RayInfo::Serialize(memStream))
			return false;

		memStream << PolarAngle << DiffStrength << RefStrength;
		
		/*memStream << Reflections.size() << Diffractions.size();
		if (Reflections.size() > 0) {
			for (auto ref : Reflections) {
				ref.Serialize(memStream);
			}
		}
		if (Diffractions.size() > 0) {
			for (auto dif : Diffractions) {
				dif.Serialize(memStream);
			}
		}*/

		return true;
	};

	virtual bool Deserialize(MemStream& memStream) {
		if (!RayInfo::Deserialize(memStream))
			return false;

		int refNumber, difNumber;
		memStream >> PolarAngle >> DiffStrength >> RefStrength;
		
		/*memStream >> refNumber >> difNumber;
		Reflections.clear();
		Reflection ref;
		for (size_t i = 0; i < refNumber; i++) {
			memStream >> ref;
			Reflections.push_back(ref);
		}

		Diffractions.clear();
		Diffraction dif;
		for (size_t i = 0; i < difNumber; i++) {
			memStream >> dif;
			Diffractions.push_back(dif);
		}*/

		return true;
	};

}HorzRay;