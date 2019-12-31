#ifndef GRIDINDEX_H
#define GRIDINDEX_H

#include <QObject>
#include <Index/grid.h>
#include <QRect>
#include <boundarybox.h>
#include <Index/spatialindex.h>

class SfsLayer;
class GridIndex : public SpatialIndex
{
    Q_OBJECT
public:
    explicit GridIndex(SfsLayer* layer, int row, int column, QObject *parent = nullptr);
    virtual ~GridIndex();
    GridIndex();
    void setColumnNum();
    void setRowNum();
    int getColumnNum() const;
    int getRowNum() const;
    virtual void createIndex();
    virtual void query(QVector<unsigned int *> *id, QRect* );
    virtual void save(QString path);
    Grid* gridAt(int row, int column);

signals:

public slots:

private:
    int column;
    int row;
    Grid** grids;
};

#endif // GRIDINDEX_H
