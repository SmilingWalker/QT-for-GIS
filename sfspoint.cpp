#include "sfspoint.h"

SfsPoint::SfsPoint()
{
    setName("Point");
}

bool SfsPoint::operator=(const SfsPoint * pt)
{
    if(fabs(this->x-pt->x)<esp&&fabs(this->y-pt->y)<esp)
        return true;
    else
        return false;

}

int SfsPoint::GeometryType()
{
    return Sfs_Point;
}

int SfsPoint::Dimension()
{
    return 0;
}
