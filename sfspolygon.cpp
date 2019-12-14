#include "sfspolygon.h"

SfsPolygon::SfsPolygon()
{
    boundaries = new QVector<SfsLineString*>;
    setName("Polygon");
}

SfsLineString *SfsPolygon::exterorRing()
{
    return boundaries->value(0);
}

int SfsPolygon::numInterior()
{
    return boundaries->size()-1;
}

SfsLineString *SfsPolygon::interioRingrN(int n)
{
    if(n<(numInterior()-1))
        return boundaries->value(n-1);
    else return nullptr;
}

int SfsPolygon::GeometryType()
{
    return Sfs_Polygon;
}

int SfsPolygon::Dimension()
{
    return 2;
}
