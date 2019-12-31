#include "boundarybox.h"
#include<SFS/sfspoint.h>
#include<SFS/sfspolygon.h>
#include<SFS/sfslinestring.h>
#include<qdebug.h>

BoundaryBox::BoundaryBox()
{

}

BoundaryBox::BoundaryBox(double topY, double buttomY, double leftX, double rightX)
{
    setTopY(topY);
    setLeftX(leftX);
    setRightX(rightX);
    setBottomY(buttomY);

}

void BoundaryBox::setBoundary(double topY, double buttomY, double leftX, double rightX)
{
    setTopY(topY);
    setBottomY(buttomY);
    setLeftX(leftX);
    setRightX(rightX);

}

double BoundaryBox::getTopY() const
{
    return topY;
}

void BoundaryBox::setTopY(double value)
{
    topY = value;
}

double BoundaryBox::getBottomY() const
{
    return bottomY;
}

void BoundaryBox::setBottomY(double value)
{
    bottomY = value;
}

double BoundaryBox::getLeftX() const
{
    return leftX;
}

void BoundaryBox::setLeftX(double value)
{
    leftX = value;
}

double BoundaryBox::getRightX() const
{
    return rightX;
}

void BoundaryBox::setRightX(double value)
{
    rightX = value;
}

bool BoundaryBox::intersect(BoundaryBox *box)
{
    if(this->leftX>box->rightX||this->rightX<box->leftX)
        return false;
    else if(this->topY<box->bottomY||this->bottomY>box->topY)
        return false;
    else
        return  true;
}

bool BoundaryBox::inBox(float x, float y)
{
    //判断 点是否在矩形内部
    if(x>this->leftX && x<this->rightX && y>this->bottomY && y < this->topY)
        return true;
    else
        return false;
}

bool BoundaryBox::isLineStripInRect(SfsPoint *point1, SfsPoint *point2)
{
    //如果有一个点在矩形内部，则线段与矩形相交或包含在矩形内
    if( isPointInRect(point1) || isPointInRect(point2)){
        return true;
    }
    return false;
}

bool BoundaryBox::isLineStripsIntersect(SfsPoint *point1, SfsPoint *point2, SfsPoint *point3, SfsPoint *point4)
{
    double x0 = point2->x - point1->x;
    double y0 = point2->y - point1->y;

    double x1 = point3->x - point1->x;
    double y1 = point3->y - point1->y;

    double x2 = point4->x - point1->x;
    double y2 = point4->y - point1->y;

    double x3 = point4->x - point3->x;
    double y3 = point4->y - point3->y;

    double x4 = point1->x - point3->x;
    double y4 = point1->y - point3->y;

    double x5 = point2->x - point3->x;
    double y5 = point2->y - point3->y;

    if((x0*y1 - x1*y0) * (x0*y2 - x2*y0) < 0){
        if((x3*y4 - x4*y3) * (x3*y5 - x5*y3) < 0){
            return true;
        }
    }
    return false;
}

bool BoundaryBox::isLineStripIntersectRect(SfsPoint *point1, SfsPoint *point2)
{
    SfsPoint* points = new SfsPoint[5];

    points[0].x = this->getLeftX();
    points[0].y = this->getTopY();

    points[1].x = this->getRightX();
    points[1].y = this->getTopY();

    points[2].x = this->getRightX();
    points[2].y = this->getBottomY();

    points[3].x = this->getLeftX();
    points[3].y = this->getBottomY();

    points[4].x = this->getLeftX();
    points[4].y = this->getTopY();

    for(int i = 0; i < 4; i++){
        if(isLineStripsIntersect(point1, point2, &points[i], &points[i+1])){
            qDebug()<<"i: "<< i;
            qDebug()<<points[i].x << " "<< points[i].y<<endl;
            qDebug()<<points[i+1].x<<" "<<points[i+1].y<<endl;
            return true;
        }
    }
    return false;
}

bool BoundaryBox::isLineStripOverLapRect(SfsPoint *point1, SfsPoint *point2)
{
    if(isLineStripInRect(point1, point2)){
        return true;
    }
    if(isLineStripIntersectRect(point1, point2)){
        return true;
    }
    return false;
}

bool BoundaryBox::isPointInRect(SfsPoint *point)
{
    double x, y;
    x = point->x;
    y = point->y;

    double lx, rx, ty, by;
    lx = this->getLeftX();
    rx = this->getRightX();
    ty = this->getTopY();
    by = this->getBottomY();
    if((x - lx)*(x - rx) > 0){
        return false;
    }
    if((y - ty)*(y - by) > 0){
        return false;
    }
    return true;
}

bool BoundaryBox::isLineStringOverlapRect(SfsLineString *line)
{
    int size = line->pts->size();
    for(int i = 0; i < size - 1; i++){
        SfsPoint* pt1 = line->PointN(i);
        SfsPoint* pt2 = line->PointN(i+1);
        if(isLineStripOverLapRect(pt1, pt2)){
            return true;
        }
    }
    return false;
}

bool BoundaryBox::isPolygonOverlapRect(SfsPolygon *polygon)
{
    SfsLineString* line = polygon->boundaries->at(0);
    if(isLineStringOverlapRect(line)){
        return true;
    }
    return false;
}
