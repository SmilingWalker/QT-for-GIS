#include "sfspoint.h"
#include "Tool/Kernel/SpatialCalculation.h"
#include "Index/spatialindex.h"

SfsPoint::SfsPoint()
{

}

SfsPoint::SfsPoint(double x, double y)
{
    this->x = x; this->y = y;
}

SfsPoint::~SfsPoint()
{
    int i = 0;
}

bool SfsPoint::operator==(const SfsPoint * pt)
{
    if(fabs(this->x-pt->x)<esp&&fabs(this->y-pt->y)<esp)
        return true;
    else
        return false;

}
bool SfsPoint::operator=(const SfsPoint pt){
    this->x = pt.x;
    this->y = pt.y;
    return true;
}

void SfsPoint::withinDist(SfsLayer *layer, double dist, QVector<SfsGeometry *> *geometries)
{
    SpatialIndex* sptIndex = (SpatialIndex*)layer->getGridIndex();
    //目前只支持基于空间索引的within查询
    if(sptIndex != nullptr){
        QVector<unsigned int > *ids;
        QVector<Grid*> *grids = new QVector<Grid*>;
        BoundaryBox* bbox = new BoundaryBox();
        bbox->setTopY(this->y + dist);
        bbox->setLeftX(this->x - dist);
        bbox->setRightX(this->x + dist);
        bbox->setBottomY( this->y - dist);
        //找出所有可能包含目标的空间格网
        sptIndex->relatedGrids(bbox, grids);

        int gridsSize = grids->size();
        int idSize = 0;
        SfsGeometry* tempGeo;
        for(int i = 0; i < gridsSize; i++){
            //对于每一个格网，检查地物之间距离
            //目前假定地物ID与其在图层中的下标相同
            ids = grids->at(i)->getIDs();
            idSize = ids->size();
            for(int j = 0; j < idSize; j++){
                //错误！！！ geometries本来应该是空的。
                tempGeo = layer->geometries->at(ids->at(j));
                if(distance(tempGeo, this) < dist){
                    if(!geometries->contains(tempGeo)){
                        geometries->append(tempGeo);
                    }
                }
            }
        }
    }
    //还不支持无空间索引的。所以目前必须建立空间索引后才能实现within查询
    else{

    }
}

GeoType SfsPoint::GeometryType()
{
    return Sfs_Point;
}

int SfsPoint::Dimension()
{
    return 0;
}
