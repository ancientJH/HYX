#pragma once

#include "PropagationModel.h"
#include "PropAIPlugins.h"
#include <string>

class PROPAIPLUGINS_EXPORT_CLASS LiShuttleAIModel : public PropagationModel
{
public:
	LiShuttleAIModel(int modelID);
	virtual ~LiShuttleAIModel();

	static const char CategoryName[];
	static std::shared_ptr<PropagationModel> CreatePropagationModel(XMLElement* pModel);

	virtual bool LoadElementsFromXML(const char* category, XMLElement* pModelElements);

	virtual int GetVersion() const;

	virtual tinyxml2::XMLElement* InsertToXMLDoc(tinyxml2::XMLDocument* doc, bool overwrite);

	virtual const char* GetCategoryName() const;
	
	virtual void ToString(std::string& str);

protected:
	virtual tinyxml2::XMLElement* SetElementsToXML(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* xmlNode) const;
};