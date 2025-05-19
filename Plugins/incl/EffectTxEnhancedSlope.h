#pragma once
#include <vector>
#include <string>
#include "EffTxHeightBase.h"
#include <stdio.h>
#include <math.h>
#include "GeoTypes.h"

using namespace std;

class EffectTxEnhancedSlope:public EffTxHeightBase {
	public:
		void CalcRayEffTxHt(double* pEffTxHeight, std::shared_ptr<SectionPathLossCalcParam> param, const short* pAltitudes, int startIndex, int endIndex);
		const char* EffectTxHeightMethodName() const;
	private:
		vector<double> FilterRece(const short* fHorig, int nIndex, std::shared_ptr<SectionPathLossCalcParam> param);
		vector<double> FilterTrans(const short* fHorig, std::shared_ptr<SectionPathLossCalcParam> param);
		int GetBinNumber(std::shared_ptr<SectionPathLossCalcParam> param, int nIndex, const short* pAltitudes, double fDistance, int BinNumR);
		double GetPointEffHeights(double midEffHeights, int pointIndex, std::shared_ptr<SectionPathLossCalcParam> param, const short* pAltitudes);
		int InfluR(const short* pAltitudes, int nIndex, std::shared_ptr<SectionPathLossCalcParam> param);
		void JudgeBinNum(int& i, int nIndex, int tempNum, const short* pAltitudes, double txHeight);
		vector<double> RxFrontLessThanBackForHalt(const short* fHorig, int i, vector<double> fRxHfilt);
		vector<double> RxFrontMoreThanBackForHalt(const short* fHorig, int i, vector<double> fRxHfilt, double temp);
		vector<double> TxBackLessThanFrontForHorig(const short* fHorig, int i, vector<double> fTxHfilt, double temp);
		vector<double> TxBackMoreThanFrontForHorig(const short* fHorig, int i, vector<double> fTxHfilt, double temp);
};