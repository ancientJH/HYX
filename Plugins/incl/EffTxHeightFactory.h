#pragma once

#include "PropPlugins.h"
#include "EffTxHeightBase.h"
#include <map>
#include <string>

class EffTxHeightFactory
{
public:
	EffTxHeightFactory();
	~EffTxHeightFactory();

	EffTxHeightBase* GetEffTxHeightMethod(const char* methodName) const;

	static EffTxHeightFactory* instance();
	static void destroy();

protected:
	void Init();

private:
	std::map<std::string, EffTxHeightBase*> m_effTxHeightList;
};