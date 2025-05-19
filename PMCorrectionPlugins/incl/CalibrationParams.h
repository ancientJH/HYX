#pragma once

#include "PMCorrPlugins.h"
#include <string>
#include <vector>
#include <UPlanCommonExport.h>
#include "LiShuttleTypes.h"
#include "NETypes.h"
#include "tinyxml2.h"

//时间，cellid，经度，纬度，RSRP
#define DT_DATA_HEAD	"PCTime,cellID,Lon,Lat,RSRP"
#define DT_DATA_FORMAT	"%d:%d:%d,%s,%.7f,%.7f,%.2f"

#define DT_DATA_ADDITION_HEAD "rad,radius,distance,deltaHeight,azimuth,tilt,real_pathloss,ant_gain,error,pred_rsrp"
#define DT_DATA_ADDITION_FORMAT "%d,%d,%.0f,%.0f,%.1f,%.1f,%.2f,%.1f,%.1f,%.2f"

//第一遍清洗后的路测数据文件的扩展名
#define DTFILE_FILTER1_EXTNAME ".clean"

//校正整理后的栅格路测数据
#define FILENAME_OF_CLEANGRID	"clean_grids.csv"

//校正后的小区特征文件名
#define FILENAME_OF_CALIBARATION_CELL	"cell_summary.csv"

//校正后的场景特征文件名
#define FILENAME_OF_CALIBARATION_SCENE	"scene_summary.csv"

//场景分类配置文件名
#define SCENE_CLASSIFY_SETTING_FILE	"sceneclass.ini"

#define MAX_DIGEST_SIZE 200000000

using CALRESULT_TYPE = int;
#define CALSUCCESS	0
#define CALSUCCESS_SOLUTION			0x0F		//存在解的掩码
#define CALSUCCESS_OPTI_SOLUTION	0x01
#define CALSUCCESS_BORDER_SOLUTION	0x02
#define CALFAIL_UNKNOWN 0x100
#define CALFAIL_NOT_ENOUGH_SAMPLES	0x200
#define CALFAIL_DIST_RANGE_TOOSMALL	0x400
#define CALFAIL_DIST_GINI_TOOBIG	0x800
#define CALFAIL_NOT_ENOUGH_ACCURACY	0x1000
#define CALFAIL_LOS_CORRCOEF_TOOSMALL	0x10000
#define CALFAIL_NLOS_CORRCOEF_TOOSMALL	0x20000

typedef struct tagPMCorrectionCalcParam
{
	double maxPower;
	double minPower;
	/*double maxDistance;
	double minDistance;
	double minAngle;
	double maxAngle;*/
	double max_MeanError;
	double max_VarError;
	double calcResolution;
	double rsrpOffset;
	double distLimit;
	double minK0;
	double minKnear;
	double minDiff;
	double minRef;
	double minDeltaKfar;	//kfar-knear的最小值
	double maxK0;
	double maxKnear;
	double maxDiff;
	double maxRef;
	double maxDeltaKfar;	//kfar-knear的最大值
	double errorThreshold;	//单个栅格的误差门限（dB）
	double minRange;		//测试距离的最小极差（m）
	double minRangeBefore15;	//天线高度在15m以下的小蜂窝测试距离的最小极差（m）
	double minCorrCoef;		//rsrp与距离的最小相关系数
	double maxGiniCoef;		//测试距离的最大基尼系数
	double minAccRate;		//最小准确度

	int epoch;
	int minSamplesPerCell;	//每个小区的最少采样点数
	int threadCount;

	bool antGainCalced;
	bool isLosCalibrated;
	bool isNLosCalibrated;
	bool isCalExcu;
	bool isCalScene;
	bool isUpdateDtFiles;
	bool isFilterBackData;
	bool rebuildScene;		//重建场景模型（不继承以前的校正结果）
	bool forceCalibration;	//强行校正，即使数据质量不达标

	std::string dtFileDictionaryPath;
	std::string calibrationDirectory;
	std::string corModelName;
	std::string cellModelFile;

	tagPMCorrectionCalcParam() {
		upl_bzero(this, upl_getoffset(tagPMCorrectionCalcParam, antGainCalced));
		
		antGainCalced = true;
		isLosCalibrated = true;
		isNLosCalibrated = true;
		isCalExcu = false;
		isUpdateDtFiles = true;
		isCalScene = true;
		isFilterBackData = true;
		rebuildScene = false;
		forceCalibration = false;
	};

	tagPMCorrectionCalcParam(const tagPMCorrectionCalcParam& that) {
		upl_memcpy(this, &that, upl_getoffset(tagPMCorrectionCalcParam, dtFileDictionaryPath));
		dtFileDictionaryPath = that.dtFileDictionaryPath;
		calibrationDirectory = that.calibrationDirectory;
		corModelName = that.corModelName;
		cellModelFile = that.cellModelFile;
	};

}PMCorrectionCalcParam;

