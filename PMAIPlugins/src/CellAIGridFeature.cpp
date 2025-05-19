#include "CellAIGridFeature.h"
#include "UPlanCommonExport.h"
#include "RadialSectionSpotData.h"
#include "LiShuttleAICalculator.h"
#include <stdlib.h>


void CellAIGridFeature::CountHeightAndClutter(short clutter, short height) {
	m_clutters[clutter - 1]++;
	int index = height <= 10 ? 0 : \
		height <= 20 ? 1 : \
		height <= 30 ? 2 : \
		height <= 40 ? 3 : \
		height <= 50 ? 4 : \
		height <= 60 ? 5 : \
		height <= 70 ? 6 : \
		height <= 80 ? 7 : \
		height <= 90 ? 8 : \
		height <= 100 ? 9 : \
		height <= 150 ? 10 : \
		height <= 200 ? 11 : \
		height <= 250 ? 12 : \
		height <= 300 ? 13 : 14;
	if (index == 14) {
		m_heights[index]++;
	}
	else
	{
		for (int i = index; i < 14; i++) {
			m_heights[i]++;
		}
	}
	//m_heights[index]++;
}

void CellAIGridFeature::GetCellFeature(char* feature, size_t bufSize) const
{
	if (!feature) {
		ULog_Write(ULOG_ERROR, "Null value of feature!");
		return;
	}
	if (!m_sectorParam) {
		ULog_Write(ULOG_ERROR, "No sectorParam");
		return;
	}
	
	std::string cellID = m_sectorParam->Cell->cellID;
	auto antModel = m_sectorParam->AntCalcParam->pAntennaGainParam->antModel;
	auto antInst = m_pAICalculator->m_pAntCalcGain->GetAntennaParams(antModel.c_str());
	int horz3dBBeamWidth;
	int vert3dBBeamWidth;
	int antMaxGain;
	if (antInst) {
		horz3dBBeamWidth = (int)antInst->horz3dBBeamWidth;
		vert3dBBeamWidth = (int)antInst->vert3dBBeamWidth;
		antMaxGain = antInst->GetMaxGain();
	}
	else
	{
		horz3dBBeamWidth = 0;
		vert3dBBeamWidth = 0;
		antMaxGain = 0;
	}
	CellCategory cellCategory = m_sectorParam->cellCategory;
	AntMountType antMount = m_sectorParam->AntCalcParam->pAntennaGainParam->mountType;
	//int antMaxGain = antInst->GetMaxGain();
	short antTxNumber = m_sectorParam->Cell->parent->numOfTrans;
	int antHeightToGround = m_sectorParam->TxAntennaHeight;
	int centerFrep = m_sectorParam->DlFrequency / 100;
	int realPointNubmerOnLine = m_sectorParam->SectionParamArray.front()->GetPointNumberOnLine();
	string clutters = "";
	string heights = "";
	for (auto c : m_clutters)
		clutters += (std::to_string(c) + "s");

	for (auto h : m_heights)
		heights += (std::to_string(h) + "s");

	snprintf(feature, bufSize, "%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.2f,%s,%s\n", \
		cellID.c_str(), horz3dBBeamWidth, vert3dBBeamWidth, cellCategory, \
		antMount, antMaxGain, antTxNumber, \
		centerFrep, antHeightToGround, \
		m_MDTsum, m_sumData, m_range60, m_range90, m_maxDistOfSignal, clutters.c_str(), heights.c_str());

}

CellAIGridFeature::CellAIGridFeature(const std::shared_ptr<SectorPathLossCalcParam> sectorParam, \
	LiShuttleAICalculator* pAICalculator) \
	: m_hashCode(0), m_sectorParam(sectorParam), m_pAICalculator(pAICalculator), \
	m_pSpotData(NULL), m_pSignalGrids(NULL), m_maxDistOfSignal(0)
{
	auto test = sizeof(SignalOnGrid);
	Init();
}

CellAIGridFeature::~CellAIGridFeature()
{
	if (m_pSpotData)
		delete m_pSpotData;

	if (m_pSignalGrids)
		upl_free(m_pSignalGrids);

	m_sectorParam.reset();
}

SectionSpotData* CellAIGridFeature::GetSpotFeature() const
{
	return m_pSpotData;
}

