#pragma once

#include "SamplingSpot.h"

typedef struct tagSignalOnGrid {
	float weight = 0;//Ȩ���ۼ�
	float pathloss = 0;//Ȩ�غ�·��˻����ۼ�
}SignalOnGrid;

//#define SAMPLINGSPOT_COUNT 20
//#define HEIGHTPROFILE_STRLEN 40
typedef struct tagSamplingGridFeature : SamplingSpot
{
	short MsAzimuth;		//��������ϵ�µ�ˮƽ�ǣ�ԭֵ����10
	short MsElevation;		//��������ϵ�µ�����,ԭֵ����10
	int	  TRDistance;		//�շ�2D����
	short TRRelHeight;		//�������߸߶� - �������߸߶ȣ���λ��
	short TxHeightProfile1;	//�������߸߶� - ����˵�1��������ĸ߶�
	short TxHeightProfile2; //�������߸߶� - ����˵�2��������ĸ߶�
	short RxHeightProfile1; //�������߸߶� - ���ն˵�����1�������㣨���յ�֮ǰ���ĸ߶�
	short RxHeightProfile2; //�������߸߶� - ���ն˵�����2��������ĸ߶�
	//char  HeightProfile[HEIGHTPROFILE_STRLEN];	//����߶�����
	short TxSceneCode;		//�����ĳ�������
	short TxSceneCodeProfile1;	//����˵�1��������ĳ�������
	short TxSceneCodeProfile2;	//����˵�2��������ĳ�������
	short RxSceneCodeProfile1;	//���ն˵�����1��������ĳ�������
	short RxSceneCodeProfile2;	//���ն˵�����2��������ĳ�������
	short TRMaxEdgeHeight;		//�������߸߶� - �շ�������߷�
	short EdgeDistanceToRx;		//��߷�ͽ��յ�ľ���
	char  Los;					//0:NLOS,1:LOS
	bool  Outdoor;				//�Ƿ�������
	double DeepLengthIndoor;	//���ڴ�͸���

	tagSamplingGridFeature(GeoXYPoint cord) : SamplingSpot(cord) {};
	virtual ~tagSamplingGridFeature() {};
	
	static tagSamplingGridFeature* CreateInstance(GeoXYPoint cord) { 
		return new tagSamplingGridFeature(cord); 
	};

	virtual void Destroy() { delete this; };

	static void ToChnTitle(char* outStr, size_t bufSize) {
		/*snprintf(outStr, bufSize, "�û�ˮƽ�н�,�û�����,�շ�2D����,�շ��߶Ȳ�,\
			����߶���������1,����߶���������2,���ո߶���������1,���ո߶���������2,\
			�շ�������߷�,��߷�ͽ��յ�ľ���,\
			���䳡����������0,���䳡����������1,���䳡����������2,\
			���ճ�����������0,���ճ�����������1,���ճ�����������2,�Ƿ����,����߶�,·��\n");*/
		snprintf(outStr, bufSize, "�û�ˮƽ�н�,�û�����,�շ�2D����,�շ��߶Ȳ�,\
			����߶���������1,����߶���������2,���ո߶���������1,���ո߶���������2,\
			�շ�������߷�,��߷�ͽ��յ�ľ���,\
			���䳡����������0,���䳡����������1,���䳡����������2,\
			���ճ�����������0,���ճ�����������1,���ճ�����������2,�Ƿ����,·��\n");
	}

	static void ToEngTitle(char* outStr, size_t bufSize) {
		/*snprintf(outStr, bufSize, "MsAzimuth, MsElevation, TRDistance, TRRelHeight, \
		TxHeightProfile1, TxHeightProfile2, RxHeightProfile1, RxHeightProfile2, \
		TRMaxEdgeHeight, EdgeDistanceToRx, \
		TxSceneCodeProfile0, TxSceneCodeProfile1, TxSceneCodeProfile2, \
		RxSceneCodeProfile0, RxSceneCodeProfile1, RxSceneCodeProfile2,LOS,HeightProfile,pathloss\n");*/
		snprintf(outStr, bufSize, "MsAzimuth, MsElevation, TRDistance, TRRelHeight, \
		TxHeightProfile1, TxHeightProfile2, RxHeightProfile1, RxHeightProfile2, \
		TRMaxEdgeHeight, EdgeDistanceToRx, \
		TxSceneCodeProfile0, TxSceneCodeProfile1, TxSceneCodeProfile2, \
		RxSceneCodeProfile0, RxSceneCodeProfile1, RxSceneCodeProfile2,LOS,pathloss\n");
	}

	void ToString(char* outStr, size_t bufSize, int pl = 0) {
		/*snprintf(outStr, bufSize, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%.2f\n", \
			this->MsAzimuth, this->MsElevation, this->TRDistance, this->TRRelHeight, \
			this->TxHeightProfile1, this->TxHeightProfile2, \
			this->RxHeightProfile1, this->RxHeightProfile2, \
			this->TRMaxEdgeHeight, this->EdgeDistanceToRx, \
			this->TxSceneCode, this->TxSceneCodeProfile1, this->TxSceneCodeProfile2, \
			this->clutterID, this->RxSceneCodeProfile1, this->RxSceneCodeProfile2, \
			this->Los, this->HeightProfile, pl);*/
		snprintf(outStr, bufSize, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", \
			this->MsAzimuth, this->MsElevation, this->TRDistance, this->TRRelHeight, \
			this->TxHeightProfile1, this->TxHeightProfile2, \
			this->RxHeightProfile1, this->RxHeightProfile2, \
			this->TRMaxEdgeHeight, this->EdgeDistanceToRx, \
			this->TxSceneCode, this->TxSceneCodeProfile1, this->TxSceneCodeProfile2, \
			this->clutterID, this->RxSceneCodeProfile1, this->RxSceneCodeProfile2, \
			this->Los, pl);
	};

}SamplingGridFeature;