#ifndef MAPLAMBERT_H
#define MAPLAMBERT_H


#include"mapproject.h"
#define Pi 3.1415926535
class MapLambert:public MapProject
{
public:
    MapLambert();
    ~MapLambert();
    void getXY(float L, float B, float* X, float* Y);
    void getLB(float x, float y, float* L, float* B);

    float B0;//原点纬线
    float L0;//中央经线
    float B1;//第一标准纬线
    float B2;//第二标准纬线

    float a;//长半轴
    float e;//扁率

    int NS,NN,EW,EE;//用于经纬网的绘制
    void setGraticule(int NS,int NN,int EW,int EE);//传入经纬网的范围
};

#endif // MAPLAMBERT_H
