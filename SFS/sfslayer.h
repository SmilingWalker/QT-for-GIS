#ifndef SFSLAYER_H
#define SFSLAYER_H
#include<qvector.h>
#include<SFS/sfsgeometry.h>
#include<SFS/sfsrender.h>

class PRQuadTree;

class SfsLayer
{
public:
    SfsLayer();
public:
    QVector<SfsGeometry*> *geometries;
    BoundaryBox *bbox;
    QString getName() const;
    void setName(const QString &value);
    PRQuadTree *TreeIndex;
    SfsRender *render;
    GeoType getGeometype() const;
    void setGeometype(const GeoType &value);

private:
    QString name;
    GeoType Geometype;
};

#endif // SFSLAYER_H
