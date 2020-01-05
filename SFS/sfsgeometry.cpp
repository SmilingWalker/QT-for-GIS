#include "sfsgeometry.h"
#include "sfslayer.h"

SfsGeometry::~SfsGeometry()
{
    if(bbox!=nullptr)
    {
        delete bbox;
        bbox = nullptr;
    }
    if( properties != nullptr){
        delete properties;
        properties = nullptr;
    }

}

SfsGeometry::SfsGeometry()
{
    bbox = new BoundaryBox();
    properties = new Properties();
}

SfsGeometry *SfsGeometry::boundary()
{
    return nullptr;
}

unsigned int SfsGeometry::getId() const
{
    return id;
}

void SfsGeometry::setId(unsigned int value)
{
    id = value;
}

bool SfsGeometry::getIsSelected() const
{
    return isSelected;
}

void SfsGeometry::setIsSelected(bool value)
{
    isSelected = value;
}

void SfsGeometry::setProperties(Properties *properties)
{
    this->properties = properties;
}

Properties *SfsGeometry::getProperties() const
{
    return properties;
}

void SfsGeometry::withinDist(SfsLayer* layer, double dist, QVector<SfsGeometry*> *geometries)
{

}

void SfsGeometry::addProperty(QString name, int type, void *value)
{
    properties->addProperty(name, type, value);
}

void SfsGeometry::getPropertyValAt(int index, QVariant* value)
{
    properties->getPropertyValAt(index, value);
}

int SfsGeometry::getPropertyTypeAt(int index)
{
    int type = properties->getProprtyTypeAt(index);
    return type;
}
