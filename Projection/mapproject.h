#ifndef MAPPROJECT_H
#define MAPPROJECT_H
#include<cmath>

class MapProject
{
public:
    MapProject();
    ~MapProject(void);
public:
    float L0,L1,B0,B1;//ͶӰ�������귶Χ
    float a;//������
    float e;//����

    virtual void getXY(float L, float B, float* X, float* Y);
    virtual void getLB(float x, float y, float* L, float* B);
    int NS,NN,EW,EE;//���ھ�γ���Ļ���
    virtual void setGraticule(int NS,int NN,int EW,int EE)=0;
};

#endif // MAPPROJECT_H
