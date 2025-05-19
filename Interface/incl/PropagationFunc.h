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
/// dB转换到线性值
/// </summary>
/// <param name="dBValue">dB</param>
/// <returns>线性值</returns>
PROPINF_EXPORT_FUNC double dBto(double dBValue);
	
/// <summary>
/// 线性值转换到dB
/// </summary>
/// <param name="linearValue">线性值</param>
/// <returns>dB值</returns>
PROPINF_EXPORT_FUNC double todB(double linearValue);
	
/// <summary>
/// dBm转换到mw
/// </summary>
/// <param name="dBmValue"></param>
/// <returns></returns>
PROPINF_EXPORT_FUNC double dBmTomw(double dBmValue);
	
/// <summary>
/// mw转换到dBm
/// </summary>
/// <param name="mwValue"></param>
/// <returns></returns>
PROPINF_EXPORT_FUNC double mwTodBm(double mwValue);
	
/// <summary>
/// dBm转换到w
/// </summary>
/// <param name="dBmValue">dBm值</param>
/// <returns>以W为单位的功率值</returns>
PROPINF_EXPORT_FUNC double dBmtoW(double dBmValue);
	
/// <summary>
/// w转换到dBm
/// </summary>
/// <param name="linearValue">线性值</param>
/// <returns>dBm值</returns>
PROPINF_EXPORT_FUNC double WtodBm(double linearValue);
	
/// <summary>
/// dB值相加
/// </summary>
/// <param name="dBValue1"></param>
/// <param name="dBValue2"></param>
/// <returns>dB结果</returns>
PROPINF_EXPORT_FUNC double AdddB(double dBValue1, double dBValue2);
	
/// <summary>
/// dBm相加
/// </summary>
/// <param name="dBmValue1"></param>
/// <param name="dBmValue2"></param>
/// <returns>dBm结果</returns>
PROPINF_EXPORT_FUNC double AdddBm(double dBmValue1, double dBmValue2);
	
/// <summary>
/// dB相减
/// </summary>
/// <param name="dBValue1"></param>
/// <param name="dBValue2"></param>
/// <returns>dBValue1-dBValue2</returns>
PROPINF_EXPORT_FUNC double SubtractdB(double dBValue1, double dBValue2);
	
/// <summary>
/// dBm相减
/// </summary>
/// <param name="dBmValue1"></param>
/// <param name="dBmValue2"></param>
/// <returns>dBmValue1-dBmValue2</returns>
PROPINF_EXPORT_FUNC double SubtractdBm(double dBmValue1, double dBmValue2);
	
/// <summary>
/// dB到dBm转换
/// </summary>
/// <param name="dBValue"></param>
/// <returns></returns>
PROPINF_EXPORT_FUNC double dBWTodBm(double dBValue);
	
/// <summary>
/// dBm到dB转换
/// </summary>
/// <param name="dBmValue"></param>
/// <returns></returns>
PROPINF_EXPORT_FUNC double dBmTodBW(double dBmValue);