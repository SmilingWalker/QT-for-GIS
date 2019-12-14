#ifndef SFSLINESTRING_H
#define SFSLINESTRING_H
#include<qvector.h>
#include<sfsgeometry.h>
#include<sfspoint.h>

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
    int GeometryType();
    int Dimension();
};

#endif // SFSLINESTRING_H
