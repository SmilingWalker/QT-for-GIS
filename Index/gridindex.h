#ifndef GRIDINDEX_H
#define GRIDINDEX_H

#include "grid.h"
#include <QRect>
#include <boundarybox.h>
#include "Index/spatialindex.h"


class SfsLayer;
class GridIndex : public SpatialIndex
{
    Q_OBJECT
public:
    explicit GridIndex(SfsLayer* layer, int row, int column, QObject *parent = nullptr);
     ~GridIndex() override;
    GridIndex();
    void setColumnNum();
    void setRowNum();
    int getColumnNum() const;
    int getRowNum() const;
    virtual void createIndex() override;
    virtual void query(QVector<unsigned int *> *id, QRect* ) override;
    virtual void save(QString path) override;
    virtual void relatedGrids(BoundaryBox *bbox, QVector<Grid *> *grids) override;
    virtual Index_Type getType() override;
    void getIndexID(SfsPoint *pt,int* row,int* column);
    Grid* gridAt(int row, int column);
signals:

public slots:

private:
    int column;
    int row;
    Grid** grids;
};

#endif // GRIDINDEX_H
