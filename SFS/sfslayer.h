#ifndef SFSLAYER_H
#define SFSLAYER_H
#include<qvector.h>
#include<SFS/sfsgeometry.h>
#include<SFS/sfsrender.h>
#include <Index/gridindex.h>

class PRQuadTree;
class ContentDB;
class SfsLayer
{
public:
    SfsLayer();
    ~SfsLayer();
public:
    QVector<SfsGeometry*> *geometries;
    BoundaryBox *bbox;
    ContentDB *textDB;

    QString getName() const;
    void setName(const QString &value);
    PRQuadTree *TreeIndex;
    SfsRender *render;
    GeoType getGeometype() const;
    void setGeometype(const GeoType &value);

    bool getVisible() const;
    void setVisible(bool value);
    void createGridIndex(int row, int column);

    GridIndex *getGridIndex() const;

private:
    QString name;
    GeoType Geometype;
    bool visible;
    GridIndex* gridIndex;
};

#endif // SFSLAYER_H
