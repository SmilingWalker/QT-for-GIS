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
    PRQuadTree *NW;//PR�Ĳ����ĸ�����
    PRQuadTree *NE;
    PRQuadTree *SW;
    PRQuadTree *SE;
    QVector<Metadata *> *Indexes;
    SfsPoint pt;
    BoundaryBox *bbox;//�����Ҷ�ӽڵ�Ļ���¼һ��������Ӿ���
    int ObjectsLimit;
public:
    void GenerateTree(SfsLayer *layer,BoundaryBox bbox);


signals:

public slots:
};

#endif // PRQUADTREE_H
