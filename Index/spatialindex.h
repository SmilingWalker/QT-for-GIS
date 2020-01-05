#ifndef SPATIALINDEX_H
#define SPATIALINDEX_H


#define QUAD_INDEX 1002
#define R_INDEX 1003

#include <QObject>
#include <boundarybox.h>
#include"./SFS/sfsgeometry.h"
#include"grid.h"

enum Index_Type{
    Grid_Index=1001,
    Quad_Index = 1002,
    R_Index = 1003,
    GENERIC=1000,

};

class SfsLayer;

class SpatialIndex : public QObject
{
    Q_OBJECT
public:
    explicit SpatialIndex(QObject *parent = nullptr);
    virtual ~SpatialIndex();
    virtual void createIndex();
    virtual void query(QVector<unsigned int *> *id, QRect* );
    virtual void save(QString path);
    //返回与bbox相交的所有grids.从而得到所有可能在距某一目标dist范围内的所有地物。
    virtual void relatedGrids(BoundaryBox* bbox, QVector<Grid*> *grids);
    virtual Index_Type getType();
signals:

public slots:
protected:
    SfsLayer* layer;
};

#endif // SPATIALINDEX_H
