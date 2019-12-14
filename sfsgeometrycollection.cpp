#include "sfsgeometrycollection.h"

SfsGeometryCollection::SfsGeometryCollection()
{
    geometries = new QVector<SfsGeometry*>;
}

int SfsGeometryCollection::numGeometries()
{
    return geometries->size();
}

SfsGeometry *SfsGeometryCollection::geometryN(int n)
{
    if(n<numGeometries()-1){
        return geometries->value(n-1);
    }
    else return nullptr;
}
int SfsGeometryCollection::GeometryType()
{
    return Sfs_GeometryCollection;
}
int SfsGeometryCollection::Dimension()
{
    return 2;
}
