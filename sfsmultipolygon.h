#ifndef SFSMULTIPOLYGON_H
#define SFSMULTIPOLYGON_H
#include<sfsgeometry.h>
#include<sfspolygon.h>
#include<qvector.h>
class SfsMultiPolygon : public SfsGeometry
{
public:
    SfsMultiPolygon();
    int numPolygons();
    SfsPolygon * polygonN(int n);
public:
    QVector<SfsPolygon*> * polygons;

    int GeometryType();
    int Dimension();
};

#endif // SFSMULTIPOLYGON_H
