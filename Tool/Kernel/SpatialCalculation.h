#ifndef SPATIALCALCULATION_H
#define SPATIALCALCULATION_H

#include<SFS/sfsgeometry.h>
#include <SFS/sfslinestring.h>
#include <SFS/sfspoint.h>
#include <SFS/sfspolygon.h>
#include <SFS/sfslayer.h>

double distance(SfsGeometry* obj1, SfsGeometry* obj2);
//求图层（加权）中心点。权值是图层地物第index个属性的值。
SfsPoint meanCenter(SfsLayer* layer);
SfsPoint meanCenter(SfsLayer* layer, int index);
//计算各地物到（加权)图层中心点的距离的中值。
double midDistToCtr(SfsLayer* layer, SfsPoint* center);
//计算SD
double stdDist(SfsLayer* layer, SfsPoint* center);
double stdDist(SfsLayer* layer, SfsPoint* center, int index);
void quick_sort(double s[], int l, int r);

#endif // SPATIALCALCULATION_H
