#pragma once

#include "PropIndoor.h"
#include "VirtualSourceTree.h"
#include "GeoBuilding.h"
#include "IDiffTreeManager.h"

#include <tuple>
#include <unordered_map>

class PROPINDOOR_EXPORT_CLASS DiffractionTreeManager : public IDiffTreeManager
{
public:
	#define VertDiffPt_FLOOR 0
	#define VertDiffPt_XY 1
	using VertDiffractionPoint = std::tuple<int, GeoXYPoint>;

	struct hash_DiffSource
	{
		double resolution = 0.05;
		std::size_t operator()(const VertDiffractionPoint& vtDiffPt) const
		{
			char strBuf[64];
			snprintf(strBuf, 64, "%d_%d_%d", std::get<VertDiffPt_FLOOR>(vtDiffPt), \
				round(std::get<VertDiffPt_XY>(vtDiffPt).x / resolution), round(std::get<VertDiffPt_XY>(vtDiffPt).y / resolution));
			std::string content = strBuf;
			std::hash<std::string> hasher;

			return hasher(content);
		};
		hash_DiffSource(double _res) : resolution(_res) {};
	};

	struct equal_DiffSource
	{
		double resolution = 0.05;
		bool operator()(const VertDiffractionPoint& pt1, const VertDiffractionPoint& pt2) const
		{
			return (std::get<VertDiffPt_FLOOR>(pt1) == std::get<VertDiffPt_FLOOR>(pt2)) \
				&& ArePointsNear1(std::get<VertDiffPt_XY>(pt1), std::get<VertDiffPt_XY>(pt2), resolution);
		};
		equal_DiffSource(double _res) : resolution(_res) {};
	};

	using VertDiffractionTreeMap = std::unordered_map<VertDiffractionPoint, VirtualSourceTree*, hash_DiffSource, equal_DiffSource>;
	using VertDiffractionTreePair = std::pair<VertDiffractionPoint, VirtualSourceTree*>;

	DiffractionTreeManager(const GeoBuilding* pGeoBuilding, double distRes, double minWallLen);
	virtual ~DiffractionTreeManager();

	virtual VSNode* GetVertDiffTreeRoot(int floorNo, double height, const GeoWallVertex* vertex, \
					RayTracingCounter tracingCounter, RayTracingCounter maxTracingNum);

private:
	double m_minWallLen;
	const GeoBuilding* m_pGeoBuilding;
	VertDiffractionTreeMap m_vertDiffTreeMap;
};