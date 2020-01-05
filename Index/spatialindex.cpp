#include "spatialindex.h"
#include "./SFS/sfslayer.h"
#include "grid.h"

SpatialIndex::SpatialIndex(QObject *parent) : QObject(parent)
{

}

SpatialIndex::~SpatialIndex()
{

}

void SpatialIndex::createIndex()
{

}

void SpatialIndex::query(QVector<unsigned int *> *id, QRect *)
{

}

void SpatialIndex::save(QString path)
{

}

void SpatialIndex::relatedGrids(BoundaryBox *bbox, QVector<Grid *> *grids)
{

}

Index_Type SpatialIndex::getType()
{
    return GENERIC;
}
