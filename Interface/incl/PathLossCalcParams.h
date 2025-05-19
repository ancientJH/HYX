#pragma once

#include "PropInterface.h"
#include "PropagationModel.h"
#include "IClutterParamsManager.h"
#include "NETypes.h"
#include "AntennaTypes.h"
#include "PropagationTypes.h"
#include "UPlanCommonExport.h"
#include "GeoExports.h"
#include "LiShuttleTypes.h"
#include <list>
#include <memory>
#include <math.h>
#include <map>
#include <set>
#include <vector>
#include <thread>

typedef enum tagCellCategory
{
	MacroCell = 0,
	MicroCell = 1,
	MiniCell = 2
}CellCategory;

#define MODE_PENELOSS_DEFAULT 0
#define MODE_MICROCELLLIMIT_DEFAULT 0

#define PL_ADDTIONAL_SUBDIR "addition"
#define AI_FEATURE_SUBDIR "features"
#define PYANDSH_SUBDIR "pyAndsh"

#define OUTDOOR_RAYSPEC_EXTNAME ".ray"
#define OUTDOOR_PATHLOSS_EXTNAME ".loss"
#define OUTDOOR_RAYTRACING_EXTNAME ".path"

#define INDOOR_RAYFILE_EXTNAME ".iry"
#define INDOOR_PATHLOSS_EXTNAME ".ils"
#define INDOOR_FIELD_EXTNAME ".field"

//测试数据锚点文件
#define FILENAME_OF_DTANCHOR	"meas.anchor"

#define CLUTTERMATCHSETTINGS_FILE "cluttermatchsettings.xml"
#define PYTHONSCHEMA_FILE "pythonschema.xml"

typedef struct tagPathlossofIndoorParams
{
	bool	isForcedCalc;
	double	calcResolution;
	double  maxCalcRadius;
	double	lengthError;
	double	minWallLengthInLos;
	double	ueHeight;
	int		maxReflectionNumber;
	int		maxDiffractionNumber;
	int		maxPenetrationNumber;
	int		maxRayNumber;
	int		maxDeepth;
	int		threadCount;
	bool	areRaysRecorded;

	std::string pathlossDirectory;
	
	tagPathlossofIndoorParams()
	{
		upl_bzero(this, upl_getoffset(tagPathlossofIndoorParams, pathlossDirectory));
	};

	tagPathlossofIndoorParams(const tagPathlossofIndoorParams& obj)
	{
		upl_memcpy(this, &obj, upl_getoffset(tagPathlossofIndoorParams, pathlossDirectory));
		this->pathlossDirectory = obj.pathlossDirectory;
	};
}PathlossofIndoorParams;

typedef struct tagPathlossCalcParams
{
	bool	isForcedCalc;
	int		peneLossMode;
	int		threadCount;
	char*	plAdditionalSubDir;
	double calcRadius;
	double calcResolution;
	double angleRes;
	double ueHeight;
	double edgeDistanceOfMerged;
	double MaxDistanceOfVS;
	double microThreshold;
	double macroThreshold;
	double microMaxRadius;
	double macroMinAntHeight;
	double shadowFadingForOutdoor;	//阴影衰落
	double shadowFadingForIndoor;
	bool	areRaysRecorded;
	bool	rayCacheSaved;
	bool	noCheckRay;
	bool	noCheckPathloss;
	bool	correctByDt;
	bool	usingBuildingPolygon;
	bool	antGainCalced;
	
	std::string clutterMatchFilePath;
	std::string clutterMatchName;
	std::string sceneSettingsFilePath;
	std::string pathlossDirectory;
	std::string dtFileFolder;
	std::string transFilter;		//cells needed to be calced. There IDs are divided by ","
	std::string featurePath;
	std::string filePathOfFocusRoads;
	std::string pythonSchema;
	
	tagPathlossCalcParams() 
	{
		upl_bzero(this, upl_getoffset(tagPathlossCalcParams, antGainCalced));
		usingBuildingPolygon = true;
		antGainCalced = true;
	};

	tagPathlossCalcParams(const tagPathlossCalcParams& obj)
	{
		upl_memcpy(this, &obj, upl_getoffset(tagPathlossCalcParams, clutterMatchFilePath));
		this->clutterMatchFilePath = obj.clutterMatchFilePath;
		this->clutterMatchName = obj.clutterMatchName;
		this->sceneSettingsFilePath = obj.sceneSettingsFilePath;
		this->pathlossDirectory = obj.pathlossDirectory;
		this->dtFileFolder = obj.dtFileFolder;
		this->transFilter = obj.transFilter;
		this->featurePath = obj.featurePath;
		this->filePathOfFocusRoads = obj.filePathOfFocusRoads;
		this->pythonSchema = obj.pythonSchema;
	};
}PathlossCalcParams;

typedef struct tagPathlossSceneFeature
{
	int gridNumberOfOutdoorRays;
	int gridNumberOfIndoorRays;
	int gridNumberOfLosRays;

	tagPathlossSceneFeature() { upl_bzero(this, sizeof(tagPathlossSceneFeature)); };

	tagPathlossSceneFeature(const tagPathlossSceneFeature& another) {
		upl_memcpy(this, &another, sizeof(tagPathlossSceneFeature));
	};

	tagPathlossSceneFeature& operator+=(const tagPathlossSceneFeature& another) {
		gridNumberOfOutdoorRays += another.gridNumberOfOutdoorRays;
		gridNumberOfIndoorRays += another.gridNumberOfIndoorRays;
		gridNumberOfLosRays += another.gridNumberOfLosRays;

		return *this;
	};

	tagPathlossSceneFeature operator+(const tagPathlossSceneFeature& another) const {
		tagPathlossSceneFeature newFeature(*this);
		newFeature.operator+=(another);

		return newFeature;
	};
}PathlossSceneFeature;

