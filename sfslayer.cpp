#include "sfslayer.h"

SfsLayer::SfsLayer()
{
    setName("图层");
    geometries = new QVector<SfsGeometry*>;
    bbox = new BoundaryBox();
}

QString SfsLayer::getName() const
{
    return name;
}

void SfsLayer::setName(const QString &value)
{
    name = value;
}
