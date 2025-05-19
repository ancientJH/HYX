#pragma once

#include "PropInterface.h"
#include "ITextFileParser.h"
#include "IDataSerialize.h"
#include "CalibrationParams.h"

#define DT_SKIP_HEADLINE 1
#define DT_FIELDNUM (DT_RSRP_POS + 1)
#define DT_TIME_POS 0
#define DT_CELLID_POS 1
#define DT_LONG_POS 2
#define DT_LAT_POS 3
#define DT_RSRP_POS 4

class IMeasuredDataVisitor {
public:
	virtual ~IMeasuredDataVisitor() {};

	virtual void call(MeasuredData* mrData) = 0;

	virtual void PostProcess() = 0;
};

class IMeasurementDataParser
{
public:
	virtual ~IMeasurementDataParser() {};

	virtual bool parse(MeasuredData* mrData, size_t maxMr, size_t& mrCount, const MeasuredRawData* dtRawData) = 0;
};

#define MAX_CELLNUM_OF_ONEMR 6
class PROPINF_EXPORT_CLASS MeasurementFileParser : public IDataSerialize
{
public:
	MeasurementFileParser(IMeasurementDataParser* dtParser, double rsrpOffset);
	virtual ~MeasurementFileParser();

	virtual void PostProcess();

	void AddVisitor(IMeasuredDataVisitor* mrVisitor);

protected:
	virtual void CallVisitor(const MeasuredRawData* rawData);

	virtual void Parse(ITextFileParser* pFileParser);

protected:
	double m_rsrpOffset;
	MeasuredData m_mrData[MAX_CELLNUM_OF_ONEMR];
	std::list<IMeasuredDataVisitor*> Visitors;
	IMeasurementDataParser* DtDataParser;
};
