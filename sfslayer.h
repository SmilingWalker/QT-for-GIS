#ifndef SFSLAYER_H
#define SFSLAYER_H
#include<qvector.h>
#include<sfsgeometry.h>
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
private:
    QString name;
};

#endif // SFSLAYER_H
