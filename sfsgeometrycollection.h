#ifndef SFSGEOMETRYCOLLECTION_H
#define SFSGEOMETRYCOLLECTION_H
#include<sfsgeometry.h>
#include<qvector.h>
class SfsGeometryCollection : public SfsGeometry
{
public:
    SfsGeometryCollection();
    int numGeometries();
    SfsGeometry * geometryN(int n);
public:
    QVector<SfsGeometry *> *geometries;

    int GeometryType();
    int Dimension();
};

#endif // SFSGEOMETRYCOLLECTION_H
