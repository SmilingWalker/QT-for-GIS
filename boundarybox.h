#ifndef BOUNDARYBOX_H
#define BOUNDARYBOX_H
class SfsPoint;
class SfsLineString;
class SfsPolygon;

class BoundaryBox
{
public:
    BoundaryBox();
    BoundaryBox(double topY,double buttomY,double leftX,double rightX);

    void setBoundary(double topY,double buttomY,double leftX,double rightX);
    double getTopY() const;
    void setTopY(double value);

    double getBottomY() const;
    void setBottomY(double value);

    double getLeftX() const;
    void setLeftX(double value);

    double getRightX() const;
    void setRightX(double value);
    bool intersect(BoundaryBox *box);
    bool inBox(float x,float y);


    //¸½¼Ó
    bool isLineStripInRect(SfsPoint* point1, SfsPoint* point2);
    bool isLineStripsIntersect(SfsPoint* point1, SfsPoint* point2, SfsPoint* point3, SfsPoint* point4);
    bool isLineStripIntersectRect(SfsPoint* point1, SfsPoint* point2);
    bool isLineStripOverLapRect(SfsPoint* point1, SfsPoint* point2);
    bool isPointInRect(SfsPoint* point);
    bool isLineStringOverlapRect(SfsLineString* line);
    bool isPolygonOverlapRect(SfsPolygon* polygon);


private:
    double topY;
    double bottomY;
    double leftX;
    double rightX;

};

#endif // BOUNDARYBOX_H
