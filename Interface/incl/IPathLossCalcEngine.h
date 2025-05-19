#pragma once

#include "NETypes.h"
#include "IPathLossAccesser.h"

class IPathLossCalcEngine
{
public:
	virtual ~IPathLossCalcEngine() = default;

	virtual IPathLossAccesser* GetPathLossAccesser() const = 0;

	virtual void Calculate(int siteId, bool raySaved = false) = 0;

	virtual void Calculate(const std::list<std::shared_ptr<Transceiver>>& transceiverList, bool raySaved = false) = 0;

public:
};
