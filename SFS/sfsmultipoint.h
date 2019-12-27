#ifndef SFSMULTIPOINT_H
#define SFSMULTIPOINT_H
#include<SFS/sfsgeometry.h>
#include<qvector.h>
#include<SFS/sfspoint.h>

class SfsMultiPoint : public SfsGeometry
{
public:
    SfsMultiPoint();
public:
    QVector<SfsPoint * > *pts;
    GeoType GeometryType();
    int Dimension();
};

#endif // SFSMULTIPOINT_H
