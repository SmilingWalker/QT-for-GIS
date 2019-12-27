#include "sfslayer.h"
#include<prquadtree.h>

SfsLayer::SfsLayer()
{
    setName("图层");
    geometries = new QVector<SfsGeometry*>;
    bbox = new BoundaryBox();
    TreeIndex = nullptr;
    render = nullptr;
}

QString SfsLayer::getName() const
{
    return name;
}

void SfsLayer::setName(const QString &value)
{
    name = value;
}

GeoType SfsLayer::getGeometype() const
{
    return Geometype;
}

void SfsLayer::setGeometype(const GeoType &value)
{
    Geometype = value;
}
