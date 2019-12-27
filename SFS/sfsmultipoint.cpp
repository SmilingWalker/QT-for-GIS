#include "sfsmultipoint.h"

SfsMultiPoint::SfsMultiPoint()
{
    pts = new QVector<SfsPoint*>;
}

GeoType SfsMultiPoint::GeometryType()
{
    return Sfs_MultiPoint;
}

int SfsMultiPoint::Dimension()
{
    return 0;
}