//路测数据分析添加
typedef struct tagDtTime
{
	unsigned char hour;
	unsigned char minute;
	unsigned char second;

	tagDtTime() { upl_bzero(this, sizeof(tagDtTime)); };

	void operator =(const tagDtTime& time) {
		upl_memcpy(this, &time, sizeof(tagDtTime));
	};

	void operator =(const char* strTime) {
		upl_bzero(this, sizeof(tagDtTime));
		if (strTime)
			sscanf(strTime, "%d:%d:%d", &hour, &minute, &second);
	};
}DtTime;

typedef struct tagMeasuredRawData
{
	DtTime time;
	double longitude = 0;
	double latitude = 0;
	double rsrp = 0;
	std::string cellID;

	tagMeasuredRawData() : longitude(0), latitude(0), rsrp(0) {};
	tagMeasuredRawData(const tagMeasuredRawData& obj) {
		upl_memcpy(this, &obj, upl_getoffset(tagMeasuredRawData, cellID));
		cellID = obj.cellID;
	}
}MeasuredRawData;

typedef enum tagCellServerType {
	CELL_BESTSERVER = 0,
	CELL_NEIGHBOR = 1
}CellServerType;

typedef struct tagMeasuredData {
	CellServerType servingType;
	DtTime time;
	double longitude;
	double latitude;
	double geoX = 0;
	double geoY = 0;
	double rsrp = 0;
	double distance2D = 0;
	double pathloss = 0;				//此处的路损包含了天线、馈线、无线信道等各种模型的增益与损耗
	double azimuthInAnt = 0;			//此处的工程角(与天线主瓣夹角)，0~360，正北为0，顺时针为正
	std::shared_ptr<Transceiver> transceiver;
}MeasuredData;

