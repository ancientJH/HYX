#pragma once

#include <string>
#include <memory>
#include "tinyxml2.h"
#include "PropInterface.h"

using namespace tinyxml2;

#define PM_SET_XMLELEMENT(_doc, _node, _eleName, _eleValue) {\
	XMLElement* pParam = (_node)->FirstChildElement(_eleName); \
	if (!pParam) {\
		pParam = dynamic_cast<XMLElement*>((_node)->InsertEndChild((_doc)->NewElement(_eleName))); \
	}\
	if (pParam) \
		pParam->SetText(_eleValue); \
}

class PROPINF_EXPORT_CLASS PropagationModel
{
public:
	PropagationModel();
	PropagationModel(const char* category, int modelID, int modelType);
	PropagationModel(const PropagationModel& model);

	virtual ~PropagationModel();

	size_t HashValue();

	static size_t HashValue(const char* category, int modelID);

	static int GetModelID(XMLElement* pModel);

	virtual int GetVersion() const = 0;

	virtual void ToString(std::string& str) = 0;

	virtual bool LoadElementsFromXML(const char* category, XMLElement* pModel) = 0;

	virtual tinyxml2::XMLElement* SetElementsToXML(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* xmlNode);

	virtual tinyxml2::XMLElement* InsertToXMLDoc(tinyxml2::XMLDocument* doc, bool overwrite) = 0;

	virtual const char* GetCategoryName() const = 0;

	virtual std::shared_ptr<PropagationModel> GetCellModel(const char* cellId, bool force = true) const;
	virtual std::shared_ptr<PropagationModel> GetDefaultModel() const;

public:
	int PropModelID;
	int PropModelType;
	float MinCoupleLoss;
	float EdgeDistanceOfMerged;
	float DiffractionCoeff;

	HashValueType AntInstHashValue;
	double	DlExtraLoss;

	std::string ModelName;
	std::string CategoryID;
	std::string EffectTxHeightMethod;
	std::string DifLossMethod;

	bool IsModelSet;
};
