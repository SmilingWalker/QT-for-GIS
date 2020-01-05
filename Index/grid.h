#ifndef GRID_H
#define GRID_H

#include <QObject>
#include <boundarybox.h>
#include "./SFS/sfsgeometry.h"
#include "./SFS/sfslinestring.h"
#include "./SFS/sfspolygon.h"
class Grid : public QObject
{
    Q_OBJECT
public:
    explicit Grid(BoundaryBox *value, QObject *parent = nullptr);
    Grid();
    ~Grid();
    double getWidth() const;
    void setWidth(double value);

    double getHeight() const;
    void setHeight(double value);

    BoundaryBox *getBbox() const;
    void setBbox(BoundaryBox *value);
    bool add(SfsGeometry* obj);
    bool remove(SfsGeometry* obj);
    bool shouldHave(SfsGeometry* obj);
    QVector<unsigned int>* getIDs();

signals:

public slots:
private:
    BoundaryBox* bbox;
    double width = 0;
    double height = 0;
    QVector<unsigned int > id;
};

#endif // GRID_H
