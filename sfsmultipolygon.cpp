#include "sfsmultipolygon.h"

SfsMultiPolygon::SfsMultiPolygon()
{
    polygons = new QVector<SfsPolygon*>;
}

int SfsMultiPolygon::numPolygons()
{
    return  polygons->size();
}
SfsPolygon* SfsMultiPolygon::polygonN(int n)
{
    if(n<(numPolygons()-1))
        return  polygons->value(n-1);
    else return nullptr;
}

int SfsMultiPolygon::GeometryType()
{
    return Sfs_MultiPolygon;
}

int SfsMultiPolygon::Dimension()
{
    return 2;
}
