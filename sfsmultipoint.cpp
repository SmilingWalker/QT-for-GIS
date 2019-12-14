#include "sfsmultipoint.h"

SfsMultiPoint::SfsMultiPoint()
{
    pts = new QVector<SfsPoint*>;
}

int SfsMultiPoint::GeometryType()
{
    return Sfs_MultiPoint;
}

int SfsMultiPoint::Dimension()
{
    return 0;
}
