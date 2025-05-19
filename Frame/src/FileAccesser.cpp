#include "FileAccesser.h"
#include "UStreamFile.h"
#include "RaySpectrum.h"

FileAccesser::FileAccesser() : m_pFileOperator(NULL), m_pLosFileManager(NULL)
{
}

FileAccesser::~FileAccesser()
{
	ClearRecords();
}

void FileAccesser::ClearRecords()
{
	if (m_pLosFileManager) {
		delete m_pLosFileManager;
		m_pLosFileManager = NULL;
	}

	if (m_pFileOperator) {
		delete m_pFileOperator;
		m_pFileOperator = NULL;
	}
}

void FileAccesser::SetSysDigest(const char* digest)
{
	if (digest)
		m_sysDigest = digest;
}

bool FileAccesser::LoadRecordFile(const char* pathLossDir)
{
	if ((NULL == pathLossDir) || (upl_strlen(pathLossDir) == 0))
	{
		ULog_Write(ULOG_FATAL, "pathLossDir shouldn't be empty!");
		return false;
	}

	ClearRecords();

	m_pFileOperator = new FileOperator(pathLossDir);
	m_pLosFileManager = new LosFileRecordManager(m_pFileOperator);

	m_pLosFileManager->LoadFromFile();
	return true;
}

const char* FileAccesser::GetPathLossDir() const
{
	return m_pFileOperator->GetRootDir();
}

//bool FileAccesser::IsNullOrFileDeleted(LosFileRecord* pOrigRecord) const
//{
//	return (!pOrigRecord) || !m_pFileOperator->IsFileExists(pOrigRecord->PathLossFilePath);
//}

bool FileAccesser::LoadPathLossFile(const char* cellName, IDataSerialize* pLossData, IDataSerialize* pRayData) const
{
	if (NULL == cellName) {
		ULog_Write(ULOG_ERROR, "cellName can't be NULL!");
		return false;
	}
	bool flag = true;
	try
	{
		LosFileRecord record;
		LosFileRecord* pLosFileRecord = m_pLosFileManager->FindRecordByCellName(cellName);
		if (pLosFileRecord) {
			if (pLossData) {
				if (!m_pFileOperator->LoadFile(pLosFileRecord->PathLossFilePath, pLossData))
					throw;
			}
			if (pRayData) {
				if (!m_pFileOperator->LoadFile(pLosFileRecord->RayFilePath, pRayData))
					throw;
			}
		}
		else
			throw;
	}
	catch (...)
	{
		ULog_Write(ULOG_FATAL, "Failed to load pathlosses or rays!");
		flag = false;
	}
	return flag;
}

bool FileAccesser::SaveData(const char* fileName, IDataSerialize* pData)
{
	if ((NULL == fileName) || (NULL == pData)) {
		ULog_Write(ULOG_ERROR, "fileName or pData can't be NULL!");
		return false;
	}
	bool flag = true;
	try
	{
		m_pFileOperator->SaveFile(fileName, pData);
	}
	catch (...){
		ULog_Write(ULOG_FATAL, "Failed to save %s!", fileName);
		flag = false;
	}
		
	return flag;
}

bool FileAccesser::SavePathLossFile(const char* cellName,\
	const GeoRasterRect& calcBound, IDataSerialize* pLossData, IDataSerialize* pRayData)
{
	if (NULL == cellName) {
		ULog_Write(ULOG_ERROR, "cellName or pData can't be NULL!");
		return false;
	}
	bool flag = true;
	try
	{
		LosFileRecord record;
		LosFileRecord* pLosFileRecord = m_pLosFileManager->FindRecordByCellName(cellName);
		if (pLosFileRecord) {
			if (pLossData) {
				m_pFileOperator->DeleteFile(pLosFileRecord->PathLossFilePath);
				pLosFileRecord->LosHashValue = pLossData->GetHash();
			}

			if (pRayData) {
				m_pFileOperator->DeleteFile(pLosFileRecord->RayFilePath);
				pLosFileRecord->RayHashValue = pRayData->GetHash();
			}

			if (!m_sysDigest.empty())
				upl_strcpy(pLosFileRecord->SysMD5, MAX_MD5_LENGTH, m_sysDigest.c_str());

			pLosFileRecord->SetBound(calcBound);
		}
		else {
			LosFileRecordManager::SetRecord(record, cellName, calcBound, (pLossData) ? pLossData->GetHash() : 0, \
				(pRayData) ? pRayData->GetHash() : 0, m_sysDigest.empty() ? nullptr : m_sysDigest.c_str());
			pLosFileRecord = &record;
		}

		if (pRayData)
			m_pFileOperator->SaveFile(pLosFileRecord->RayFilePath, pRayData);
		if (pLossData)
			m_pFileOperator->SaveFile(pLosFileRecord->PathLossFilePath, pLossData);
		m_pLosFileManager->UpdateRecord(*pLosFileRecord);

		/*std::string completeFileRecordPath;
		m_pFileOperator->GetFullPath(completeFileRecordPath, m_taskId.c_str());
		StreamFile completeFile(completeFileRecordPath.c_str(), "a");
		if (completeFile.isOpen())
		{
			completeFile.write(cellName, upl_strlen(cellName));
			completeFile.write("\n", 1);
			completeFile.close();
		}*/
	}
	catch (...)
	{
		ULog_Write(ULOG_FATAL, "Failed to save pathloss!");
		flag = false;
	}
	return flag;
}

