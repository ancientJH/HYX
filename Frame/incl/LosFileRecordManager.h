#pragma once

#include "PropFrame.h"
#include <map>
#include <string>
#include "LosFileRecord.h"
#include "FileOperator.h"
#include "UPlanCommonExport.h"
#include "GeoXYRect.h"

using namespace uplancommon;

class LosFileRecordManager : IDataSerialize
{
public:
	LosFileRecordManager(FileOperator* fileOperator);
	virtual ~LosFileRecordManager();

	static void SetRecord(LosFileRecord& record, const char* cellName, \
		const GeoRasterRect& calcBound, size_t lossDigest, size_t rayDigest = 0, const char* sysMD = nullptr);
	
	bool ExistsRecordByCellName(const char* cellName) const;
	bool ExistsRecord(const char* key) const;
	LosFileRecord* FindRecordByCellName(const char* cellName);
	LosFileRecord* FindRecord(const char* searchKey);
	void SaveRecordFile();
	void LoadFromFile();
	void InsertRecord(LosFileRecord& newRecord);
	void UpdateRecord(LosFileRecord& losFileRecord);
	const char* GetRecordPath() const;

	virtual void Deserialize(StreamFile& streamFile);
	virtual void Serialize(StreamFile& streamFile) const;
	virtual size_t GetHash() const;
	virtual void WriteTxt(StreamFile& streamFile) const;

protected:
	void ClearData(LosFileRecord& curRecord);
	void RecordFileDataValidate();

private:
	FileOperator* m_fileOperator;
	std::map<std::string, LosFileRecord> m_losFileDic;
	uplancommon::Mutex m_mutex;
};