struct tagSectorPathLossCalcParam;
//目前按放射线方式设置参数
typedef struct tagSectionPathLossCalcParam
{
	double CalcResolution;
	double AngleResolution;
	double CalcEndDistance;
	double CalcStartDistance;
	double StartAngle;
	double EndAngle;
	double X;
	double Y;
	double BandWidth = 20;
	double Frequency;
	double ShadowFadingForOutdoor = 0;	//阴影衰落
	double ShadowFadingForIndoor = 0;	
	double MinBuildingPeneLoss = 0;
	double MergeEdgeMaxDis = 10;
	double MaxDistanceOfVS = 300;
	double RxAntennaHeight = 1.5f;		//离地高度
	double TxAntennaHeight;				//离地高度
	double microThreshold = 0.25;
	double macroThreshold = 0.75;
	double microMaxRadius = 500;
	double macroMinAntHeight = 15;
	int		threadCount = 0;
	int		PeneLossMode = MODE_PENELOSS_DEFAULT;
	int		MicroCellLimitMode = MODE_MICROCELLLIMIT_DEFAULT;
	bool	usingBuildingPolygon = true;
	bool	AntGainCalced = true;
	bool	RecordRayPaths = false;

	std::shared_ptr<PropagationModel> PropagModel;
	IClutterParamsManager* ClutterParamsManager;
	std::shared_ptr<tagSectorPathLossCalcParam> SectorCalcParam;
	std::string pythonSchema = "";

	short GetStartPointIndex() const{
		int num = (int)(CalcStartDistance / CalcResolution);
		return (short)max(num, 1);
	};

	short GetEndPointIndex() const {
		return (short)ceil((double)(CalcEndDistance / CalcResolution));
	};

	short GetPointNumberOnLine() const {
		int num = (GetEndPointIndex() - GetStartPointIndex()) + 1;
		return (short)num;
	};

	short GetScanningLineNumber() const	{
		int angle = (int)(EndAngle - StartAngle) % 360;
		if (angle <= 0)
			angle += 360;
		if (angle == 360)
			return (short)((double)angle / AngleResolution);
		else
			return (short)((double)angle / AngleResolution) + 1;
	};

}SectionPathLossCalcParam;

struct tagAntPathLossCalcParam;
typedef struct tagSectorPathLossCalcParam
{
	double DlFrequency;
	double UlFrequency;
	double BandWidth = 20;
	double TxAntennaHeight;		//离地高度
	double RxAntennaHeight;		//离地高度
	double MergeEdgeMaxDis = 10;
	double StartAngle = 0;
	double EndAngle = 360;
	double MainCalcRadius;
	double MainResolution;
	double MainAngleRes;
	double X;
	double Y;
	CellCategory cellCategory = CellCategory::MiniCell;
	int		threadCount = 0;
	int PeneLossMode = MODE_PENELOSS_DEFAULT;
	bool AntGainCalced;
	PathlossSceneFeature sceneFeature;
	
	std::shared_ptr<UCell> Cell;
	std::list<std::shared_ptr<SectionPathLossCalcParam>> SectionParamArray;
	std::shared_ptr<tagAntPathLossCalcParam> AntCalcParam;
}SectorPathLossCalcParam;
typedef std::vector<std::shared_ptr<SectorPathLossCalcParam>> SectorPathLossCalcParamVector;

typedef struct tagAntPathLossCalcParam
{
	int TransId;
	double StartAngle = 0;
	double EndAngle = 360;
	double RayTracingRadius;
	double RayTracingResolution;
	double RayTracingAngleRes;
	double X;
	double Y;
	double Frequency;
	AntInstParam* pAntennaGainParam;
	std::list< std::shared_ptr<SectorPathLossCalcParam> > SectorCalcParams;

	short GetEndPointIndex() const {
		return (short)ceil((double)(RayTracingRadius / RayTracingResolution));
	};

	short GetPointNumberOnLine() const {
		int num = GetEndPointIndex();
		return (short)num;
	};

	short GetScanningLineNumber() const {
		int angle = (int)(EndAngle - StartAngle) % 360;
		if (angle <= 0)
			angle += 360;
		return (short)(angle / RayTracingAngleRes);
	};

	int GetMainModelType() const {
		if (SectorCalcParams.empty())
			return -1;

		if (!SectorCalcParams.front() || SectorCalcParams.front()->SectionParamArray.empty())
			return -1;

		if (!SectorCalcParams.front()->SectionParamArray.front())
			return -1;

		if (!SectorCalcParams.front()->SectionParamArray.front()->PropagModel)
			return -1;

		return SectorCalcParams.front()->SectionParamArray.front()->PropagModel->PropModelType;
	};

	void ToString(std::string& digest) const
	{
		char str[256];
		snprintf(str, 255, "%d,%d,%d,%d,%d", (int)X, (int)Y, \
			(pAntennaGainParam) ? (int)(pAntennaGainParam->heightToGround) : 0, (int)RayTracingRadius, (int)Frequency);
		digest += str;
	}

}AntPathLossCalcParam;

bool PROPINF_EXPORT_FUNC SetupPathLossParams(PathlossCalcParams* pPlCalcParams, const tinyxml2::XMLElement* pSchema, const LiShuttleCommonParams* pCommonParams, const char* processFolder);

bool PROPINF_EXPORT_FUNC SetupPathLossofIndoorParams(PathlossofIndoorParams* pPlofIndoorParams, const tinyxml2::XMLElement* pSchema, const LiShuttleCommonParams* pCommonParams, const char* processFolder);