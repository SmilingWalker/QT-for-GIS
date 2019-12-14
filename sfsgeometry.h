#ifndef SFSGEOMETRY_H
#define SFSGEOMETRY_H



#define Sfs_Point 10000
#define Sfs_Polygon 10001
#define Sfs_LineString 10002
#define Sfs_MultiPolygon 10003
#define Sfs_MultiLineString 10004
#define Sfs_MultiPoint 10005
#define Sfs_GeometryCollection 10006

#include<qrect.h>
#include<boundarybox.h>
#include<qstring.h>
#include<qmap.h>
#include<qvector.h>
#include<properties.h>

class SfsGeometry
{

public:
    ~SfsGeometry();
    SfsGeometry();
    virtual int  GeometryType()=0;
    virtual int Dimension()=0;
    SfsGeometry * boundary();
    QString getName() const;
    void setName(const QString &value);
public:
    BoundaryBox *bbox;
    Properties *properties;

private:
    QString name;
};

#endif // SFSGEOMETRY_H
