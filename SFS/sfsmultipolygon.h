#ifndef SFSMULTIPOLYGON_H
#define SFSMULTIPOLYGON_H
#include<SFS/sfsgeometry.h>
#include<SFS/sfspolygon.h>
#include<qvector.h>
class SfsMultiPolygon : public SfsGeometry
{
public:
    SfsMultiPolygon();
    int numPolygons();
    SfsPolygon * polygonN(int n);
public:
    QVector<SfsPolygon*> * polygons;

    GeoType GeometryType();
    int Dimension();
};

#endif // SFSMULTIPOLYGON_H
