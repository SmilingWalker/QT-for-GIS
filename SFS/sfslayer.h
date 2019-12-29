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
    ~SfsLayer();
public:
    QVector<SfsGeometry*> *geometries;
    BoundaryBox *bbox;
    QString getName() const;
    void setName(const QString &value);
    PRQuadTree *TreeIndex;
    SfsRender *render;
    GeoType getGeometype() const;
    void setGeometype(const GeoType &value);

    bool getVisible() const;
    void setVisible(bool value);

private:
    QString name;
    GeoType Geometype;
    bool visible;
};

#endif // SFSLAYER_H
