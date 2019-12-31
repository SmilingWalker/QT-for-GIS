#ifndef SFSGEOMETRY_H
#define SFSGEOMETRY_H



//#define Sfs_Point 10000
//#define Sfs_Polygon 10001
//#define Sfs_LineString 10002

//#define Sfs_MultiPolygon 10003
//#define Sfs_MultiLineString 10004
//#define Sfs_MultiPoint 10005
//#define Sfs_GeometryCollection 10006

#include<qrect.h>
#include<boundarybox.h>
#include<qstring.h>
#include<qmap.h>
#include<qvector.h>
#include<properties.h>

enum GeoType{
    Sfs_Point=10000,Sfs_Polygon=10001,Sfs_LineString=10002,Sfs_MultiPolygon=10003,
    Sfs_MultiLineString=10004,Sfs_MultiPoint=10005,Sfs_GeometryCollection=10006
};

class SfsGeometry
{

public:
    ~SfsGeometry();
    SfsGeometry();
    virtual GeoType  GeometryType()=0;
    virtual int Dimension()=0;
    SfsGeometry * boundary();
    QString getName() const;
    void setName(const QString &value);
    Properties *getProperties() const;
    void setProperties(Properties *value);
    unsigned int getId() const;
    void setId(unsigned int value);
    bool getIsSelected() const;
    void setIsSelected(bool value);

public:
    BoundaryBox *bbox;
    Properties *properties;
    unsigned int id;
    bool isSelected = false;

private:
    QString name;
};

#endif // SFSGEOMETRY_H
