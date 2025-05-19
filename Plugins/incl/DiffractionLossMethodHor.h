#pragma once
#include"LiShuttlePropagationModel.h"
#include"DiffractionLossBase.h"
#include"LiShuttlePathLossBase.h"
#include"DiffractionLossMethodHor.h"
#include"HorzRay.h"
#include<vector>
#include<list>

class PROPPLUGINS_EXPORT_CLASS DiffractionLossMethodHor
{
public:

	DiffractionLossMethodHor();
	
	void GetParam(double weveLen, GeoXYPoint peak_list , GeoXYPoint ts, GeoXYPoint rc, \
		const LiShuttleModelParams& propModel, MultiPeakCalcParam& VertcalcuParam);

	/*float GetPTLDistance(GeoXYPoint ts, GeoXYPoint rc, GeoXYPoint M);

	float GetPTPDistance(GeoXYPoint p1, GeoXYPoint p2);*/

	double CalcDifLoss(const CalcPathLossParam& calcParam, const std::list<Diffraction>& diffractons) ;

	double GetPTPDiffractionLoss(double weveLen, GeoXYPointList& peak_list, \
		GeoXYPoint ts, GeoXYPoint rc, MultiPeakCalcParam& VertcalcuParam, \
		const LiShuttleModelParams& propModel);

private :		
	DiffractionLossFactory* m_hDiffLossFactory;
	char* pDiffLosMethondName;
};