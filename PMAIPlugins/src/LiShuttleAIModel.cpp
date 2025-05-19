#include "LiShuttleAIModel.h"
#include "UPlanCommonExport.h"
#include "PropagationTypes.h"

const char LiShuttleAIModel::CategoryName[] = Category_LiShuttleAIModel;

#define LiShuttleAIModelVersion 0

LiShuttleAIModel::LiShuttleAIModel(int modelID)\
	: PropagationModel(LiShuttleAIModel::CategoryName, modelID, AI_MODEL)
{

}

LiShuttleAIModel::~LiShuttleAIModel()
{

}

int LiShuttleAIModel::GetVersion() const
{
	return LiShuttleAIModelVersion;
}

const char* LiShuttleAIModel::GetCategoryName() const
{
	return LiShuttleAIModel::CategoryName;
}

std::shared_ptr<PropagationModel> LiShuttleAIModel::CreatePropagationModel(XMLElement* pModel)
{
	int modelID = GetModelID(pModel);

	return modelID < 0 ? nullptr : std::make_shared<LiShuttleAIModel>(modelID);
}

bool LiShuttleAIModel::LoadElementsFromXML(const char* category, XMLElement* pModelElements)
{
	/*if ((NULL == category) || (0 != upl_strcmp(category, LiShuttleAIModel::CategoryName)))
		return false;*/

	return true;
}

tinyxml2::XMLElement* LiShuttleAIModel::SetElementsToXML(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* xmlNode) const
{
	return xmlNode;
}

tinyxml2::XMLElement* LiShuttleAIModel::InsertToXMLDoc(tinyxml2::XMLDocument* doc, bool overwrite)
{
	return nullptr;
}

void LiShuttleAIModel::ToString(std::string& str)
{
	PropagationModel::ToString(str);
}