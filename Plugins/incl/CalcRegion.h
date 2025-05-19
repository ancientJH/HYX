#pragma once

#include "PropPlugins.h"
#include "PropagationExports.h"
#include "ScanningLineInfo.h"
#include "ScanningSpotInfo.h"
#include "HorzNode.h"
#include "RayPath.h"
#include <memory>

class GISDATA_EXPORT_CLASS CalcRegion
{
public:
	using TypeofSpotInCalcRegion = enum tagTypeofSpotInCalcRegion {
		OUTSIDE = 0,
		INSIDE_OUTDOOR = 1,
		INSIDE_INDOOR = 2
	};

public:
	CalcRegion(std::shared_ptr<GeoRadialArea> sectorRegion, \
		std::shared_ptr<GeoXYLine> endLine, std::shared_ptr<HorzNode>, double txHeight, bool is3D = false);
	~CalcRegion();

	bool GetScanningSpots(SamplingSpotList& spotInfoList, \
				ScanningLineInfo* pScanningLine, bool restrictedOurdoor, \
				double maxDistance = -1, bool isOpti = true) const;

	bool GetAllScanningSpots(SamplingSpotList& outdoorSpots,
		SamplingSpotList& indoorSpots, ScanningLineInfo* pScanningLine, double maxDistance = -1) const;

	TypeofSpotInCalcRegion GetSpotType(GeoXYPoint pt, GeoBuildingPtr building) const;

	const std::shared_ptr<GeoRadialArea> GetSectorRegion() const;

	const std::shared_ptr<GeoXYLine> GetEndLine() const;

	const shared_ptr<HorzNode> GetHorzNode() const;
	shared_ptr<HorzNode> GetHorzNode();

	RayPath* GetRayPath();

	static int GetUseCount();

protected:
	bool GetSpotsOfSector(SamplingSpotList& spotInfoList, \
		ScanningLineInfo* pScanningLine, bool restrictedOurdoor, double maxDistance, bool isOpti = true) const;

	bool GetSpotsOfPolygon(SamplingSpotList& spotInfoList, \
		ScanningLineInfo* pScanningLine, const std::vector<GeoXYPoint>& geoPolygon, \
		bool restrictedOurdoor, double maxDistance, bool isOpti) const;

	void GetSpotsOnLineInSector(const ScanningLineInfo* pScanningLine, \
		const std::shared_ptr<GeoRadialArea> sectorArea, int& start, int& end, double maxDistance, bool isOpti = true) const;

	void SearchSpotsInArea(const ScanningLineInfo* pScanningLine, \
		const void* area, bool isSector, GeoXYPoint* pPt0, GeoXYPoint* pPt1, \
		int& start, int& end, double maxDistance) const;

	void Section(const ScanningLineInfo* pScanningLine, int start, int end, \
		int& outdoorStart, int& outdoorEnd, int& indoorStart, int& indoorEnd) const;

	bool IsPointInArea(GeoXYPoint pt, const void* area, bool isSector) const;

	bool IsPointInPolygon(GeoXYPoint pt, const std::vector<GeoXYPoint>* pPoly) const;

	void CreatePolygon(std::vector<GeoXYPoint>& geoPolygon) const;

	void GetSpotIndexInArea(const ScanningLineInfo* pScanningLine, const void* area, \
		bool isSector, int& start, int& end, double maxDistance) const;

	GeoXYRect GetMiniEnclosingRect(bool includingEndLine = true) const;

private:
	int m_ID;
	double m_txHeight;
	bool m_is3D;
	RayPath* m_pRayPath;
	std::shared_ptr<GeoRadialArea> m_sectorRegion;
	std::shared_ptr<GeoXYLine> m_endLine;
	std::shared_ptr<HorzNode> m_horzNode;
};

typedef std::vector<std::shared_ptr<CalcRegion>> CalcRegionVector;
typedef std::vector<std::shared_ptr<CalcRegion>> CalcRegionList;