#pragma once
#include "PathLossMatrixBase.h"
#include "RadialRaySpectrum.h"
#include "AntennaTypes.h"
#include "MaskMatrix.h"
#include <memory>

class IPathLossExtractor {
public:
	virtual ~IPathLossExtractor() {};

	virtual std::shared_ptr<PathLossMatrixBase> GetPathLossMatrix(std::shared_ptr<MaskMatrix> regionMatrix, \
		const AntInstParam& antSettings, const std::shared_ptr<UCell> cell) = 0;

	virtual std::shared_ptr<RadialRaySpectrum> GetRays(const std::shared_ptr<UCell> cell, \
		unsigned char loadFlag) = 0;

	virtual bool IsPathLossFileExist(const char* cellName) const = 0;

	virtual bool IsRayFileExist(const char* cellName) const = 0;

	virtual size_t getFileLoadCnt() = 0;
};