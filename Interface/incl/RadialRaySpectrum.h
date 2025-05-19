#pragma once

#include "PropInterface.h"
#include "RaySpectrum.h"
#include "PathLossCalcParams.h"
#include "RadialRegionData.h"
#include <memory>

class PROPINF_EXPORT_CLASS RadialRaySpectrum : public RaySpectrum
{
public:
	RadialRaySpectrum(std::shared_ptr<AntPathLossCalcParam> antCalcParams);
	RadialRaySpectrum();
	
	int GetDataOffset(int rayIndex) const;

	void AssignRegionData(RadialRegionData* pRegionData) const;

	/*得到指定扫描线、指定点的射线表*/
	std::shared_ptr<RayInfoList> GetRaysOnSpot(int lineNo, int spotNo);

	virtual std::shared_ptr<RaySpectrum> Clone() const;

	virtual size_t GetHash() const;

	virtual void WriteTxt(uplancommon::StreamFile& streamFile) const;

	virtual HeadInfo DeserializeHead(StreamFile& streamFile);

	virtual bool SerializeHead(StreamFile& streamFile) const;

protected:

public:
	int SampleNumberOnOneLine;
	int ScanningLineNumber;
};
