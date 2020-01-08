#ifndef PRQUADTREE_H
#define PRQUADTREE_H

#include <QObject>
#include<qvector.h>
#include<SFS/sfspoint.h>
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
    bool isleaf,isroot;
    PRQuadTree *NW;//PR�Ĳ����ĸ�����
    PRQuadTree *NE;
    PRQuadTree *SW;
    PRQuadTree *SE;
    QVector<Metadata *> *Indexes;
    SfsPoint pt;
    BoundaryBox *bbox;//�����Ҷ�ӽڵ�Ļ���¼һ��������Ӿ���
    QVector<BoundaryBox*> *bboxes;
public:
    void GenerateTree(SfsLayer *layer,BoundaryBox bbox);
    int getObjectsLimit() const;
    void setObjectsLimit(int value);

private:
    int ObjectsLimit;


signals:

public slots:
};

#endif // PRQUADTREE_H
