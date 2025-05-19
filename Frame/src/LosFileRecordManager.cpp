#include "LosFileRecordManager.h"
#include "UPlanCommonExport.h"
#include "PropagationFunc.h"
#include <string>
#include <list>
#include <stdio.h>

using namespace uplancommon;

#define FILERECORD_NAME "index"

LosFileRecordManager::LosFileRecordManager(FileOperator* fileOperator) \
	: m_fileOperator(fileOperator), m_mutex()
{
	if (!fileOperator) {
		ULog_Write(ULOG_ERROR, "fileOperator can't be NULL!");
		throw;
	}
}

LosFileRecordManager::~LosFileRecordManager()
{
}

void LosFileRecordManager::ClearData(LosFileRecord& curRecord)
{
	m_fileOperator->DeleteFile(curRecord.PathLossFilePath);
}

void LosFileRecordManager::SetRecord(LosFileRecord& record, \
	const char* cellName, const GeoRasterRect& calcBound, size_t lossDigest, size_t rayDigest, const char* sysMD)
{
	record.left = calcBound.left;
	record.right = calcBound.left + calcBound.resolution * calcBound.columns;
	record.bottom = calcBound.top - calcBound.resolution * calcBound.rows;
	record.top = calcBound.top;
	record.resolution = calcBound.resolution;
	GetLossFileName(record.PathLossFilePath, record.RayFilePath, MAX_LOSSFILEPATH_LEN, cellName);
	record.LosHashValue = lossDigest;
	record.RayHashValue = rayDigest;
	if (!sysMD || '\0' == sysMD[0])
		record.SysMD5[0] = '\0';
	else
		upl_strcpy(record.SysMD5, MAX_MD5_LENGTH, sysMD);
}

bool LosFileRecordManager::ExistsRecordByCellName(const char* cellName) const
{
	char lossKey[MAX_LOSSFILEPATH_LEN];
	char rayKey[MAX_LOSSFILEPATH_LEN];
	GetLossFileName(lossKey, rayKey, MAX_LOSSFILEPATH_LEN, cellName);
	return this->ExistsRecord(lossKey);
}

bool LosFileRecordManager::ExistsRecord(const char* key) const
{
	return (m_losFileDic.find(key) != m_losFileDic.end());
}

LosFileRecord* LosFileRecordManager::FindRecordByCellName(const char* cellName)
{
	char lossKey[MAX_LOSSFILEPATH_LEN];
	char rayKey[MAX_LOSSFILEPATH_LEN];
	GetLossFileName(lossKey, rayKey, MAX_LOSSFILEPATH_LEN, cellName);
	auto iter = m_losFileDic.find(lossKey);
	return (iter!=m_losFileDic.end()) ? &(iter->second) : NULL;
}

LosFileRecord* LosFileRecordManager::FindRecord(const char* searchKey)
{
	auto iter = m_losFileDic.find(searchKey);
	return (iter != m_losFileDic.end()) ? &(iter->second) : NULL;
}

const char* LosFileRecordManager::GetRecordPath() const
{
	return FILERECORD_NAME;
}

