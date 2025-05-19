#pragma once

#include "PropInterface.h"
#include "MeasurementFileParser.h"
#include "CalibrationParams.h"

class  PROPINF_EXPORT_CLASS LiShuttleDtFileParser : public MeasurementFileParser
{
public:
	LiShuttleDtFileParser(IMeasurementDataParser* dtParser, double rsrpOffset = 0);
	virtual ~LiShuttleDtFileParser();

	// Í¨¹ý IDtFileParser ¼Ì³Ð
	virtual void Serialize(StreamFile& streamFile) const;
	
	virtual void Deserialize(StreamFile& streamFile);
	
	virtual size_t GetHash() const;

private:

};