void CellAIGridFeature::Init()
{
	//初始化
	m_pSpotData = new RadialSectionSpotData(m_sectorParam->SectionParamArray.front(), false);
	m_lineNumber = m_pSpotData->GetLineNumber();
	m_spotNumberOnLine = m_pSpotData->GetSpotCountInLine();

	m_clutters.resize(20);
	for (auto clutter : m_clutters)
		clutter = 0;
	m_heights.resize(15);
	for (auto height : m_heights)
		height = 0;


	size_t count = m_lineNumber * m_spotNumberOnLine;
	m_pSignalGrids = (SignalOnGrid*)upl_malloc(sizeof(SignalOnGrid) * count) ;
	if (!m_pSignalGrids) {
		ULog_Write(ULOG_FATAL, "Memory overflow!");
		throw;
		return;
	}
	upl_bzero(m_pSignalGrids, sizeof(SignalOnGrid) * count);		//全部清零
}

const char* CellAIGridFeature::GetCellId() const
{
	if (!m_sectorParam || !m_sectorParam->Cell)
		return "";

	return m_sectorParam->Cell->cellID.c_str();
}

std::shared_ptr<SectorPathLossCalcParam> CellAIGridFeature::GetSectorParam() const
{
	return m_sectorParam;
}

SamplingGridFeature* CellAIGridFeature::FindGridFeature(double x, double y)
{
	if (!m_pSpotData)
		return NULL;

	return (SamplingGridFeature*)(m_pSpotData->GetSamplingSpotByXY(x, y));
}

void CellAIGridFeature::SetPathLoss(MeasuredData* mrData)
{
	if (!m_pSignalGrids || !m_pSpotData)
		return;

	int lineNo, spotNo;
	if (!m_pSpotData->GetIndex(mrData->geoX, mrData->geoY, lineNo, spotNo))
		return;

	double x, y;
	if (!m_pSpotData->GetGridCord(lineNo, spotNo, x, y))
		return;

	SignalOnGrid* pGridSignal = m_pSignalGrids + (lineNo * m_spotNumberOnLine + spotNo);
	double distance = sqrt((mrData->geoX - x) * (mrData->geoX - x) + (mrData->geoY - y) * (mrData->geoY - y));
	if (distance < EPSILON)
		distance = EPSILON;
	pGridSignal->pathloss += mrData->pathloss / distance;
	pGridSignal->weight += 1 / distance;
	m_MDTsum++;
	distance = spotNo * m_sectorParam->MainResolution;
	if (distance > m_maxDistOfSignal)
		m_maxDistOfSignal = distance;
}

void CellAIGridFeature::CalculateStatisticsFeature()
{
	//将内容序列化到streamFile文件中
	if (!m_pSpotData)
		return;

	//计算栅格特征
	if (!m_pAICalculator)
		return;
	auto sectionParam = m_sectorParam->SectionParamArray.front();
	double origEndDistance = sectionParam->CalcEndDistance;
	if (m_maxDistOfSignal < sectionParam->CalcEndDistance - sectionParam->CalcResolution)
	{
		//sectionParam->CalcEndDistance = m_maxDistOfSignal + sectionParam->CalcResolution;
		sectionParam->CalcEndDistance = m_maxDistOfSignal > 500 ? m_maxDistOfSignal : 500;
	}
	int realPointNubmerOnLine = sectionParam->GetPointNumberOnLine();
	sectionParam->CalcEndDistance = origEndDistance;
	//统计小区内地物及建筑物高度信息
	for (int i = 0; i < m_lineNumber; i++)
	{
		auto scanningLineInfo = m_pSpotData->GetScanningLine(i);
		int offset = i * m_spotNumberOnLine;
		for (int j = 0; j < realPointNubmerOnLine; j++)
		{
			auto spotInfo = (SamplingGridFeature*)(*scanningLineInfo)[j];
			if (spotInfo->TRDistance <= 500) {
				short clutterID = spotInfo->clutterID;
				short height = spotInfo->buildingHeight;
				CountHeightAndClutter(clutterID, height);
			}
		}
	}
}

