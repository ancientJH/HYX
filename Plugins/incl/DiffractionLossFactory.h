#pragma once

#include "PropPlugins.h"
#include "DiffractionLossBase.h"
#include <map>
#include <string>

class DiffractionLossFactory
{
public:
	DiffractionLossFactory();
	~DiffractionLossFactory();

	DiffractionLossBase* GetDiffLossMethod(const char* methodName) const;

	static DiffractionLossFactory* instance();
	static void destroy();

protected:
	void Init();

private:
	std::map<std::string, DiffractionLossBase*> m_diffLossList;
};