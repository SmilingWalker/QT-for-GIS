#include "boundarybox.h"

BoundaryBox::BoundaryBox()
{

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
