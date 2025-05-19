#pragma once

#include "PropInterface.h"
#include "PathLossCalcParams.h"
#include "RayInfo.h"
#include "PlanetVectorFile.h"
#include "IDataSerialize.h"
#include "IGeoInfo.h"
#include <memory>
#include <list>
#include <string>

using RaysOnSpot = struct tagRaysOnSpot {
	
	int roadId;
	int samplingNo;
	double x;
	double y;
	double distance;		//采样点与AP的距离
	double rxPower;
	std::string cellId;
	RayInfoList rays;

	tagRaysOnSpot(int _roadId, int _samplingNo, double _x, double _y) : roadId(_roadId), samplingNo(_samplingNo), \
		x(_x), y(_y), distance(0), rxPower(NEGINFINITY), cellId("") {};

	bool AddRay(std::shared_ptr<RayInfo> rayInfo, int MaxRayNumber)
	{
		if (rays.size() < MaxRayNumber) {
			rays.push_back(rayInfo);
			return true;
		}

		return false;
	}

	void ToString(std::string& content, IGeoCoordinate* pCord) const
	{
		if (content.empty())
			return;
	};
};

using RaysOnScanningLine = struct tagRaysOnScanningLine {
	tagRaysOnScanningLine(double _angle) : angle(_angle) {};

	double angle;	//角度
	std::list<RaysOnSpot*> raysOnSpotList;
};

class PROPINF_EXPORT_CLASS RayTracker : public IDataSerialize, IPlanetVecFileSubscriber
{
public:
	RayTracker(const IGeoCoordinate* pCord, double calcResolution);
	~RayTracker();

	std::list<RaysOnScanningLine*>::const_iterator beginOfScanningLine() const;
	std::list<RaysOnScanningLine*>::const_iterator endOfScanningLine() const;

	std::list<RaysOnSpot*>::const_iterator beginOfSpots() const;
	std::list<RaysOnSpot*>::const_iterator endOfSpots() const;

	double	GetCalcResolution() const;

	void	GetMinMaxDistance(double& minDist, double& maxDist) const;

	size_t	GetCountInCell() const;

	void	LoadFile(const char* filePath);

	void ReplaceAnt(std::shared_ptr<AntPathLossCalcParam> antCalcParams);		//换一个新小区

	virtual void Serialize(StreamFile& streamFile) const;
	virtual void Deserialize(StreamFile& streamFile);
	virtual size_t GetHash() const;

	virtual void ReportNewVector(int id, int ptCount);
	virtual void SetPoint(int ptIndex, GeoXYPoint pt);
	virtual void CompleteVector(int id);
	virtual void CollectVector(int id);

protected:
	void ClearAllData();
	void ClearScanningLine();					//清除扫描线

	//返回采样点数量
	int	AddRoads(int roadId, int existedCount, double longitude1, double latitude1, double longitude2, double latitude2);

	void AddFootprint(int roadId, int samplingNo, double x, double y);

private:
	double m_calcResolution;
	const IGeoCoordinate* m_pCord;
	std::list<RaysOnScanningLine*> m_raysOnScanningLine;	//按扫描线装配采样点
	std::list<RaysOnSpot*> m_raysOnSpots;					//所有采样点的原始仓库

	int m_roadId;
	int m_countofCurrRoad;
	GeoXYPoint m_lastPt;
};