#define TEXTBUF_SIZE 1024
static char g_cellAITextBuf[TEXTBUF_SIZE];
void CellAIGridFeature::StatFeatures()
{
	m_outputStatInfo.clear();

	//将内容序列化到streamFile文件中
	if (!m_pSpotData)
		return;

	//计算栅格特征
	if (!m_pAICalculator || 0 == m_maxDistOfSignal)
		return;
	auto sectionParam = m_sectorParam->SectionParamArray.front();
	if (m_maxDistOfSignal < sectionParam->CalcStartDistance)
		return;

	double origEndDistance = sectionParam->CalcEndDistance;
	if (m_maxDistOfSignal < sectionParam->CalcEndDistance - sectionParam->CalcResolution)
	{
		//sectionParam->CalcEndDistance = m_maxDistOfSignal + sectionParam->CalcResolution;
		sectionParam->CalcEndDistance = m_maxDistOfSignal > 500 ? m_maxDistOfSignal : 500;
	}
	int realPointNubmerOnLine = sectionParam->GetPointNumberOnLine();

	m_pSpotData->PrepareMem();		//预分配内存
	m_sectorParam->cellCategory = m_pAICalculator->CalculateSectionFeature((RadialSectionSpotData*)m_pSpotData, sectionParam);
	sectionParam->CalcEndDistance = origEndDistance;

	if (m_MDTsum == 0)
	{
		ULog_Write(ULOG_INFO, "cell:%s doesn't have MDTdata!", m_sectorParam->Cell->cellID.c_str());
		return;
	}

	m_outputStatInfo = "用户水平夹角,用户仰角,收发2D距离,收发高度差,\
			发射高度剖面特征1,发射高度剖面特征2,接收高度剖面特征1,接收高度剖面特征2,\
			收发连线最高峰,最高峰和接收点的距离,\
			发射场景剖面特征0,发射场景剖面特征1,发射场景剖面特征2,\
			接收场景剖面特征0,接收场景剖面特征1,接收场景剖面特征2,是否可视,路损\n";

	m_outputStatInfo.append("MsAzimuth, MsElevation, TRDistance, TRRelHeight, \
		TxHeightProfile1, TxHeightProfile2, RxHeightProfile1, RxHeightProfile2, \
		TRMaxEdgeHeight, EdgeDistanceToRx, \
		TxSceneCodeProfile0, TxSceneCodeProfile1, TxSceneCodeProfile2, \
		RxSceneCodeProfile0, RxSceneCodeProfile1, RxSceneCodeProfile2,LOS,pathloss\n");
	
	//小区统计特征计算
	//统计小区内地物及建筑物高度信息
	for (int i = 0; i < m_lineNumber; i++)
	{
		auto scanningLineInfo = m_pSpotData->GetScanningLine(i);
		int offset = i * m_spotNumberOnLine;
		for (int j = 0; j < realPointNubmerOnLine; j++)
		{
			auto spotInfo = (SamplingGridFeature*)(*scanningLineInfo)[j];
			if (spotInfo->TRDistance <= 500) {
				short clutterID = spotInfo->clutterID;
				short height = spotInfo->buildingHeight;
				CountHeightAndClutter(clutterID, height);  
			}
			if (0 == m_pSignalGrids[offset + j].weight)
				continue;
			if (!spotInfo->Outdoor)//室内点不要
				continue;
			m_sumData++;  
			if (spotInfo->MsAzimuth <= 900 || spotInfo->MsAzimuth >= 2700) {
				this->m_range90++;  
				if (spotInfo->MsAzimuth <= 600 || spotInfo->MsAzimuth >= 3000)
				{
					this->m_range60++;  
				}
			}
			float pathloss = m_pSignalGrids[offset + j].pathloss / m_pSignalGrids[offset + j].weight;
			spotInfo->ToString(g_cellAITextBuf, TEXTBUF_SIZE, pathloss);
			m_outputStatInfo.append(g_cellAITextBuf);
		}
	}
	m_pSpotData->RecycleMem();		//回收内存
}

void CellAIGridFeature::Serialize(StreamFile& streamFile) const
{
	streamFile.write(m_outputStatInfo.c_str(), m_outputStatInfo.length());
}

