#pragma once

#include "PropAIPlugins.h"
#include "PathLossCalcParams.h"

typedef struct tagPMAITrainingParams : PathlossCalcParams {
	std::string MDTpath;

	tagPMAITrainingParams() : PathlossCalcParams() {};
	tagPMAITrainingParams(const tagPMAITrainingParams& that) : PathlossCalcParams((const PathlossCalcParams&)that){
		MDTpath = that.MDTpath;
	};
}PMAITrainingParams;

bool PROPAIPLUGINS_EXPORT_FUNC SetupPMAIParams(PMAITrainingParams* pPlAIParams, \
	const tinyxml2::XMLElement* pSchema, const LiShuttleCommonParams* pCommonParams, const char* processFolder);