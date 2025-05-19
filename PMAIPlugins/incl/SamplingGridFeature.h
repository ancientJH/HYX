#pragma once

#include "SamplingSpot.h"

typedef struct tagSignalOnGrid {
	float weight = 0;//权重累加
	float pathloss = 0;//权重和路损乘积的累加
}SignalOnGrid;

//#define SAMPLINGSPOT_COUNT 20
//#define HEIGHTPROFILE_STRLEN 40
typedef struct tagSamplingGridFeature : SamplingSpot
{
	short MsAzimuth;		//天线坐标系下的水平角，原值乘以10
	short MsElevation;		//天线坐标系下的仰角,原值乘以10
	int	  TRDistance;		//收发2D距离
	short TRRelHeight;		//发射天线高度 - 接收天线高度，单位米
	short TxHeightProfile1;	//发射天线高度 - 发射端第1个采样点的高度
	short TxHeightProfile2; //发射天线高度 - 发射端第2个采样点的高度
	short RxHeightProfile1; //发射天线高度 - 接收端倒数第1个采样点（接收点之前）的高度
	short RxHeightProfile2; //发射天线高度 - 接收端倒数第2个采样点的高度
	//char  HeightProfile[HEIGHTPROFILE_STRLEN];	//剖面高度特征
	short TxSceneCode;		//发射点的场景代码
	short TxSceneCodeProfile1;	//发射端第1个采样点的场景代码
	short TxSceneCodeProfile2;	//发射端第2个采样点的场景代码
	short RxSceneCodeProfile1;	//接收端倒数第1个采样点的场景代码
	short RxSceneCodeProfile2;	//接收端倒数第2个采样点的场景代码
	short TRMaxEdgeHeight;		//发射天线高度 - 收发连线最高峰
	short EdgeDistanceToRx;		//最高峰和接收点的距离
	char  Los;					//0:NLOS,1:LOS
	bool  Outdoor;				//是否在室外
	double DeepLengthIndoor;	//室内穿透深度

	tagSamplingGridFeature(GeoXYPoint cord) : SamplingSpot(cord) {};
	virtual ~tagSamplingGridFeature() {};
	
	static tagSamplingGridFeature* CreateInstance(GeoXYPoint cord) { 
		return new tagSamplingGridFeature(cord); 
	};

	virtual void Destroy() { delete this; };

	static void ToChnTitle(char* outStr, size_t bufSize) {
		/*snprintf(outStr, bufSize, "用户水平夹角,用户仰角,收发2D距离,收发高度差,\
			发射高度剖面特征1,发射高度剖面特征2,接收高度剖面特征1,接收高度剖面特征2,\
			收发连线最高峰,最高峰和接收点的距离,\
			发射场景剖面特征0,发射场景剖面特征1,发射场景剖面特征2,\
			接收场景剖面特征0,接收场景剖面特征1,接收场景剖面特征2,是否可视,剖面高度,路损\n");*/
		snprintf(outStr, bufSize, "用户水平夹角,用户仰角,收发2D距离,收发高度差,\
			发射高度剖面特征1,发射高度剖面特征2,接收高度剖面特征1,接收高度剖面特征2,\
			收发连线最高峰,最高峰和接收点的距离,\
			发射场景剖面特征0,发射场景剖面特征1,发射场景剖面特征2,\
			接收场景剖面特征0,接收场景剖面特征1,接收场景剖面特征2,是否可视,路损\n");
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