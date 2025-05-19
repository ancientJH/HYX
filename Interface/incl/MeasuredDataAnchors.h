#pragma once

#include "PropInterface.h"
#include "IDataSerialize.h"
#include "FileOperator.h"
#include "NETypes.h"
#include <UPlanCommon.h>
#include <list>
#include <map>

using PathlossLabel = struct tagPathlossLabel {
	float rad;
	float radius;
	float pathloss;
	float mainAntGain;

	tagPathlossLabel() { upl_bzero(this, sizeof(tagPathlossLabel)); };

	tagPathlossLabel(float _rad, float _radius, float _pathloss, float _antGain) : \
		rad(_rad), radius(_radius), pathloss(_pathloss), mainAntGain(_antGain) {};

	tagPathlossLabel(const tagPathlossLabel& obj) {
		upl_memcpy(this, &obj, sizeof(tagPathlossLabel));
	};
};

using CellPathlossLabels = struct tagCellPathlossLabels {
	HashValueType hashValue;
	std::list<PathlossLabel> plLabels;

	tagCellPathlossLabels(HashValueType v) : hashValue(v) {};
	tagCellPathlossLabels() : tagCellPathlossLabels(0) {};
};

class PROPINF_EXPORT_CLASS MeasuredDataAnchors : public IDataSerialize
{
public:
	MeasuredDataAnchors();
	virtual ~MeasuredDataAnchors();

	void AddPathlossLabel(std::shared_ptr<UCell> pCell, float rad, float radius, float pathloss, float antGain);

	std::list<PathlossLabel>* GetPathlossAnchors(std::shared_ptr<UCell> pCell);

	virtual void Serialize(StreamFile& streamFile) const;
	virtual void Deserialize(StreamFile& streamFile);
	virtual size_t GetHash() const;

	HashValueType GetCellHash(std::shared_ptr<UCell> pCell) const;

private:
	std::map<std::string, CellPathlossLabels> m_pathlossAnchors;
};
