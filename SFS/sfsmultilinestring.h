#ifndef SFSMULTILINESTRING_H
#define SFSMULTILINESTRING_H
#include<SFS/sfsgeometry.h>
#include<SFS/sfsgeometrycollection.h>
#include<qvector.h>
#include<SFS/sfslinestring.h>
class SfsMultiLineString : public SfsGeometry
{
public:
    SfsMultiLineString();
    int numLineStrings();
    SfsLineString* LineStringN(int n);
public:
    QVector<SfsLineString *> *lineStrings;

    GeoType GeometryType();
    int Dimension();
};

#endif // SFSMULTILINESTRING_H
