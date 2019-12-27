#ifndef SFSPOLYGON_H
#define SFSPOLYGON_H
#include<SFS/sfsgeometry.h>
#include<SFS/sfslinestring.h>
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
    GeoType GeometryType();
    int Dimension();
};

#endif // SFSPOLYGON_H
