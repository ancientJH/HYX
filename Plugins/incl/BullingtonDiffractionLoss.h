#pragma once
#include<iostream>
#include<vector>
#include"DiffractionLossBase.h"

class BullingtonDiffractionLoss : public DiffractionLossBase
{
public:
	BullingtonDiffractionLoss();

	double DoublePeakLoss(const MultiPeakCalcParam& calcParam)const;

	double TriplePeakLoss(const MultiPeakCalcParam& calcParam)const;

	const char* DifLosMethodName() const;
	
private:
	void BullingtonDoubleHeight(const MultiPeakCalcParam& calcParam,std::vector<double>& numvector)const;

	void CombinationUnite(double& d1, double& h, double d2, const MultiPeakCalcParam& calcPara)const;

};