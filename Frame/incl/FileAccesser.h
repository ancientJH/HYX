#pragma once

#include "PropFrame.h"
#include "FileOperator.h"
#include "LosFileRecordManager.h"
#include "LosFileRecord.h"
#include "GeoXYRect.h"
#include "UPlanCommonExport.h"
#include "IPathLossAccesser.h"
#include <memory>

using namespace std;

typedef enum tagRunningStatus
{
	INITIALIZATION = 1,
	FAILED,
	PROCESSING,
	END
}RunningStatus;

class PROPFRAME_EXPORT_CLASS FileAccesser : public IPathLossAccesser
{
public:
	 FileAccesser();
	virtual ~ FileAccesser();

	void SetSysDigest(const char* digest);

	bool LoadRecordFile(const char* pathLossDir);

	bool SaveData(const char* fileName, IDataSerialize* pData);

	bool SavePathLossFile(const char* cellName, const GeoRasterRect& calcBound, \
		IDataSerialize* pLossData, IDataSerialize* pRayData = NULL);

	virtual bool LoadPathLossFile(const char* cellName, \
		IDataSerialize* pLossData, IDataSerialize* pRayData) const;
	
	virtual bool IsPathLossFileExist(const char* cellName) const;

	virtual bool IsRayFileExist(const char* cellName) const;
	
	virtual ReCalcType NeedReCalcPathLoss(const char* cellName, size_t lossHash, size_t rayHash) const;

	virtual const LosFileRecord* GetRecord(const char* cellName) const;
	
	virtual const char* GetPathLossDir() const;
	
protected:
	void SaveRecordFile();

	void ClearRecords();

	//bool IsNullOrFileDeleted(LosFileRecord* pOrigRecord) const;

private:
	FileOperator* m_pFileOperator;
	LosFileRecordManager* m_pLosFileManager;
	std::string m_sysDigest;
};