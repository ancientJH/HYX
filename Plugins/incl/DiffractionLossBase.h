#pragma once

#include "PropPlugins.h"
#include "UPlanCommonExport.h"

typedef struct tagPeakParam {
	float PeakHeight;
	float DistToTx;
	float DistToRx;
	float DistToBefore;

	tagPeakParam(float _height, float _distToTx, float _distToRx, float _distToBefore)\
		: PeakHeight(_height), DistToTx(_distToTx), DistToRx(_distToRx), DistToBefore(_distToBefore)
	{}

	tagPeakParam(const tagPeakParam& obj) {
		upl_memcpy(this, &obj, sizeof(tagPeakParam));
	}

	float GetTotalDistance() const {
		return DistToTx + DistToRx;
	};
}PeakParam;

typedef struct tagMultiPeakCalcParam
{
public:
    double DisFirstPeakToTx;
	double DisSecondPeakToTx;
	double TxAbs;
	double FirstPeakAbs;
	double RxAbs;
	double WaveLen;
	double SecondPeakAbs;
	double DisThirdPeakToTx;
	double ThirdPeakAbs;
	double DisFirstPeakToRx;
	double DisSecondPeakToRx;
	double DisThirdPeakToRx;
	double TxToRxLineLen;

	tagMultiPeakCalcParam() {
		upl_bzero(this, sizeof(tagMultiPeakCalcParam));
	};

	tagMultiPeakCalcParam(const tagMultiPeakCalcParam& obj) {
		upl_memcpy(this, &obj, sizeof(tagMultiPeakCalcParam));
	}
}MultiPeakCalcParam;

double GetTraSpl(double fLine1, double fLine2, double fRatio);

bool IsInFres(double weveLen, double peakHeight, double distToTx, \
	double distToRx, double txHeight, double rxHeight);

class DiffractionLossBase
{
public:
	DiffractionLossBase();
	virtual ~DiffractionLossBase();

	virtual double SinglePeakLoss(const MultiPeakCalcParam& calcuPara) const;

	virtual double DoublePeakLoss(const MultiPeakCalcParam& calcuPara) const = 0;

	virtual double TriplePeakLoss(const MultiPeakCalcParam& calcuPara) const = 0;

	virtual const char* DifLosMethodName() const = 0;

protected:
	 double GetTraSpl(double fLine1, double fLine2, double fRatio) const;

	 double EdgeDiff(double fWaveLen, double fHyper, double txToQDis, double qToRxDis) const;
};