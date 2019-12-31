#include "grid.h"
#include "SFS/sfspoint.h"
#include "SFS/sfslinestring.h"
#include "SFS/sfspolygon.h"

Grid::Grid(BoundaryBox* bbox, QObject *parent) : QObject(parent)
{
    this->bbox = new BoundaryBox();
    double ty, by, lx, rx;
    ty = bbox->getTopY();
    by = bbox->getBottomY();
    lx = bbox->getLeftX();
    rx = bbox->getRightX();
    this->bbox->setBoundary(ty, by, lx, rx);
    width = lx - rx;
    height = ty - by;
}

Grid::Grid(){
    this->bbox = new BoundaryBox();
}
Grid::~Grid(){
    delete bbox;
}

double Grid::getWidth() const
{
    return width;
}

void Grid::setWidth(double value)
{
    width = value;
}

double Grid::getHeight() const
{
    return height;
}

void Grid::setHeight(double value)
{
    height = value;
}

BoundaryBox *Grid::getBbox() const
{
    return bbox;
}

void Grid::setBbox(BoundaryBox *bbox)
{
    double ty, by, lx, rx;
    ty = bbox->getTopY();
    by = bbox->getBottomY();
    lx = bbox->getLeftX();
    rx = bbox->getRightX();
    this->bbox->setBoundary(ty, by, lx, rx);
}

bool Grid::add(SfsGeometry* obj){
    this->id.append(obj->getId());
    return true;
}
bool Grid::remove(SfsGeometry* obj){
    unsigned int id = obj->getId();
    int size = this->id.size();
    int index = -1;
    for(int i = 0; i < size; i++){
        if(this->id[i] == id){
            index = i;
            break;
        }
    }
    if(index == -1){
        return false;
    }
    this->id.remove(index);
    return true;
}

bool Grid::shouldHave(SfsGeometry* obj){
    double ty, by, lx, rx;
    ty = bbox->getTopY();
    by = bbox->getBottomY();
    lx = bbox->getLeftX();
    rx = bbox->getRightX();

    int type = obj->GeometryType();
    if(type == Sfs_Point){
        if(bbox->isPointInRect((SfsPoint*)obj)){
            return true;
        }
        return false;
    }
    if(type == Sfs_LineString){
        if(bbox->isLineStringOverlapRect((SfsLineString*)obj)){
            return true;
        }
        return false;
    }
    if(type == Sfs_Polygon){
        if(bbox->isPolygonOverlapRect((SfsPolygon*)obj)){
            return true;
        }
        return false;
    }
    return false;
}

















