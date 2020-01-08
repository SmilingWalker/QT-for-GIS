#ifndef MAPPROJECT_H
#define MAPPROJECT_H
#include<cmath>

class MapProject
{
public:
    MapProject();
    ~MapProject(void);
public:
    float L0,L1,B0,B1;//投影区域坐标范围
    float a;//长半轴
    float e;//扁率

    virtual void getXY(float L, float B, float* X, float* Y);
    virtual void getLB(float x, float y, float* L, float* B);
    int NS,NN,EW,EE;//用于经纬网的绘制
    virtual void setGraticule(int NS,int NN,int EW,int EE)=0;
};

#endif // MAPPROJECT_H