bool FileAccesser::IsPathLossFileExist(const char* cellName) const
{
	LosFileRecord* pLosFileRecord = m_pLosFileManager->FindRecordByCellName(cellName);
	if (pLosFileRecord)
	{
		return m_pFileOperator->IsFileExists(pLosFileRecord->PathLossFilePath);
	}
	return false;
}

bool FileAccesser::IsRayFileExist(const char* cellName) const
{
	LosFileRecord* pLosFileRecord = m_pLosFileManager->FindRecordByCellName(cellName);
	if (pLosFileRecord)
	{
		return m_pFileOperator->IsFileExists(pLosFileRecord->RayFilePath);
	}
	return false;
}

const LosFileRecord* FileAccesser::GetRecord(const char* cellName) const
{
	if (NULL == cellName) {
		ULog_Write(ULOG_ERROR, "cellName can't be NULL!");
		return NULL;
	}

	return m_pLosFileManager->FindRecordByCellName(cellName);
}

ReCalcType FileAccesser::NeedReCalcPathLoss(const char* cellName, size_t lossHash, size_t rayHash) const
{
	LosFileRecord* pOrigRecord = m_pLosFileManager->FindRecordByCellName(cellName);
	ReCalcType calcType = 0;
	if (!pOrigRecord)
		calcType = NEEDTO_CALC_PATHLOSS | NEEDTO_CALC_RAYSPEC;
	else if ( (m_sysDigest.empty()) || ('\0' == pOrigRecord->SysMD5[0]) \
		|| (0 != m_sysDigest.compare(pOrigRecord->SysMD5)) )
		calcType = NEEDTO_CALC_PATHLOSS | NEEDTO_CALC_RAYSPEC;
	else {
		if (!m_pFileOperator->IsFileExists(pOrigRecord->PathLossFilePath))
			calcType |= NEEDTO_CALC_PATHLOSS;
		else if (0 == lossHash || lossHash != pOrigRecord->LosHashValue)
			calcType |= NEEDTO_CALC_PATHLOSS;

		if (!m_pFileOperator->IsFileExists(pOrigRecord->RayFilePath))		//判断文件是否存在
			calcType |= NEEDTO_CALC_RAYSPEC;
		else if (0 == rayHash || rayHash != pOrigRecord->RayHashValue)		//判断hash值是否新
			calcType |= NEEDTO_CALC_PATHLOSS | NEEDTO_CALC_RAYSPEC;
		//else {																//判断文件版本是否一致
		//	char* fullPath = NULL;
		//	if (m_pFileOperator->GetFullPath(&fullPath, pOrigRecord->RayFilePath) && fullPath) {
		//		if (0 != RaySpectrum::CompareVersion(fullPath))
		//			calcType |= NEEDTO_CALC_PATHLOSS | NEEDTO_CALC_RAYSPEC;
		//		FileOperator::FreePath(fullPath);
		//	}else
		//		calcType |= NEEDTO_CALC_PATHLOSS | NEEDTO_CALC_RAYSPEC;
		//}  //文件版本号合并在hash值里，不用单独比较
	}

	return calcType;
}

void FileAccesser::SaveRecordFile()
{
	m_pLosFileManager->SaveRecordFile();
}