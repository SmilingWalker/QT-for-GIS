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
    // ��ʼĬ�϶��б߽������
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
