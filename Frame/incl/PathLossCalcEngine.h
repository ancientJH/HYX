#pragma once

#include "PropFrame.h"
#include "PropagationTypes.h"
#include "CalcParamFactory.h"
#include "PropagationCalculator.h"
#include "FileAccesser.h"
#include "PathLossCalcParams.h"
#include "SectorPathlossData.h"
#include "SectorPathlossMatrix.h"
#include "TransceiverPLAccesser.h"
#include "AppEngine.h"
#include "IPathLossCalcEngine.h"
#include "MeasuredDataAnchors.h"
#include <list>
#include <memory>
#include <string>

PROPFRAME_EXPORT_FUNC bool TransCompare(const std::shared_ptr<Transceiver>& a, const std::shared_ptr<Transceiver>& b);

class PROPFRAME_EXPORT_CLASS PathLossCalcEngine : public AppEngine, public IPathLossCalcEngine
{
public:
	PathLossCalcEngine(IServiceHelper* pServiceHelper);
	virtual ~PathLossCalcEngine();

	void SetRaySaved(bool raySaved = true);

	bool GetRaySaved() const;

	virtual bool Init();

	virtual void Run(const char* taskId);

	virtual const char* GetResultFolder() const;

	virtual IPathLossAccesser* GetPathLossAccesser() const;

	virtual void Calculate(int siteId, bool raySaved = false);

	virtual void Calculate(const std::list<std::shared_ptr<Transceiver>>& transceiverList, bool raySaved = false);

protected:
	void TraceRays(const std::list<std::shared_ptr<Transceiver>>& transceiverList);

	std::unique_ptr<TransceiverPLAccesser> CalculateOneTransceiver(const std::shared_ptr<Transceiver> transceiver, \
		std::unique_ptr<TransceiverPLAccesser> pTransceiverPLAccesser, bool raySaved = false);

	std::unique_ptr<SectorPathLossData> CalculateOneSector(const std::shared_ptr<SectorPathLossCalcParam> sectorCalcParam,\
		std::unique_ptr<SectorPathLossData> pSectorPathLoss);

	std::unique_ptr<SectorPathLossMatrix> SaveSectorPathLoss( \
			std::unique_ptr<SectorPathLossMatrix> sectorPathLossMat, \
			std::shared_ptr<RaySpectrum> antRaySpectrum);

	std::unique_ptr<SectorPathLossData> OutputPathLossTxt(const char* cellId, std::unique_ptr<SectorPathLossData> pSectorPathLoss) const;

	void PrintDebugInfo();

public:
	GeoXYRect m_mapRect;
	PathlossCalcParams m_calcParams;
	MeasuredDataAnchors m_measAnchors;
	CalcParamFactory* m_pCalcParamFactory;
	FileAccesser m_fileAccesser;
	std::string m_taskId;
	bool m_stopFlag;
};