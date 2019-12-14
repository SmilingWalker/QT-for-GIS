#ifndef BOUNDARYBOX_H
#define BOUNDARYBOX_H


class BoundaryBox
{
public:
    BoundaryBox();

    void setBoundary(double topY,double buttomY,double leftX,double rightX);
    double getTopY() const;
    void setTopY(double value);

    double getBottomY() const;
    void setBottomY(double value);

    double getLeftX() const;
    void setLeftX(double value);

    double getRightX() const;
    void setRightX(double value);

private:
    double topY;
    double bottomY;
    double leftX;
    double rightX;

};

#endif // BOUNDARYBOX_H
