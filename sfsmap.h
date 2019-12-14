#ifndef SFSMAP_H
#define SFSMAP_H
#include<sfslayer.h>
#include<qvector.h>

class SfsMap
{
public:
    SfsMap();
public:
    QVector<SfsLayer *> *layers;
    BoundaryBox *bbox;

};

#endif // SFSMAP_H
