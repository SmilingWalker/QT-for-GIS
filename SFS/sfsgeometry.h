#ifndef SFSGEOMETRY_H
#define SFSGEOMETRY_H

#include<qrect.h>
#include<boundarybox.h>
#include<properties.h>

enum GeoType{
    Sfs_Point=10000,Sfs_Polygon=10001,Sfs_LineString=10002,Sfs_MultiPolygon=10003,
    Sfs_MultiLineString=10004,Sfs_MultiPoint=10005,Sfs_GeometryCollection=10006
};

class SfsLayer;

class SfsGeometry
{
public:
    virtual ~SfsGeometry();
    SfsGeometry();
    virtual GeoType  GeometryType()=0;
    virtual int Dimension()=0;
    SfsGeometry * boundary();
    BoundaryBox *bbox;
    unsigned int getId() const;
    void setId(unsigned int value);
    bool getIsSelected() const;
    void setIsSelected(bool value);
    void setProperties(Properties* properties);
    Properties *getProperties() const;
    //虚函数，目前只有point重写了。得到距地物"this" dist范围内的所有地物
    virtual void withinDist(SfsLayer* layer, double dist, QVector<SfsGeometry*> *geometries);
    //添加属性
    void addProperty(QString name, int type, void* value);
    //获取属性值
    void getPropertyValAt(int index, QVariant* value);
    int getPropertyTypeAt(int index);
    //将空间计算（distanceTo 等）作为地物成员函数更合适（虚函数），但是为了方便，目前没有改
    Properties* properties;

protected:
    unsigned int id;
    bool isSelected = false;

};

#endif // SFSGEOMETRY_H
