#pragma once

#include "PropagationTypes.h"
#include "ISceneInfo.h"
#include <string>

typedef std::map<short, SceneInfomation> ClutterParamsMap;

class IClutterParamsManager
{
public:
	virtual ~IClutterParamsManager() {};

	virtual const SceneInfomation& GetSceneParamsByClutter(short clutterCode) const = 0;

	virtual const SceneInfomation& GetSceneParams(short sceneCode) const = 0;

	virtual float GetPenetrationLoss(float* pLinearLoss, short& usedSceneCode, short clutterCode, \
		float deep, float freq) const = 0;

	virtual float GetPenetrationLossOfScene(float* pLinearLoss, short& usedSceneCode, short sceneCode, \
		float deep, float freq) const = 0;

	virtual short CorrectClutterCodeOfBuilding(short clutterID) const = 0;

	virtual std::string str() const = 0;
};