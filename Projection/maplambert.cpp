#include "maplambert.h"

MapLambert::MapLambert()
{
    L0 = 105.0*Pi/180;
    B1 = 25.0/180.0*Pi;
    B2 = 47.0/180*Pi;
    a = 6378137;//2000国家大地坐标系参数
    e = 298.257222101;
    B0 = 0*Pi/180;
}

MapLambert::~MapLambert()
{

}

void MapLambert::getXY(float L, float B, float *X, float *Y)
{
    L = L*Pi/180;
    B = B*Pi/180;
     double Ef = 609600;
    float e1 = 1*exp(1.0);
    float mb1 = cos(B1)/sqrt(1-1/pow(e,2)*pow(sin(B1),2));
    float mb2 = cos(B2)/sqrt(1-1/pow(e,2)*pow(sin(B2),2));
    float t = tan(Pi/4-B/2)/pow(((1-sin(B)/e)/(1+sin(B)/e)),1/(2*e));
    float t0 = tan(Pi/4-B0/2)/pow(((1-sin(B0)/e)/(1+sin(B0)/e)),1/(2*e));
    float tb1 = tan(Pi/4-B1/2)/pow(((1-sin(B1)/e)/(1+sin(B1)/e)),1/(2*e));
    float tb2 = tan(Pi/4-B2/2)/pow(((1-sin(B2)/e)/(1+sin(B2)/e)),1/(2*e));
    float n = log10f(mb1/mb2)/log10f(tb1/tb2);
    float F = mb1/(pow(tb1,n)*n);
    float r0 = a*F*pow(t0,n);
    float r = a*F*pow(t,n);
    float theta = n*(L-L0);
    (*X) = r0 - r*cos(theta);
    (*Y) = r*sin(theta)+Ef;
}

void MapLambert::getLB(float x, float y, float *L, float *B)
{

}

void MapLambert::setGraticule(int NS, int NN, int EW, int EE)
{
    this->NS = NS;
    this->NN=NN;
    this->EW=EW;
    this->EE=EE;
}
