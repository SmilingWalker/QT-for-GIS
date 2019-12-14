#ifndef SFSPOLYGON_H
#define SFSPOLYGON_H
#include<sfsgeometry.h>
#include<sfslinestring.h>
#include<qvector.h>
class SfsPolygon : public SfsGeometry
{
public:
    SfsPolygon();
    SfsLineString* exterorRing();
    int  numInterior();
    SfsLineString* interioRingrN(int n);
public:
    QVector<SfsLineString*> *boundaries;
    int GeometryType();
    int Dimension();
};

#endif // SFSPOLYGON_H
