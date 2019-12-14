#ifndef SFSRENDER_H
#define SFSRENDER_H
#include<qpainter.h>
#include<sfsmap.h>

class SfsRender
{
public:
    SfsRender(SfsMap * map);
    void BeginRender(QPainter *painter);
public:
    SfsMap * map;
};

#endif // SFSRENDER_H
