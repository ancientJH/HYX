#pragma once

#include "PropFrame.h"
#include "GeoInfo.h"
#include "PathLossCalcParams.h"
#include "PropagationModelFactory.h"
#include "AntCalcGain.h"
#include "PropagationTypes.h"
#include "ClutterParamsManager.h"
#include "AntennaLoader.h"
#include "ISceneInfo.h"
#include <list>
#include <map>
#include <memory>

class CalcParamFactory
{
public:
	CalcParamFactory(const PathlossCalcParams& calcParam, ISceneInfo* pSceneInfo, const char* city);
	virtual ~CalcParamFactory();

	std::shared_ptr<AntPathLossCalcParam> GetAntCalcParam(AntInstParam* pAntennaParam, \
		const std::list<std::shared_ptr<UCell>>& cells);

	void SetAntLibrary(AntennaLoader* pAntLibrary);

private:
	void GetSectorsCalcPara(std::shared_ptr<AntPathLossCalcParam> antCalcParam,
		const std::list<std::shared_ptr<UCell>>& cells);

	std::shared_ptr<SectorPathLossCalcParam> GetSectorParam(std::shared_ptr<UCell> cell, \
		std::shared_ptr<AntPathLossCalcParam> antCalcParam);

	void SetSectorCalcParam(std::shared_ptr<SectorPathLossCalcParam> sectorParam);

	std::shared_ptr<SectionPathLossCalcParam> CreateSectionParam(std::shared_ptr<SectorPathLossCalcParam> sectorParam, \
		double startRadius, PropModelConfig& pmc) const;

	IClutterParamsManager* GetClutterParamsManager();

	void LoadClutterMatching(const char*  xmlFileName, const char* clutterMatchingname);

	void LoadModelXmlFile(const char* xmlFileName);

private:
	ISceneInfo* m_pSceneInfo;
	std::map<short, short> m_clutterMatchMap;
	std::string m_city;
	
	PropagationModelFactory* m_pPropModelFactory;

	ClutterParamsManager* m_pClutterParamsManager;

	PathlossCalcParams m_calcParams;

	AntennaLoader* m_pAntLibrary;
};
