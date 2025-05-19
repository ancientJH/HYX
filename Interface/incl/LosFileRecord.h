#pragma once

#include "PropFrame.h"
#include "UPlanCommonExport.h"
#include "GeoXYRect.h"

using namespace std;

#define MAX_LOSSFILEPATH_LEN 48
#define MAX_MD5_LENGTH 48
typedef struct tagLosFileRecord
{
	double left;
	double right;
	double bottom;
	double top;
	double resolution;
	size_t LosHashValue;
	size_t RayHashValue;
	char SysMD5[MAX_MD5_LENGTH];
	char PathLossFilePath[MAX_LOSSFILEPATH_LEN];
	char RayFilePath[MAX_LOSSFILEPATH_LEN];

	tagLosFileRecord() {
		upl_bzero(this, sizeof(tagLosFileRecord));
	};

	tagLosFileRecord(const tagLosFileRecord& obj) {
		upl_memcpy(this, &obj, sizeof(tagLosFileRecord));
	}

	void SetBound(const GeoRasterRect& calcBound) {
		this->left = calcBound.left;
		this->right = calcBound.right();
		this->bottom = calcBound.bottom();
		this->top = calcBound.top;
		this->resolution = calcBound.resolution;
	}

	tagLosFileRecord& operator=(const tagLosFileRecord& obj) {
		upl_memcpy(this, &obj, sizeof(tagLosFileRecord));
		return *this;
	}
}LosFileRecord;