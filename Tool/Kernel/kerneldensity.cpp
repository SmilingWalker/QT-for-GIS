#include "kerneldensity.h"
#include <SFS/sfslayer.h>
#include "cmath"
#include "SpatialCalculation.h"
#include "properties.h"
#include "QDebug"
#include "cmath"
#include<stdlib.h>
#include<QTime>
#include <QElapsedTimer>

KernelDensity::KernelDensity()
{
    layer = nullptr;
    pixels = nullptr;
    densityMax = -1;
    densityMin = -1;

}

KernelDensity::KernelDensity(SfsLayer *layer, double size, int index)
{
    //size是输出栅格的分辨率
    this->layer = layer;
    this->size = size;
    if(layer->geometries->at(0)->getPropertyTypeAt(index) != Double_PRO){
        qDebug()<<"property type is not double!"<<endl;
        return;
    }
    this->index = index;

    BoundaryBox* bbox = layer->bbox;
    double top = bbox->getTopY();
    double left = bbox->getLeftX();
    double width = bbox->getRightX() - bbox->getLeftX();
    double height = bbox->getTopY() - bbox->getBottomY();

    //用栅格分辨率size计算栅格行列数
    //不能整除，则加一行/列
    double widthCeil = ceil(width);
    this->column = width/size + ((widthCeil - width) > 0.000001)*1;
    double heightCeil = ceil(height);
    this->row = height/size + ((heightCeil - height )> 0.000001)*1;

    pixels = new SfsPoint*[row];
    for(int i = 0; i < row; i++){
        pixels[i] = new SfsPoint[column];
        for(int j = 0; j < column; j++){
            pixels[i][j].x = left + (j + 0.5)*size;
            pixels[i][j].y = top - (i + 0.5)*size;
        }
    }
}

KernelDensity::~KernelDensity()
{

    if(pixels != nullptr){
        for(int i = 0; i < row; i++){
            delete [] pixels[i];
        }
    }
    layer = nullptr;
}

//按照arcgis官网方法计算默认带宽
double KernelDensity::defaultBandWidth()
{
    SfsPoint center = meanCenter(layer,index);
    double SD = stdDist(layer, &center, index);
    double midDist = midDistToCtr(layer, &center);
    midDist = pow(1/log(2), 0.5)*midDist;

    double dist = midDist < SD ? midDist: SD;
    double weightSum = layer->getWeightSumAt(index);

    return 0.9 * dist * pow(weightSum, -0.2);
}

void KernelDensity::compute(double radius)
{

    double sumDensity = 0;
    double sumWeight = 0;
    //利用index找出可能的地物
    for( int i = 0; i < row; i++){
        QTime t;
        t.start();
        for(int j = 0 ; j < column; j++){
            //对于每一个栅格
            QVector<SfsGeometry*>  geoms;
            pixels[i][j].withinDist(layer, radius, &geoms);
            int geomSize = geoms.size();
            //计算每个在栅格dist范围内的geometry对于像素栅格的贡献，为像素增加属性字段“density”存储计算结果
            double *density = new double;
            *density = 0;
            for(int z = 0; z < geomSize; z++){
                //获取属性值
                QVariant* val = new QVariant();
                geoms.at(z)->getPropertyValAt(index, val);
                double population = val->toDouble();
                delete  val;
                //获取地物与栅格的距离。目前只支持点状地物的计算。

                double dist = distance(&pixels[i][j], geoms.at(z));
                if(dist < 0){
                    qDebug()<<"Un-Point feature occurred when computing KDE!"<< endl;
                }
                else {
                    *density += population * pow(1 - pow(dist/radius, 2),2);
                }
            }
            *density *= 3/(PI * pow(radius, 2));
            *density *= layer->getWeightSumAt(index);
            //test
            sumDensity += *density;
            pixels[i][j].addProperty("density", Double_PRO, density);
            if(j>24&&j<460&&i>24&&i<250)
                int g=0;
            if(i == 0 && j == 0){
                densityMax = *density;
                densityMin = *density;
            }
            if(densityMax < *density)
                densityMax = *density;
            if(densityMin > *density)
                densityMin = *density;
        }
        qDebug("Rendering elapsed: %d ms", t.elapsed());
    }
    for(int i = 0; i < layer->geometries->size(); i++){
        QVariant* val = new QVariant();
        layer->geometries->at(i)->getPropertyValAt(index, val);
        sumWeight += val->toDouble();
    }
    qDebug()<<row<<endl;
    qDebug()<< column<<endl;
    qDebug()<<densityMax <<endl;
    qDebug()<<densityMin <<endl;
    qDebug()<<sumWeight<<endl;
    qDebug()<<sumDensity<<endl;
}

void KernelDensity::save(QString path)
{

}

int KernelDensity::getColumn() const
{
    return column;
}

void KernelDensity::setColumn(int value)
{
    column = value;
}

int KernelDensity::getRow() const
{
    return row;
}

void KernelDensity::setRow(int value)
{
    row = value;
}

double KernelDensity::getPixelSize() const
{
    return this->size;
}

void KernelDensity::setPixelSize(double value)
{
    this->size = value;
}

SfsLayer *KernelDensity::getLayer() const
{
    return layer;
}

void KernelDensity::setLayer(SfsLayer *value)
{
    layer = value;
}

int KernelDensity::getIndex() const
{
    return index;
}

void KernelDensity::setIndex(int value)
{
    index = value;
}

double KernelDensity::getDensityMax()
{
    return  densityMax;
}

double KernelDensity::getDensityMin()
{
    return densityMin;
}

