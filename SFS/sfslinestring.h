#ifndef SFSLINESTRING_H
#define SFSLINESTRING_H
#include<qvector.h>
#include<SFS/sfsgeometry.h>
#include<SFS/sfspoint.h>

class SfsLineString : public SfsGeometry
{
public:
    SfsLineString();
    int numPoints();
    SfsPoint* PointN(int n);
    SfsPoint* StartPoint();
    SfsPoint* EndPoint();
    bool IsClosed();
public:
    QVector<SfsPoint *> *pts;
    GeoType GeometryType();
    int Dimension();
};

#endif // SFSLINESTRING_H
