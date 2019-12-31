#ifndef METADATA_H
#define METADATA_H

//用于存储数据项，
#include<SFS/sfslayer.h>
class Metadata
{
public:
    Metadata();
public:
    SfsLayer *layer;    //图层项
    QString content;    //数据项
    int ID;         //这是在图层内的顺序
    int ContentID;  //这是内容数据层
};

#endif // METADATA_H
