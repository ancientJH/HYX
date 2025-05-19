#pragma once

#include "PropInterface.h"
#include "MeasurementFileParser.h"
#include "CellLoader.h"
#include "IGeoCoordinate.h"
#include "FileOperator.h"
#include <list>

class PROPINF_EXPORT_CLASS MeasuredDataParser : public IMeasurementDataParser
{
public:
	MeasuredDataParser(CellLoader* pCellLoader, IGeoCoordinate* pGeoCord);
	virtual ~MeasuredDataParser();

	// Í¨¹ý IDtDataParser ¼Ì³Ð
	virtual bool parse(MeasuredData* mrData, size_t maxMr, size_t& mrCount, const MeasuredRawData* dtRawData);
	std::map<std::string, std::shared_ptr<Transceiver>> transcvMap;
private:
	CellLoader* m_pCellLoader;
	IGeoCoordinate* m_pGeoCord;
};
