#pragma once

#include "PropInterface.h"
#include "PathLossCalcParams.h"
#include "RayInfo.h"
#include "UPlanCommon.h"
#include "UPlanCommonExport.h"
#include "IDataSerialize.h"
#include <vector>
#include <memory>

#define RAYSPECTRUM_VERSION	6

typedef struct tagRaySpectrumCalcFactor
{
	short AntHangupHeight;
	short StartAngle;
	short EndAngle;
	short Resolution;
	short AngleResolution;
	int MainCalcRadius;
	int CenterX;
	int CenterY;
	int Frequency;

	/// <summary>
	/// 地物映射描述
	/// </summary>
	std::string ClutterMatching;

	tagRaySpectrumCalcFactor() {};

	tagRaySpectrumCalcFactor(const tagRaySpectrumCalcFactor& rayCalcFactor)
	{
		upl_memcpy(this, &rayCalcFactor, upl_getoffset(tagRaySpectrumCalcFactor, ClutterMatching));
		ClutterMatching = rayCalcFactor.ClutterMatching;
	}

	void ToString(std::string& digest) const
	{
		char str[256];
		snprintf(str, 255, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,", \
			RAYSPECTRUM_VERSION, AntHangupHeight, StartAngle, EndAngle, Resolution, \
			AngleResolution, MainCalcRadius, CenterX, CenterY, Frequency);
		digest += str;
		digest.append(ClutterMatching);
	}
}RaySpectrumCalcFactor;

#define RW_ALL	0		//读写全部信息
#define RW_HEADINFO	1	//仅独写头信息

class IRayHandler
{
public:
	virtual ~IRayHandler() {};
	virtual void BeginHandling(size_t gridNumber) = 0;
	virtual void EndHandling() = 0;
	virtual void HandleOneRay(int gridNo, int rayNo, std::shared_ptr<RayInfo> ray) = 0;
};

typedef std::vector<std::shared_ptr<RayInfoList>> RaysVector;
class PROPINF_EXPORT_CLASS RaySpectrum : public IDataSerialize, IRayHandler
{
public:
	typedef struct tagHeadInfo {
		char headFlag[24];
		int version;
		size_t spotCount;
		size_t headSize;
	}HeadInfo;

public:
	RaySpectrum(const std::shared_ptr<AntPathLossCalcParam> antCalcParams);
	RaySpectrum();
	virtual ~RaySpectrum();
	
	static void SetCalcFactor(RaySpectrumCalcFactor& calcFactor, \
		const std::shared_ptr<AntPathLossCalcParam> antParams);

	static int CompareVersion(const char* filepath);

	virtual std::shared_ptr<RaySpectrum> Clone() const;

	void   CopyTo(RaySpectrum* pRaySpectrum) const;

	static HashValueType GetHash(const RaySpectrumCalcFactor& calcFactor);

	virtual HashValueType GetHash() const;

	virtual void WriteTxt(StreamFile& streamFile) const;

	virtual void Serialize(StreamFile& streamFile) const;

	virtual void Deserialize(StreamFile& streamFile);

	void SetRayInfoList(int index, std::shared_ptr<RayInfoList>);

	void SetRayInfo(int index, RayInfoList::iterator _begin, RayInfoList::iterator _end);

	std::shared_ptr<RayInfoList> GetRaysWithIndex(int index) const;

	void ClearBuffer(int index);

	inline const RaysVector& GetRaysVector() { return m_rayInfos; }

	virtual HeadInfo DeserializeHead(StreamFile& streamFile);

	virtual bool SerializeHead(StreamFile& streamFile) const;

	void SetRayHandler(IRayHandler* pRayHandler);

	static int GetUseCount();

	void ClearBuffer();

protected:
	std::shared_ptr<RayInfoList> operator[](int i);

	void CreateRaySpecArray(int arrSize);

	virtual void BeginHandling(size_t gridNumber);

	virtual void EndHandling();

	virtual void HandleOneRay(int gridNo, int rayNo, std::shared_ptr<RayInfo> ray);

public:
	RaySpectrumCalcFactor CalcFactor;

	unsigned char RWFlag = RW_ALL;  //0：读写所有内容；1：仅读写头信息
	
	//以下参数不需要计算md5值，但需要存储
	int AntConfigID;
	double CenterX;
	double CenterY;
	double CalcRadius;
	double Resolution;
	double AngleResolution;
	double StartAngle;
	double EndAngle;

	unsigned char EndFlag;

private:
	RaysVector m_rayInfos;
	IRayHandler *m_pRayHandler;
};

typedef std::vector< std::shared_ptr<RaySpectrum> > RaySpectrumVector;