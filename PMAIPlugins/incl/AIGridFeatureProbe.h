#pragma once

#include "PropAIPlugins.h"
#include "AIGridFeatureProbe.h"
#include "IDataSerialize.h"
#include "NETypes.h"
#include "CellAIGridFeature.h"
#include "MeasurementFileParser.h"
#include "LiShuttleAICalculator.h"
#include <map>
#include <set>

class PROPAIPLUGINS_EXPORT_CLASS AIGridFeatureProbe : public IDataSerialize, public IMeasuredDataVisitor
{
public:
	AIGridFeatureProbe(LiShuttleAICalculator* pAICalculator);
	virtual ~AIGridFeatureProbe();

public:
	virtual void call(MeasuredData* mrData);

	virtual void PostProcess();

	void SetCellParams(std::shared_ptr<AntPathLossCalcParam> antCalcParam);

	virtual void Serialize(StreamFile& streamFile) const;
	virtual void Deserialize(StreamFile& streamFile);
	virtual size_t GetHash() const;

	std::map<std::string, CellAIGridFeature*>& GetCellAIGridFeatures() { return m_cellAIGridFeatures; };

protected:
	void Clear();	

private:
	LiShuttleAICalculator* m_pAICalculator;
	std::map<std::string, CellAIGridFeature*> m_cellAIGridFeatures;
	std::map<std::string, std::shared_ptr<SectorPathLossCalcParam>> m_sectorParams;
};