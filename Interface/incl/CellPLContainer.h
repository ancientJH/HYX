#pragma once

#include "PropInterface.h"
#include "PathLossMatrixBase.h"
#include "IPathLossExtractor.h"
#include "IPathLossCalcEngine.h"
#include "FileAccesser.h"
#include <mutex>
#include <unordered_set>

//非线程安全
class PROPINF_EXPORT_CLASS CellPLContainer : public IPathLossExtractor
{
public:
	CellPLContainer(IPathLossCalcEngine* pPathLossCalc);
	virtual ~CellPLContainer();

	// 通过 IPathLossExtractor 继承
	virtual std::shared_ptr<PathLossMatrixBase> GetPathLossMatrix(std::shared_ptr<MaskMatrix> maskMat, \
		const AntInstParam& antSettings, const std::shared_ptr<UCell> cell);

	virtual std::shared_ptr<RadialRaySpectrum> GetRays(const std::shared_ptr<UCell> cell, \
		unsigned char loadFlag);

	bool IsPathLossFileExist(const char* cellName) const;

	bool IsRayFileExist(const char* cellName) const;

	size_t getFileLoadCnt() { return 0; }

protected:
	void CalculatePathLoss(const std::shared_ptr<UCell> cell, bool raySaved = false);

private:
	IPathLossCalcEngine* m_pPathLossCalc;
	PathLossMatrixBase* m_pCellPathLossMatrix;
	std::unordered_set<int> m_calcedSite;
	std::mutex m_pathlossMutex;
};