//自变量个数
#define INDEPVAR_COUNT	4
#define CROSS_VARPRODUCT_COUNT (INDEPVAR_COUNT+1)*INDEPVAR_COUNT/2
using InterParam = struct tagInterParam
{
	int		dataSize;
	double	sumY;
	double	squareY;
	double	sumX[INDEPVAR_COUNT];
	double	squareX[INDEPVAR_COUNT];
	double	sumProduct[CROSS_VARPRODUCT_COUNT];	/*变量两两的乘积，0: y*x1; 1: y*x2;	2: x1*x2*/

	tagInterParam() {
		Reset();
	};

	tagInterParam(const tagInterParam& obj) {
		upl_memcpy(this, &obj, sizeof(tagInterParam));
	}

	void Reset() {
		upl_bzero(this, sizeof(tagInterParam));
	};

	tagInterParam& operator+=(const tagInterParam& another) {
		dataSize += another.dataSize;
		sumY += another.sumY;
		squareY += another.squareY;
		for (size_t i = 0; i < INDEPVAR_COUNT; ++i) {
			sumX[i] += another.sumX[i];
			squareX[i] += another.squareX[i];
		}
		size_t crossNum = CROSS_VARPRODUCT_COUNT;
		for (size_t i = 0; i < crossNum; ++i)
			sumProduct[i] += another.sumProduct[i];

		return *this;
	};

	tagInterParam operator+(const tagInterParam& another) const {
		tagInterParam newParam(*this);
		newParam.operator+=(another);

		return newParam;
	};

	void SetElementsToXML(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* xmlNode) const {
		PM_SET_XMLELEMENT(doc, xmlNode, "dataSize", dataSize);
		PM_SET_XMLELEMENT(doc, xmlNode, "sumY", sumY);
		PM_SET_XMLELEMENT(doc, xmlNode, "squareY", squareY);
		std::string strX, strSX, strProduct;
		char buf[32];
		for (int i = 0; i < INDEPVAR_COUNT; ++i) {
			if (i > 0) {
				strX.append(",");
				strSX.append(",");
			}
			upl_snprintf(buf, 32, "%.4f", sumX[i]);
			strX.append(buf);
			upl_snprintf(buf, 32, "%.4f", squareX[i]);
			strSX.append(buf);
		}
		PM_SET_XMLELEMENT(doc, xmlNode, "sumX", strX.c_str());
		PM_SET_XMLELEMENT(doc, xmlNode, "squareX", strSX.c_str());
		for (int i = 0; i < (INDEPVAR_COUNT + 1) * INDEPVAR_COUNT / 2; ++i) {
			if (i > 0)
				strProduct.append(",");
			upl_snprintf(buf, 32, "%.4f", sumProduct[i]);
			strProduct.append(buf);
		}
		PM_SET_XMLELEMENT(doc, xmlNode, "sumProduct", strProduct.c_str());
	};

	#define PICK_MULTI_DOUBLE(_str, _values, _max) \
		if (!(_str).empty()) { \
			int k = 0; \
			size_t head = 0, tail = 0; \
			do { \
				tail = str.find(',', head); \
				if (tail == head) \
					(_values)[k] = 0; \
				else if ((tail > head) && (tail < str.size())) \
					(_values)[k] = atof((_str).substr(head, tail - head).c_str()); \
				else { \
					(_values)[k] = atof((_str).c_str() + head); \
					break; \
				} \
				head = tail + 1; \
				++k; \
				if (k >= (_max)) \
					break; \
			} while (1); \
		}

	void LoadElementsFromXML(tinyxml2::XMLElement* xmlNode) {
		XMLElement* pParam = xmlNode->FirstChildElement("dataSize");
		if (pParam)
			pParam->QueryIntText(&dataSize);

		pParam = xmlNode->FirstChildElement("sumY");
		if (pParam)
			pParam->QueryDoubleText(&sumY);

		pParam = xmlNode->FirstChildElement("squareY");
		if (pParam)
			pParam->QueryDoubleText(&squareY);

		pParam = xmlNode->FirstChildElement("sumX");
		std::string str = (pParam) ? pParam->GetText() : "";
		PICK_MULTI_DOUBLE(str, sumX, INDEPVAR_COUNT);

		pParam = xmlNode->FirstChildElement("squareX");
		str = (pParam) ? pParam->GetText() : "";
		PICK_MULTI_DOUBLE(str, squareX, INDEPVAR_COUNT);
		
		pParam = xmlNode->FirstChildElement("sumProduct");
		str = (pParam) ? pParam->GetText() : "";
		PICK_MULTI_DOUBLE(str, sumProduct, (INDEPVAR_COUNT + 1) * INDEPVAR_COUNT / 2);
	};
};

//校正模型的约束条件
typedef struct tagModelRestrictions
{
	double minK0;
	double minKnear;
	double minDiff;
	double minRef;
	double minDeltaKfar;			//kfar-knear的最小值
	double maxK0;
	double maxKnear;
	double maxDiff;
	double maxRef;
	double maxDeltaKfar;			//kfar-knear的最大值
	int minSamples;			//最少采样点数
	double minCorrCoef;	//最小相关系数
}ModelRestrictions;

PMCORR_EXPORT_FUNC bool SetupCalcParams(PMCorrectionCalcParam* pmcorrParams, const tinyxml2::XMLElement *pSchema, const LiShuttleCommonParams* pCommonParams);