#include "AIGridFeatureProbe.h"
#include "UPlanCommonExport.h"
#include "PropagationFunc.h"


AIGridFeatureProbe::AIGridFeatureProbe(LiShuttleAICalculator* pAICalculator) : m_pAICalculator(pAICalculator)
{
}

AIGridFeatureProbe::~AIGridFeatureProbe()
{
	Clear();
}

void AIGridFeatureProbe::Clear()
{
	for (auto featurePair : m_cellAIGridFeatures) {
		if (featurePair.second)
			delete featurePair.second;
	}
	m_cellAIGridFeatures.clear();
	m_sectorParams.clear();
}

void AIGridFeatureProbe::PostProcess()
{
}

void AIGridFeatureProbe::call(MeasuredData* mrData)
{
	if (!mrData)
		return;
	int length = m_sectorParams.size();
	/*��С��������ǵ�һ�Σ��������µ�С����������*/
	CellAIGridFeature* pCellFeature = NULL;
	auto iter = m_cellAIGridFeatures.find(mrData->transceiver->transID);
	if (m_cellAIGridFeatures.end() == iter) //��һ�μ��ظ�С����·������
	{
		auto iter1 = m_sectorParams.find(mrData->transceiver->transID);
		if (m_sectorParams.end() == iter1)
			return;

		pCellFeature = new CellAIGridFeature(iter1->second, m_pAICalculator);
		m_cellAIGridFeatures.insert(std::pair<std::string, CellAIGridFeature*>(mrData->transceiver->transID, pCellFeature));
	}
	else
	{
		pCellFeature = iter->second;
	}

	/*ofstream ofile;
	std::string filename = "/home/zhangjiarui/.vs/MDTdata/" + mrData->transceiver->transID + ".csv";
	ofile.open(filename, ios::app | ios::out);
	
	char test[256];
	snprintf(test, 255, "%.4f,%.4f,%.4f\n", \
		mrData->geoX, mrData->geoY, mrData->pathloss);
	ofile << test;
	ofile.close();*/


	//����·����·��ֵ��Ȩƽ����դ�����
	if (pCellFeature)
		pCellFeature->SetPathLoss(mrData);
}

void AIGridFeatureProbe::SetCellParams(std::shared_ptr<AntPathLossCalcParam> antCalcParam)
{
	if (!antCalcParam)
		return;

	for (auto sectorParam : antCalcParam->SectorCalcParams) {
		m_sectorParams.insert(\
			std::pair<std::string, std::shared_ptr<SectorPathLossCalcParam>>(sectorParam->Cell->cellID, sectorParam) );
	}
}

#define TEXTBUF_SIZE 1024
static char g_cellAIFeature[TEXTBUF_SIZE];
void AIGridFeatureProbe::Serialize(StreamFile& streamFile) const
{
	//С���������л�
	if (m_sectorParams.empty())
		return;
	//д������
	snprintf(g_cellAIFeature, TEXTBUF_SIZE, "cellID,horz3dBBeamWidth,vert3dBBeamWidth,\
			cellCategory,antMount,antMaxGain,antTxNumber,centerFrep,\
			antHeightToGround,MDTsum,sumCount,range60, range90,maxDistance,clutters,heights\n");
	streamFile.write(g_cellAIFeature, upl_strlen(g_cellAIFeature));
	for (auto cellFeature : m_cellAIGridFeatures) {
		if (cellFeature.second->GetMDTsum() == 0) continue;
		cellFeature.second->GetCellFeature(g_cellAIFeature, TEXTBUF_SIZE);
		streamFile.write(g_cellAIFeature, upl_strlen(g_cellAIFeature));
	}
}

void AIGridFeatureProbe::Deserialize(StreamFile& streamFile)
{
	ULog_Write(ULOG_ERROR, "Not implement!");
}

size_t AIGridFeatureProbe::GetHash() const
{
	return 0;
}

