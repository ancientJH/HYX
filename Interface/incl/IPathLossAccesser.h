#pragma once

#include "PropInterface.h"
#include "LosFileRecord.h"
#include "IDataSerialize.h"

typedef unsigned char ReCalcType;
#define NEEDTO_CALC_PATHLOSS 0x01
#define NEEDTO_CALC_RAYSPEC 0x02

class IPathLossAccesser
{
public:
	virtual ~IPathLossAccesser() = default;

	virtual bool LoadPathLossFile(const char* cellName, \
		IDataSerialize* pLossData, IDataSerialize* pRayData) const = 0;

	virtual bool IsPathLossFileExist(const char* cellName) const = 0;

	virtual bool IsRayFileExist(const char* cellName) const = 0;

	virtual ReCalcType NeedReCalcPathLoss(const char* cellName, size_t lossHash, size_t rayHash) const = 0;

	virtual const LosFileRecord* GetRecord(const char* cellName) const = 0;

	virtual const char* GetPathLossDir() const = 0;
};