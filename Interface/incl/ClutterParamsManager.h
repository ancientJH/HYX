#pragma once

#include "PropInterface.h"
#include "ISceneInfo.h"
#include "IClutterParamsManager.h"
#include <map>

class PROPINF_EXPORT_CLASS ClutterParamsManager : public IClutterParamsManager
{
public:
	ClutterParamsManager(ISceneInfo* pSceneInfo);
	virtual ~ClutterParamsManager();

	virtual const SceneInfomation& GetSceneParamsByClutter(short clutterCode) const;

	virtual const SceneInfomation& GetSceneParams(short sceneCode) const;
	
	virtual float GetPenetrationLoss(float* pLinearLoss, short& usedSceneCode, short clutterCode, \
		float deep, float freq) const;

	virtual float GetPenetrationLossOfScene(float* pLinearLoss, short& usedSceneCode, short sceneCode, \
		float deep, float freq) const;

	virtual short CorrectClutterCodeOfBuilding(short clutterID) const;

	virtual std::string str() const;

	//void SetClutterParams(const ClutterPropagtionParams&);
	void SetClutterParams(short clutterCode, short sceneCode);

protected:
	float GetPenetrationLossOfScene(float* pLinearLoss, const SceneInfomation& scene, \
		float deep, float freq) const;

private:
	ClutterParamsMap m_clutterParamsMap;
	ISceneInfo	*m_pSceneInfo;
	SceneInfomation m_defaultSceneParams;
};

