#ifndef PRQUADTREE_H
#define PRQUADTREE_H

#include <QObject>
#include<qvector.h>
#include<sfspoint.h>
#include<boundarybox.h>
#include<qdebug.h>
#include<metadata.h>
class SfsLayer;
class PRQuadTree : public QObject
{
    Q_OBJECT
public:
    explicit PRQuadTree(QObject *parent = nullptr);
    ~PRQuadTree();
    bool isleaf;
    PRQuadTree *NW;//PR四叉树四个方向
    PRQuadTree *NE;
    PRQuadTree *SW;
    PRQuadTree *SE;
    QVector<Metadata *> *Indexes;
    SfsPoint pt;
    BoundaryBox *bbox;//如果是叶子节点的话记录一下它的外接矩阵
    int ObjectsLimit;
public:
    void GenerateTree(SfsLayer *layer,BoundaryBox bbox);


signals:

public slots:
};

#endif // PRQUADTREE_H
