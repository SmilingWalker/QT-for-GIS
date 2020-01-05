#include "sfslayer.h"
#include<prquadtree.h>
#include<contentdb.h>
#include <Index/gridindex.h>

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
    return this->gridIndex;
}

double SfsLayer::getWeightSumAt(int index)
{
    if(index >= getPropertySize()){
        qDebug()<<"SfsLayer:: getWeightSumAt: imput index is larger than property size!"<<endl;
        return 0;
    }
    if(getPropertyTypeAt(index) != Double_PRO){
        qDebug()<<"SfsLayer:: getWeightSumAt: type of property at 'index' is not double!!"<<endl;
        return 0;
    }
    double sum = 0;
    QVariant* val  = new QVariant();
    for(auto i : *(this->geometries)){
        i->getPropertyValAt(index, val);
        sum += val->toDouble();
    }
    delete val;
    return sum;

}

int SfsLayer::getPropertySize()
{
    return geometries->at(0)->getProperties()->ProName->size();
}

int SfsLayer::getPropertyTypeAt(int index)
{
    return  geometries->at(0)->getProperties()->ProType->at(index);

}
