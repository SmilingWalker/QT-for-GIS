#include "sfsgeometry.h"

SfsGeometry::~SfsGeometry()
{
    if(bbox!=nullptr)
    {
        delete bbox;
        bbox = nullptr;
    }
    if(properties!=nullptr)
    {
        delete properties;
        properties=nullptr;
    }
}

SfsGeometry::SfsGeometry()
{
    // 初始默认都有边界和属性
    bbox = new BoundaryBox();
    properties = new Properties();
}

SfsGeometry *SfsGeometry::boundary()
{
    return nullptr;
}

QString SfsGeometry::getName() const
{
    return name;
}

void SfsGeometry::setName(const QString &value)
{
    name = value;
}
