#ifndef SFSMULTIPOINT_H
#define SFSMULTIPOINT_H
#include<sfsgeometry.h>
#include<qvector.h>
#include<sfspoint.h>

class SfsMultiPoint : public SfsGeometry
{
public:
    SfsMultiPoint();
public:
    QVector<SfsPoint * > *pts;
    int GeometryType();
    int Dimension();
};

#endif // SFSMULTIPOINT_H
