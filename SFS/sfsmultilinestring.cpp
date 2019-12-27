#include "SFS/sfsmultilinestring.h"

SfsMultiLineString::SfsMultiLineString()
{
    lineStrings = new QVector<SfsLineString*>;
}

int SfsMultiLineString::numLineStrings()
{
    return lineStrings->size();
}

SfsLineString *SfsMultiLineString::LineStringN(int n)
{
    if(n<(numLineStrings()-1))
        return lineStrings->value(n);
    else {
        return nullptr;
    }
}

GeoType SfsMultiLineString::GeometryType()
{
    return Sfs_MultiLineString;
}
int SfsMultiLineString::Dimension()
{
    return 1;
}