//void CellAIGridFeature::Serialize(StreamFile& streamFile) const
//{
//	//将内容序列化到streamFile文件中
//	if (!m_pSpotData)
//		return;
//
//	//计算栅格特征
//	if (!m_pAICalculator || 0 == m_maxDistOfSignal)
//		return;
//	auto sectionParam = m_sectorParam->SectionParamArray.front();
//	if (m_maxDistOfSignal < sectionParam->CalcStartDistance)
//		return;
//
//	double origEndDistance = sectionParam->CalcEndDistance;
//	if (m_maxDistOfSignal < sectionParam->CalcEndDistance - sectionParam->CalcResolution)
//	{
//		//sectionParam->CalcEndDistance = m_maxDistOfSignal + sectionParam->CalcResolution;
//		sectionParam->CalcEndDistance = m_maxDistOfSignal > 500 ? m_maxDistOfSignal : 500;
//	}
//	int realPointNubmerOnLine = sectionParam->GetPointNumberOnLine();
//
//	m_pSpotData->PrepareMem();		//预分配内存
//	m_sectorParam->cellCategory = m_pAICalculator->CalculateSectionFeature((RadialSectionSpotData*)m_pSpotData, sectionParam);
//	sectionParam->CalcEndDistance = origEndDistance;
//
//	if (m_MDTsum == 0)
//	{
//		ULog_Write(ULOG_INFO, "cell:%s doesn't have MDTdata!", m_sectorParam->Cell->cellID.c_str());
//		return;
//	}
//	//写标题行
//	snprintf(g_cellAITextBuf, TEXTBUF_SIZE, "用户水平夹角,用户仰角,收发2D距离,收发高度差,\
//			发射高度剖面特征1,发射高度剖面特征2,接收高度剖面特征1,接收高度剖面特征2,\
//			收发连线最高峰,最高峰和接收点的距离,\
//			发射场景剖面特征0,发射场景剖面特征1,发射场景剖面特征2,\
//			接收场景剖面特征0,接收场景剖面特征1,接收场景剖面特征2,是否可视,路损\n");
//	streamFile.write(g_cellAITextBuf, upl_strlen(g_cellAITextBuf));
//	snprintf(g_cellAITextBuf, TEXTBUF_SIZE, "MsAzimuth, MsElevation, TRDistance, TRRelHeight, \
//		TxHeightProfile1, TxHeightProfile2, RxHeightProfile1, RxHeightProfile2, \
//		TRMaxEdgeHeight, EdgeDistanceToRx, \
//		TxSceneCodeProfile0, TxSceneCodeProfile1, TxSceneCodeProfile2, \
//		RxSceneCodeProfile0, RxSceneCodeProfile1, RxSceneCodeProfile2,LOS,pathloss\n");
//	streamFile.write(g_cellAITextBuf, upl_strlen(g_cellAITextBuf));
//
//	//小区统计特征计算
//	//统计小区内地物及建筑物高度信息
//	for (int i = 0; i < m_lineNumber; i++)
//	{
//		auto scanningLineInfo = m_pSpotData->GetScanningLine(i);
//		int offset = i * m_spotNumberOnLine;
//		for (int j = 0; j < realPointNubmerOnLine; j++)
//		{
//			auto spotInfo = (SamplingGridFeature*)(*scanningLineInfo)[j];
//			if (spotInfo->TRDistance <= 500) {
//				short clutterID = spotInfo->clutterID;
//				short height = spotInfo->buildingHeight;
//				//CountHeightAndClutter(clutterID, height);  //为什么序列化函数还要进行统计？并写到成员变量里？如果序列化两次呢？不允许
//			}
//			if (0 == m_pSignalGrids[offset + j].weight)
//				continue;
//			if (!spotInfo->Outdoor)//室内点不要
//				continue;
//			//m_sumData++;  //为什么序列化函数还要进行统计？并写到成员变量里？如果序列化两次呢？不允许
//			if (spotInfo->MsAzimuth <= 900 || spotInfo->MsAzimuth >= 2700) {
//				//this->m_range90++;   //为什么序列化函数还要进行统计？并写到成员变量里？如果序列化两次呢？不允许
//				if (spotInfo->MsAzimuth <= 600 || spotInfo->MsAzimuth >= 3000)
//				{
//					//this->m_range60++;  //为什么序列化函数还要进行统计？并写到成员变量里？如果序列化两次呢？不允许
//				}
//			}
//			float pathloss = m_pSignalGrids[offset + j].pathloss / m_pSignalGrids[offset + j].weight;
//			spotInfo->ToString(g_cellAITextBuf, TEXTBUF_SIZE, pathloss);
//			streamFile.write(g_cellAITextBuf, upl_strlen(g_cellAITextBuf));
//		}
//	}
//	m_pSpotData->RecycleMem();		//回收内存
//}

void CellAIGridFeature::Deserialize(StreamFile& streamFile)
{
	ULog_Write(ULOG_ERROR, "Not implement!");
}

size_t CellAIGridFeature::GetHash() const
{
	return m_hashCode;
}

//void CellAIGridFeature::RangeCount()
//{
//	if (!m_pSpotData)
//		return;
//	size_t lineNumber = m_pSpotData->GetLineNumber();
//	for (int i = 0; i < lineNumber; i++)
//	{
//		auto scanningLineInfo = m_pSpotData->GetScanningLine(i);
//		int startIndex = scanningLineInfo->GetStartIndex();
//		int endIndex = scanningLineInfo->GetEndIndex();
//		for (int j = 1; j <= endIndex; j++)
//		{
//			if (j < startIndex)
//			{
//				continue;
//			}
//			auto spotInfo = (SamplingGridFeature*)(*scanningLineInfo)[j - startIndex];
//			if (spotInfo->pathloss == 0) {
//				continue;
//			}
//			this->m_sumData++;
//			if (spotInfo->MsAzimuth <= 900 || spotInfo->MsAzimuth >= 2700) {
//				this->m_range90++;
//				if (spotInfo->MsAzimuth <= 600 || spotInfo->MsAzimuth >= 3000)
//				{
//					this->m_range60++;
//				}
//			}
//
//		}
//	}
//}
