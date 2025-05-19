#pragma once

#include "DiffractionLossBase.h"

class DeugoutDiffractionLoss: public DiffractionLossBase
{
public:
	DeugoutDiffractionLoss();

	double DoublePeakLoss(const MultiPeakCalcParam& calcuPara)const;

	double TriplePeakLoss(const MultiPeakCalcParam& calcuPara)const;

	const char* DifLosMethodName() const;

private:
	double getVmodulus(const MultiPeakCalcParam& calcuPara, double TxToRxLen, double TxToPeak, double peakHeight)const;

	double OneHighestPeakInTwo(const MultiPeakCalcParam& calcuPara) const;

	double TwoHighestPeakInTwo(const MultiPeakCalcParam& calcuPara)const;

	double OneHighestPeakInThree(const MultiPeakCalcParam& calcuPara)const;

	double getLossOfPeak(const MultiPeakCalcParam& calcuPara, double TxToRxLen, double TxToPeak, double peakHeight)const;

	double TwoHighestPeakInThree(const MultiPeakCalcParam& calcuPara)const;

	double ThreeHighestPeakInThree(const MultiPeakCalcParam& calcuPara)const;

};