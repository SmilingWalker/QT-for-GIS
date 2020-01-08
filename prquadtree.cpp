#include "prquadtree.h"
#include<SFS/sfslayer.h>

PRQuadTree::PRQuadTree(QObject *parent) : QObject(parent)
{
    //��ʼ���Ĳ�������ʼΪ��
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;
    Indexes = nullptr;
    isleaf = false;//Ĭ�Ϲ���Ϊ��Ҷ�ӽڵ�
    bbox = nullptr;
    bboxes = nullptr;
}

PRQuadTree::~PRQuadTree()
{

    if(NW!=nullptr)
    {
        delete NW;
        NW = nullptr;
    }
    if(NE!=nullptr)
    {
        delete NE;
        NE = nullptr;
    }
    if(SW!=nullptr)
    {
        delete SW;
        SW = nullptr;
    }
    if(SE!=nullptr)
    {
        delete SE;
        SE = nullptr;
    }
    if(Indexes!=nullptr)
    {
        qDeleteAll(Indexes->begin(),Indexes->end());//�ͷſռ�
        Indexes->clear();
        delete Indexes;
        Indexes = nullptr;
    }
    if(bbox!=nullptr)
    {
        delete bbox;
        bbox = nullptr;
    }
    if(bboxes!=nullptr)
    {
        bboxes->clear();
        delete bboxes;
        bboxes = nullptr;
    }

}

void PRQuadTree::GenerateTree(SfsLayer *layer, BoundaryBox bbox)
{
    int num = 0;//�ж��Ƿ���
    pt.x = (bbox.getLeftX()+bbox.getRightX())/2.0;
    pt.y = (bbox.getBottomY()+bbox.getTopY())/2.0;
    //�ݹ������Ĳ����Ĺ���,ÿ�δ��ݹ���ֻ�ǶԱ߽����ֵ���ݣ�����Ҷ�ӽڵ�Ͳ��ý��д���
    for(int j=0;j<layer->geometries->size();j++)
    {
        if(layer->geometries->value(j)->bbox->intersect(&bbox))
            num++;
    }
    if(num>ObjectsLimit)
    {
        //�������������޶ȣ�����еݹ����
        NW = new PRQuadTree(this);
        NW->setObjectsLimit(this->getObjectsLimit());
        NW->GenerateTree(layer,BoundaryBox(bbox.getTopY(),pt.y,pt.x,bbox.getRightX()));
        NE = new PRQuadTree(this);
        NE->setObjectsLimit(this->getObjectsLimit());
        NE->GenerateTree(layer,BoundaryBox(bbox.getTopY(),pt.y,bbox.getLeftX(),pt.x));
        SW = new PRQuadTree(this);
        SW->setObjectsLimit(this->getObjectsLimit());
        SW->GenerateTree(layer,BoundaryBox(pt.y,bbox.getBottomY(),pt.x,bbox.getRightX()));
        SE = new PRQuadTree(this);
        SE->setObjectsLimit(this->getObjectsLimit());
        SE->GenerateTree(layer,BoundaryBox(pt.y,bbox.getBottomY(),bbox.getLeftX(),pt.x));
    }
    else{
        //�����������޶ȣ����¼bbox����ID��
        Indexes = new QVector<Metadata *>;
        isleaf = true;
        this->bbox = new BoundaryBox(bbox.getTopY(),bbox.getBottomY(),bbox.getLeftX(),bbox.getRightX());
        layer->TreeIndex->bboxes->append(this->bbox);
        for(int j=0;j<layer->geometries->size();j++)
        {
            if(layer->geometries->value(j)->bbox->intersect(&bbox))
             {
                Metadata *data = new Metadata();
                //�����ݽ��з�װ
                data->ID = j;
                data->layer = layer;
                data->content = "";
                Indexes->append(data);//��¼����ֵ
            }
        }
    }
}

int PRQuadTree::getObjectsLimit() const
{
    return ObjectsLimit;
}

void PRQuadTree::setObjectsLimit(int value)
{
    ObjectsLimit = value;
}
