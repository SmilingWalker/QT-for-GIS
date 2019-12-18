#include "prquadtree.h"
#include<sfslayer.h>

PRQuadTree::PRQuadTree(QObject *parent) : QObject(parent)
{
    //初始化四叉树，开始为空
    NW = nullptr;
    NE = nullptr;
    SW = nullptr;
    SE = nullptr;
    Indexes = nullptr;
    isleaf = false;//默认构造为非叶子节点
    bbox = nullptr;
    ObjectsLimit = 7;//初始化，四叉树建立时，默认每个格最多有五个
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
        delete Indexes;
        Indexes = nullptr;
    }
    if(bbox!=nullptr)
    {
        delete bbox;
        bbox = nullptr;
    }

}

void PRQuadTree::GenerateTree(SfsLayer *layer, BoundaryBox bbox)
{
    PRQuadTree *tr = new PRQuadTree(this);
    int num = 0;//判断是否超限
    pt.x = (bbox.getLeftX()+bbox.getRightX())/2.0;
    pt.y = (bbox.getBottomY()+bbox.getTopY())/2.0;
    //递归生成四叉树的过程,每次传递过程只是对边界进行值传递，不是叶子节点就不用进行储存
    for(int j=0;j<layer->geometries->size();j++)
    {
        if(layer->geometries->value(j)->bbox->intersect(&bbox))
            num++;
    }
    if(num>ObjectsLimit)
    {
        //如果不满足最低限度，则进行递归计算
        NW = new PRQuadTree(this);
        NW->GenerateTree(layer,BoundaryBox(bbox.getTopY(),pt.y,pt.x,bbox.getRightX()));
        NE = new PRQuadTree(this);
        NE->GenerateTree(layer,BoundaryBox(bbox.getTopY(),pt.y,bbox.getLeftX(),pt.x));
        SW = new PRQuadTree(this);
        SW->GenerateTree(layer,BoundaryBox(pt.y,bbox.getBottomY(),pt.x,bbox.getRightX()));
        SE = new PRQuadTree(this);
        SE->GenerateTree(layer,BoundaryBox(pt.y,bbox.getBottomY(),bbox.getLeftX(),pt.x));
    }
    else{
        //如果满足最低限度，则记录bbox，和ID号
        Indexes = new QVector<int>;
        isleaf = true;
        this->bbox = new BoundaryBox(bbox.getTopY(),bbox.getBottomY(),bbox.getLeftX(),bbox.getRightX());
        for(int j=0;j<layer->geometries->size();j++)
        {
            if(layer->geometries->value(j)->bbox->intersect(&bbox))
                Indexes->append(j);//记录索引值
        }
    }
}
