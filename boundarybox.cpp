#include "boundarybox.h"

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
