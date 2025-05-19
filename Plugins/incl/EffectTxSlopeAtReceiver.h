#pragma once
#include <string>
#include <vector>
#include <math.h>
#include "PathLossCalcParams.h"
#include "EffTxHeightBase.h"
#include "GeoTypes.h"

using namespace std;
class EffectTxSlopeAtReceiver:public EffTxHeightBase
{
public:
	void CalcRayEffTxHt(double* pEffTxHeight, std::shared_ptr<SectionPathLossCalcParam> param, const short* pAltitudes, int startIndex, int endIndex);
	double GetSlope(std::shared_ptr<SectionPathLossCalcParam> param, const short* pAltitudes, int nIndex);
	int Trim(int fNumber, int Lower, int Upper);
	const char* EffectTxHeightMethodName() const;
	int mvfitgn(double* A, double* y, vector<double> a, int n, int dim, bool isAdjustClutter);
private:
	double GetPointEffHeights(int pointIndex, const short* pAltitudes, std::shared_ptr<SectionPathLossCalcParam> param, double midEffHeights);
	double RxMoreThanTx(double midEffHeights, double fDistance, const short* pAltitudes, short fRxPosAlt, short fTxPosAlt, double fSloAng);
	void ConstructMatrix(double aa, double* A, int i, int n, double* pAn, int dim);
	double CalcColumn(double* A, int i, int dim, int j, double aa, int n);
	int gcpelim(double* an, vector<double> a, int dim, bool isAdjustClutter);
	void SearchMaxvalue(double* an, int k, int dim, double& pelement, int& i0);
	void SaveMaxvalue(double value, double& pelement, int& i0, int i);
	bool ChangeValue(double& valueA, double& valueB, double& pelement);
	bool AdjustParamValid(vector<double> a, int i);
};