#ifndef SFSPOINT_H
#define SFSPOINT_H
#include"sfsgeometry.h"
#include"sfslayer.h"
#define  esp 1e-12
#include<math.h>
class SfsPoint : public SfsGeometry
{
public:

    SfsPoint();
    SfsPoint(double x, double y);
    ~SfsPoint();
    bool operator==(const SfsPoint* pt);
    bool operator=(const SfsPoint);
    virtual void withinDist(SfsLayer *layer, double dist, QVector<SfsGeometry *> *geometries) override;

public:
    double x;//x的坐标
    double y;//y的坐标
    GeoType GeometryType() override;
    int Dimension();
};

#endif // SFSPOINT_H
