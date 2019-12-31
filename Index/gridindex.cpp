#include "gridindex.h"
#include <SFS/sfslayer.h>

GridIndex::GridIndex(SfsLayer* layer, int row, int column, QObject *parent):SpatialIndex(parent)
{
    this->layer = layer;
    this->row = row;
    this->column = column;
    this->grids = new Grid*[row];
    for(int i = 0; i < row; i++){
        grids[i] = new Grid[column];
    }
}

GridIndex::~GridIndex(){
    for(int i = 0; i < row; i++){
        delete [] grids[i];
    }
    delete [] grids;
}

void GridIndex::setColumnNum(){

}
void GridIndex::setRowNum(){

}
int GridIndex::getColumnNum() const{
    return column;
}
int GridIndex::getRowNum() const{
    return row;
}
Grid* GridIndex::gridAt(int row, int column){
    return &grids[row][column];
}
void GridIndex::createIndex(){

    BoundaryBox* bbox = layer->bbox;
    double ty, by, lx, rx;
    ty = bbox->getTopY();
    by = bbox->getBottomY();
    lx = bbox->getLeftX();
    rx = bbox->getRightX();

    double width = rx - lx;
    double height = ty - by;
    double gridWidth = width / column;
    double gridHeight = height / row;
    BoundaryBox *tempBox;

    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j++){
            tempBox = new BoundaryBox();
            tempBox->setBoundary(ty - i*gridHeight, ty - (i + 1)*gridHeight, lx + gridWidth*j, lx + gridWidth*(j + 1));
            grids[i][j].setBbox(tempBox);
        }
    }
    int size = layer->geometries->size();
    SfsGeometry* geometry;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j++){
            for(int z = 0; z < size; z++){
                geometry = layer->geometries->at(z);
                if(grids[i][j].shouldHave(geometry)){
                    grids[i][j].add(geometry);//判断是否相交，相交则记录
                }
            }
        }
    }
}
void GridIndex::query(QVector<unsigned int *> *id, QRect* ){

}
void GridIndex::save(QString path){

}