void LosFileRecordManager::LoadFromFile()
{
	char* fullPath;
	if (!m_fileOperator->GetFullPath(&fullPath, FILERECORD_NAME))
		return;
	if ((-1 == upl_access(fullPath, MODE_EXISTENCE_ONLY)) \
		|| (UPL_PATH_MODE::P_IS_FILE != upl_pathmode(fullPath))) {
		FileOperator::FreePath(fullPath);
		return;
	}

	SpaceSeparatedFile indexFile;
	if (!indexFile.parseIndexFile(fullPath))
	{
		ULog_Write(ULOG_FATAL, "Couldn't parse %s!", fullPath);
		FileOperator::FreePath(fullPath);
		return;
	}
	FileOperator::FreePath(fullPath);

	//AutoMutex autoMutex(&m_mutex);
	m_losFileDic.clear();
	unsigned int lineNumber = indexFile.getLineNumber();
	for (unsigned int i = 0; i < lineNumber; i++) {
		try {
			unsigned int fieldNumber = indexFile.getFieldNum(i);
			if (fieldNumber == 0)
			{
				continue;
			}

			char cellName[MAX_LOSSFILEPATH_LEN];
			upl_strcpy(cellName, MAX_LOSSFILEPATH_LEN - 1, indexFile.getField(i, 0));
			cellName[upl_strlen(cellName) - 5] = '\0';

			LosFileRecord record;
			GetLossFileName(record.PathLossFilePath, record.RayFilePath, \
				MAX_LOSSFILEPATH_LEN - 1, cellName);
			record.LosHashValue = strtoull(indexFile.getField(i, 1), NULL, 10);
			record.left = upl_atof(indexFile.getField(i, 2));
			record.right = upl_atof(indexFile.getField(i, 3));
			record.bottom = upl_atof(indexFile.getField(i, 4));
			record.top = upl_atof(indexFile.getField(i, 5));
			record.resolution = upl_atof(indexFile.getField(i, 6));
			record.RayHashValue = (indexFile.getFieldNum(i) < 8) ? 0 : \
						strtoull(indexFile.getField(i, 7), NULL, 10);
			if (indexFile.getFieldNum(i) > 8) {
				upl_strcpy(record.SysMD5, MAX_MD5_LENGTH - 1, indexFile.getField(i, 8));
			}
			
			m_losFileDic.insert(std::pair<std::string, LosFileRecord>(\
				record.PathLossFilePath, record));
		}
		catch (...) {
			ULog_Write(ULOG_ERROR, "Failed to parse the %dth line!", i);
		}
	}
}

void LosFileRecordManager::SaveRecordFile()
{
	RecordFileDataValidate();
	if (0 != m_losFileDic.size())
	{
		m_fileOperator->SaveFile(FILERECORD_NAME, this);
	}
}

void LosFileRecordManager::UpdateRecord(LosFileRecord& losFileRecord)
{
	if (!this->ExistsRecord(losFileRecord.PathLossFilePath))
	{
		this->InsertRecord(losFileRecord);
	}
	else
	{
		//AutoMutex autoMutex(&m_mutex);
		m_losFileDic[losFileRecord.PathLossFilePath] = losFileRecord;
		SaveRecordFile();
	}
}

//void LosFileRecordManager::UpdateRecord(LosFileRecord& origRecord,\
//			LosFileRecord& losFileRecord)
//{
//	AutoMutex autoMutex(&m_mutex);
//	upl_memcpy(&losFileRecord, &origRecord, sizeof(LosFileRecord));
//	SaveRecordFile();
//}

void LosFileRecordManager::InsertRecord(LosFileRecord& newRecord)
{
	//AutoMutex autoMutex(&m_mutex);
	m_losFileDic.insert(std::pair<std::string, LosFileRecord>(\
		newRecord.PathLossFilePath, newRecord));
	SaveRecordFile();
}

size_t LosFileRecordManager::GetHash() const
{
	return 0;
}

void LosFileRecordManager::WriteTxt(StreamFile& streamFile) const
{
}

void LosFileRecordManager::Deserialize(StreamFile& streamFile)
{

}

void LosFileRecordManager::Serialize(StreamFile& streamFile) const
{
	if (streamFile.isOpen()) 
	{
		for (auto iter : m_losFileDic)
		{
			char recordStr[256];
			snprintf(recordStr, 255, "%s\t%s\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%s\t%s\n", \
				iter.second.PathLossFilePath, to_string(iter.second.LosHashValue).c_str(), iter.second.left, iter.second.right, \
				iter.second.bottom, iter.second.top, (int)(iter.second.resolution), \
				to_string(iter.second.RayHashValue).c_str(), iter.second.SysMD5);
			streamFile.write(recordStr, upl_strlen(recordStr));
		}
	}
}

void LosFileRecordManager::RecordFileDataValidate()
{
	std::list<std::string> delList;
	std::string lossFile, rayFile;
	for (auto iter = m_losFileDic.begin(); m_losFileDic.end() != iter; )
	{
		if (!m_fileOperator->IsFileExists(iter->second.PathLossFilePath) &&
			!m_fileOperator->IsFileExists(iter->second.RayFilePath) )
			iter = m_losFileDic.erase(iter);
		else
			++iter;
	}
}