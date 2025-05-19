#pragma once

#include "PropInterface.h"
#include "PathLossCalcParams.h"

class PROPINF_EXPORT_CLASS CellularType
{
public:
	CellularType(const short* pHeights, int len, float txHeight, float microThreshold,	float macroThreshold );
	~CellularType();
	
	CellCategory GetType();

private:
	const short* m_pHeights;
	int m_len;
	float m_txHeight;
	float m_microThreshold;
	float m_macroThreshold;
};