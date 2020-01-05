#ifndef KERNELDESITY_H
#define KERNELDESITY_H

#define PI 3.14159

#include <SFS/sfspoint.h>
#include "SpatialCalculation.h"
#include <Index/spatialindex.h>
#include <Index/grid.h>
#include"spatialstatistics.h"

class KernelDensity : public SpatialStatistics
{
public:
    KernelDensity();
    //目前只有用带参数方式初始化核密度才能正常使用，因为setColumn\ setRow没有真正定义。
    //也就是说，目前，kerneldensity的图层属性和栅格属性在初始化时就被固定了。
    KernelDensity(SfsLayer* layer, double size, int index);
    ~KernelDensity();
    //计算，默认带宽
    double defaultBandWidth();
    //尚未定义，目前必须由用户指定
    double defaultPixelSize();
    //计算输出。目前dist是默认带宽
    void compute(double dist);
    //存储到指定路径
    void save(QString path);
    int getColumn() const;
    void setColumn(int value);

    int getRow() const;
    void setRow(int value);

    double getPixelSize() const;
    void setPixelSize(double value);

    SfsLayer *getLayer() const;
    void setLayer(SfsLayer *value);

    //可以更改用于计算的图层属性下标
    int getIndex() const;
    void setIndex(int value);
    //获取density最大、最小值
    double getDensityMax();
    double getDensityMin();

    //栅格，以SfsPoint的形式存储
    SfsPoint** pixels;
private:
    //输出栅格的行列数
    int column;
    int row;
    //输出栅格的大小
    double size;
    //属性下标
    int index;
    //density最大值
    double densityMax;
    //density 最小值
    double densityMin;
//    //栅格，以SfsPoint的形式存储
//    SfsPoint** pixels;
    //用于分析的图层
    SfsLayer* layer;
};

#endif // KERNELDESITY_H
