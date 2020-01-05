#include "SFS/sfslinestring.h"

SfsLineString::SfsLineString()
{
    pts = new QVector<SfsPoint*>();
}

int SfsLineString::numPoints()
{
    return pts->size();
}

SfsPoint* SfsLineString::PointN(int n)
{
    if(n<=numPoints()-1)
        return pts->value(n);
    else {
        return nullptr;
    }
}

SfsPoint* SfsLineString::StartPoint()
{
    return pts->value(0);
}

SfsPoint* SfsLineString::EndPoint()
{
    return  pts->value(numPoints()-1);
}

bool SfsLineString::IsClosed()
{
    if(StartPoint()==EndPoint())
        return true;
    else
        return false;
}
GeoType SfsLineString::GeometryType()
{
    return Sfs_LineString;
}
int SfsLineString::Dimension()
{
    return 1;
}
