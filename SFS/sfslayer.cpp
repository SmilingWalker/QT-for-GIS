#include "sfslayer.h"
#include<prquadtree.h>
#include<contentdb.h>

SfsLayer::SfsLayer()
{
    setName("图层");
    geometries = new QVector<SfsGeometry*>;
    bbox = new BoundaryBox();
    TreeIndex = nullptr;
    render = new SfsRender();
    visible = true;
    gridIndex = nullptr;
    textDB = nullptr;
}

SfsLayer::~SfsLayer()
{
    if(render!=nullptr)
        delete render;
    render = nullptr;
    if(TreeIndex != nullptr)
        delete TreeIndex;
    TreeIndex = nullptr;
    if(gridIndex!=nullptr)
        delete gridIndex;
    gridIndex = nullptr;
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

bool SfsLayer::getVisible() const
{
    return visible;
}

void SfsLayer::setVisible(bool value)
{
    visible = value;
}

void SfsLayer::createGridIndex(int row, int column)
{
    gridIndex = new GridIndex(this, row, column);
    gridIndex->createIndex();
}

GridIndex *SfsLayer::getGridIndex() const
{
    return gridIndex;
}
