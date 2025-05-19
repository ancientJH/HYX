#pragma once

#include "PropInterface.h"
#include "PropagationModel.h"
#include "NETypes.h"
#include <map>
#include <memory>
#include <string>
#include <tinyxml2.h>

#define MODEL_VERSION_ELEMENT	"ModelVersion"
#define MODEL_LISHUTTLE_AUTOMODEL	"LiShuttleAuto"

typedef std::shared_ptr<PropagationModel>(*createModel)(XMLElement*);

class PROPINF_EXPORT_CLASS PropagationModelFactory
{
public:
	PropagationModelFactory();
	~PropagationModelFactory();

public:
	void AddModelCreater(const char* _type, createModel _creator);

	void LoadModels();

	void LoadSceneModels(std::shared_ptr<PropagationModel> sceneModels);

	void LoadCellModels(std::shared_ptr<PropagationModel> cellModels, const char* mapName);

	std::shared_ptr<PropagationModel> InsertModel(std::shared_ptr<PropagationModel> model, bool overwrite);

	std::shared_ptr<PropagationModel> GetPropagationModel(HashValueType modelHash, \
		const std::shared_ptr<UCell> pCell = nullptr);
	std::shared_ptr<PropagationModel> GetAutoModel(const std::shared_ptr<UCell> pCell = nullptr);

	std::shared_ptr<PropagationModel> GetCellModelSet();
	std::shared_ptr<PropagationModel> GetSceneModelSet();

	//inline std::map<size_t, std::shared_ptr<PropagationModel>>& GetPropagationModel() { return m_propagationModels; };

	static void GetCellModelFolder(char* szModelFile, size_t len);
	static void GetCellModelFile(char* szModelFile, size_t len, const char* mapName);
	static void GetSceneModelFolder(char* szModelFile, size_t len);
	static void GetSceneModelFile(char* szModelFile, size_t len);
	static void GetCustomSceneMapFile(char* szModelFile, size_t len);

	static PropagationModelFactory* instance();
	static void destroy();

protected:
	void LoadModelsFromFolder(const char* folder, std::shared_ptr<PropagationModel> pModel = NULL);

	void LoadModelXmlFile(const char* xmlFileName, std::shared_ptr<PropagationModel> pModel = NULL);

private:
	std::map<size_t, std::shared_ptr<PropagationModel> > m_propagationModels;
	std::shared_ptr<PropagationModel> m_cellModelSet;
	std::shared_ptr<PropagationModel> m_sceneModelSet;

	std::map<std::string, createModel> m_modelCreater;
};