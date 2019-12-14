#ifndef SFSMULTILINESTRING_H
#define SFSMULTILINESTRING_H
#include<sfsgeometry.h>
#include<sfsgeometrycollection.h>
#include<qvector.h>
#include<sfslinestring.h>
class SfsMultiLineString : public SfsGeometry
{
public:
    SfsMultiLineString();
    int numLineStrings();
    SfsLineString* LineStringN(int n);
public:
    QVector<SfsLineString *> *lineStrings;

    int GeometryType();
    int Dimension();
};

#endif // SFSMULTILINESTRING_H
