#ifndef SFSPOINT_H
#define SFSPOINT_H
#include<SFS/sfsgeometry.h>
#define  esp 1e-12
#include<math.h>
class SfsPoint : public SfsGeometry
{
public:

    SfsPoint();
    bool operator=(const SfsPoint* pt);
public:
    double x;//x的坐标
    double y;//y的坐标
    GeoType GeometryType();
    int Dimension();
};

#endif // SFSPOINT_H
