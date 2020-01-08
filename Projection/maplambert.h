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

    float B0;//ԭ��γ��
    float L0;//���뾭��
    float B1;//��һ��׼γ��
    float B2;//�ڶ���׼γ��

    float a;//������
    float e;//����

    int NS,NN,EW,EE;//���ھ�γ���Ļ���
    void setGraticule(int NS,int NN,int EW,int EE);//���뾭γ���ķ�Χ
};

#endif // MAPLAMBERT_H
