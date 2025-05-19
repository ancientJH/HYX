#include "PMAITrainingParams.h"

bool SetupPMAIParams(PMAITrainingParams* pPlAIParams, const tinyxml2::XMLElement* pSchema, \
	const LiShuttleCommonParams* pCommonParams, const char* processFolder)
{
	if (!SetupPathLossParams(pPlAIParams, pSchema, pCommonParams, processFolder)) {
		ULog_Write(ULOG_ERROR, "Error occurs when parsing the xml file for pathloss!");
		return false;
	}

	const XMLElement* pParam = NULL;

	XML_FORCE_PARSE_TEXT(pSchema, pParam, pPlAIParams->MDTpath, "DriverTestFileDictionaryPath");
	XML_PARSE_TEXT_WITH_DEFAULTVALUE(pSchema, pParam, pPlAIParams->pythonSchema, "PythonSchema", "pythonschema.xml");

	return true;
}