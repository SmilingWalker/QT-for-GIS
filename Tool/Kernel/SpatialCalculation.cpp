#include "SpatialCalculation.h"
#include <QDebug>
#include <properties.h>
#include <math.h>
#include<exception>

double distance(SfsGeometry* obj1, SfsGeometry* obj2){
    double distance = -1;
    int type1 = obj1->GeometryType();
    int type2 = obj2->GeometryType();
    if(type1 == Sfs_Point){
        if(type2 == Sfs_Point){
            float dx = ((SfsPoint*)obj1)->x - ((SfsPoint*)obj2)->x;
            float dy = ((SfsPoint*)obj1)->y - ((SfsPoint*)obj2)->y;
            distance = dx*dx + dy*dy;
        }
    }
    else{
        qDebug()<<"Spatial calculation only supports SfsPoint"<<endl;
        return -1;
    }
    return pow(distance, 0.5);
}

//only for points
SfsPoint meanCenter(SfsLayer* layer){

    int size = layer->geometries->size();
    SfsPoint center;
    double x=0, y=0;
    for(int i = 0; i < size; i++){
        SfsGeometry* obj = layer->geometries->at(i);
        if(obj->GeometryType() == Sfs_Point){
            x += ((SfsPoint*)obj)->x / size;
            y += ((SfsPoint*)obj)->y / size;
        }
    }
    center.x = x;
    center.y = y;
    return center;
}
//only for points for now
SfsPoint meanCenter(SfsLayer *layer, int index)
{
    int propertySize = layer->geometries->at(0)->getProperties()->ProName->size();
    if(index >= propertySize){
        qDebug()<<"Index out of Bound!"<<endl;
        throw "Index out of Bound!";
    }
    int type = layer->geometries->at(0)->getProperties()->ProType->at(index);
    if(type != Double_PRO){
        qDebug()<<"Attribute not double!"<<endl;
        throw "Attribute not double!";
    }

    double x = 0;
    double y = 0;
    double weightSum = 0;
    double weightTemp = 0;
    QVariant* val = new QVariant();
    int size  = layer->geometries->size();
    for(int i = 0; i < size; i++){
        SfsGeometry* obj = layer->geometries->at(i);
        if(obj->GeometryType() == Sfs_Point){
            obj->getProperties()->getPropertyValAt(index, val);
            weightTemp = val->toDouble();
            x += ((SfsPoint*)obj)->x * weightTemp;
            y += ((SfsPoint*)obj)->y * weightTemp;
            weightSum += weightTemp;
        }
    }

    return SfsPoint(x/weightSum, y/weightSum);
}
//only for points
double midDistToCtr(SfsLayer *layer, SfsPoint *center)
{
    int size = layer->geometries->size();
    double *dists = new double[size];
    for(int i = 0; i < size; i++){
        SfsGeometry* obj = layer->geometries->at(i);
        if(obj->GeometryType() == Sfs_Point){
            dists[i] = distance(obj, center);
        }
    }
    quick_sort(dists, 0, size-1);
    return dists[size/2];
}

//only for points
double stdDist(SfsLayer *layer, SfsPoint *center)
{
    double x2 = 0;
    double y2 = 0;
    int size  = layer->geometries->size();
    for(int i = 0; i < size; i++){
        SfsGeometry* obj = layer->geometries->at(i);
        if(obj->GeometryType() == Sfs_Point){
            x2 += pow((((SfsPoint*)obj)->x -center->x), 2);
            y2 += pow((((SfsPoint*)obj)->y - center->y), 2);
        }
    }
    double dist = pow(x2 + y2, 0.5);
    return dist;
}

double stdDist(SfsLayer *layer, SfsPoint *center, int index)
{
    //目前认为一个图层内所有地物都有相同种类的属性，且属性值不为空
    int propertySize = layer->geometries->at(0)->getProperties()->ProName->size();
    if(index >= propertySize){
        qDebug()<<"Index out of Bound!"<<endl;
        throw "Index out of Bound!";
    }
    //只计算double属性。为什么不计算int?
    int type = layer->geometries->at(0)->getProperties()->ProType->at(index);
    if(type != Double_PRO){
        qDebug()<<"Attribute not double!"<<endl;
        throw "Attribute not double!";
    }

    double x2 = 0;
    double y2 = 0;
    double weightSum = 0;
    double weightTemp = 0;
    QVariant* val = new QVariant();
    int size  = layer->geometries->size();
    for(int i = 0; i < size; i++){
        SfsGeometry* obj = layer->geometries->at(i);
        if(obj->GeometryType() == Sfs_Point){
            obj->getProperties()->getPropertyValAt(index, val);
            weightTemp = val->toDouble();
            x2 += pow((((SfsPoint*)obj)->x -center->x), 2) * weightTemp;
            y2 += pow((((SfsPoint*)obj)->y - center->y), 2) * weightTemp;
            weightSum += weightTemp;
        }
    }

    double dist = pow(x2/weightSum + y2/weightSum, 0.5);
    return dist;
}

void quick_sort(double s[], int l, int r){
    if (l < r){
        int i = l, j = r; //左右下标
        double x = s[l]; //基准值
        while (i < j){
            while(i < j && s[j] >= x) // 从右向左找第一个小于x的数
                j--;
            if(i < j)
                s[i++] = s[j];
            while(i < j && s[i] < x) // 从左向右找第一个大于等于x的数
                i++;
            if(i < j)
                s[j--] = s[i];
        }
        s[i] = x;
        quick_sort(s, l, i - 1); // 递归调用
        quick_sort(s, i + 1, r);
    }
}
