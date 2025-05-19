#pragma once

#include "UPlanCommonExport.h"
#include "IMaterialFinder.h"
#include "GeoDataUtility.h"
#include <list>
#include <map>

//common const values
#define PATHLOSS_INVALID_VALUE -32768
#define PATHLOSS_INVALID_STORED_VALUE -32768
#define PATHLOSS_MINVALUE -100
#define PATHLOSS_MAXVALUE 300
#define PATHLOSS_MINVALUE_SHORT -5000
#define PATHLOSS_MAXVALUE_SHORT 30000
#define PATHLOSS_INVALID_CELLID -32768
#define PATHLOSS_MINRESOLUTON 0.01
#define PATHLOSS_MAXRESOLUTON 10000
#define PATHLOSS_MINANGLERESOLUTON 0.01
#define PATHLOSS_MAXANGLERESOLUTON 360
#define PATHLOSS_MINCALCRADIUS 1
#define PATHLOSS_MAXCALCRADIUS 10000
//#define PATHLOSS_STRENGTH_FLOORVALUE 1e-14
//#define PATHLOSS_STRENGTH_UNDERSTATION_MIN 1e-13
#define PATHLOSS_POWER_FLOORVALUE 1e-28
#define PATHLOSS_POWER_UNDERSTATION_MIN 1e-26
#define MAX_DIFF_STRENGTH 0.95
#define MAX_REF_STRENGTH 0.99

#define PATHLOSS_SCALE 100

#define CLASSIC_MODEL 0             
#define RAYTRACING_MODEL 1
#define AI_MODEL 2

/*propagation model definition directory*/
#define PL_PROPAGATION_MODEL_DIR "models"
#define PL_SCENE_MODEL_DIR "scenes"
#define PL_CELL_MODEL_DIR "cells"
#define SCENE_MODEL_NAME "SceneModel.xml"
#define CUSTOM_SCENE_MAP "custom_cellscene_map.ini"

#define Category_LiShuttleModel "LiShuttleModel"
#define Category_LiShuttleCellModel "LiShuttleCellModel"
#define Category_LiShuttleSceneModel "LiShuttleSceneModel"
#define Category_LiShuttleAIModel "LiShuttleAIModel"

//最大合并径数
#define MAX_COMBINED_PATH 1

#define MAX_COMBINATION_RAY 15		//单个栅格的最大射线条数
#define MAX_VERTDIFF_RAY 4			//垂直衍射的最大射线条数

#define DEFAULT_MATERIAL_CODE 9999

#define DATASERVICE_OF_PATHLOSS	"PathLoss"
#define DATASERVICE_OF_INDOORRAY	"RaysOfIndoor"

typedef enum tagPathlossCalcRange
{
	/*Calculate all the map range specified by the user.*/
	WholeGrid = 0,
}PathlossCalcRange;

