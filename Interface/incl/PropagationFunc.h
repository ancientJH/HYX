#pragma once

#include "PropInterface.h"
#include "GeoExports.h"
#include "stddef.h"
#include <memory>

PROPINF_EXPORT_FUNC double GetPeneDeepDistance(double h, double d, double h1, double d1, double h2 = 0, double d2 = 0);

PROPINF_EXPORT_FUNC std::shared_ptr<GeoRadialArea> GetCalcedSector(GeoXYPoint center, double startAngle, double endAngle, \
	std::shared_ptr<GeoRadialArea> antSector);

PROPINF_EXPORT_FUNC void GetLossFileName(char* lossFileName, char* rayFileName, int bufSize, const char* cellName);

PROPINF_EXPORT_FUNC void GetDtFiles(std::vector<std::string>* pDtFiles, const char* dtDir);

PROPINF_EXPORT_FUNC void LoadClutterMatching(std::map<short, short>* pSceneMap, const char* xmlFileName, const char* clutterMatchingName = nullptr);

/// <summary>
/// dBת��������ֵ
/// </summary>
/// <param name="dBValue">dB</param>
/// <returns>����ֵ</returns>
PROPINF_EXPORT_FUNC double dBto(double dBValue);
	
/// <summary>
/// ����ֵת����dB
/// </summary>
/// <param name="linearValue">����ֵ</param>
/// <returns>dBֵ</returns>
PROPINF_EXPORT_FUNC double todB(double linearValue);
	
/// <summary>
/// dBmת����mw
/// </summary>
/// <param name="dBmValue"></param>
/// <returns></returns>
PROPINF_EXPORT_FUNC double dBmTomw(double dBmValue);
	
/// <summary>
/// mwת����dBm
/// </summary>
/// <param name="mwValue"></param>
/// <returns></returns>
PROPINF_EXPORT_FUNC double mwTodBm(double mwValue);
	
/// <summary>
/// dBmת����w
/// </summary>
/// <param name="dBmValue">dBmֵ</param>
/// <returns>��WΪ��λ�Ĺ���ֵ</returns>
PROPINF_EXPORT_FUNC double dBmtoW(double dBmValue);
	
/// <summary>
/// wת����dBm
/// </summary>
/// <param name="linearValue">����ֵ</param>
/// <returns>dBmֵ</returns>
PROPINF_EXPORT_FUNC double WtodBm(double linearValue);
	
/// <summary>
/// dBֵ���
/// </summary>
/// <param name="dBValue1"></param>
/// <param name="dBValue2"></param>
/// <returns>dB���</returns>
PROPINF_EXPORT_FUNC double AdddB(double dBValue1, double dBValue2);
	
/// <summary>
/// dBm���
/// </summary>
/// <param name="dBmValue1"></param>
/// <param name="dBmValue2"></param>
/// <returns>dBm���</returns>
PROPINF_EXPORT_FUNC double AdddBm(double dBmValue1, double dBmValue2);
	
/// <summary>
/// dB���
/// </summary>
/// <param name="dBValue1"></param>
/// <param name="dBValue2"></param>
/// <returns>dBValue1-dBValue2</returns>
PROPINF_EXPORT_FUNC double SubtractdB(double dBValue1, double dBValue2);
	
/// <summary>
/// dBm���
/// </summary>
/// <param name="dBmValue1"></param>
/// <param name="dBmValue2"></param>
/// <returns>dBmValue1-dBmValue2</returns>
PROPINF_EXPORT_FUNC double SubtractdBm(double dBmValue1, double dBmValue2);
	
/// <summary>
/// dB��dBmת��
/// </summary>
/// <param name="dBValue"></param>
/// <returns></returns>
PROPINF_EXPORT_FUNC double dBWTodBm(double dBValue);
	
/// <summary>
/// dBm��dBת��
/// </summary>
/// <param name="dBmValue"></param>
/// <returns></returns>
PROPINF_EXPORT_FUNC double dBmTodBW(double dBmValue);