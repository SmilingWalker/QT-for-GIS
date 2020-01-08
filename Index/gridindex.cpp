#include "gridindex.h"
#include "./SFS/sfslayer.h"
#include<qdebug.h>
#include<cmath>

GridIndex::GridIndex(SfsLayer* layer, int row, int column, QObject *parent)
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
    BoundaryBox tempBox;

    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j++){
            tempBox.setBoundary(ty - i*gridHeight, ty - (i + 1)*gridHeight, lx + gridWidth*j, lx + gridWidth*(j + 1));
            grids[i][j].setBbox(&tempBox);
        }
    }

    int size = layer->geometries->size();
    SfsGeometry* geometry;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j++){
            for(int z = 0; z < size; z++){
                geometry = layer->geometries->at(z);
                if(grids[i][j].shouldHave(geometry)){
                    grids[i][j].add(geometry);
                    //测试：把一个grid内的地物设为selected
                    if(i == 5 && j == 5){
                        geometry->setIsSelected(true);
                    }
                }
            }
        }
    }
}
void GridIndex::query(QVector<unsigned int *> *id, QRect* ){

}
void GridIndex::save(QString path){

}

void GridIndex::relatedGrids(BoundaryBox *bbox, QVector<Grid *> *grids)
{
    SfsPoint lt_pt,rb_pt;

    lt_pt.y = bbox->getTopY();
    lt_pt.x = bbox->getLeftX();
    rb_pt.y = bbox->getBottomY();
    rb_pt.x = bbox->getRightX();
    int lt_column,lt_row,rb_column,rb_row;
    getIndexID(&lt_pt,&lt_row,&lt_column);
    getIndexID(&rb_pt,&rb_row,&rb_column);
    for(int i=lt_row; i <= rb_row; i++)
    {
        for(int j=lt_column;j<=rb_column;j++)
        {
            grids->append(this->gridAt(i, j));
        }
    }
}

Index_Type GridIndex::getType()
{
    return Grid_Index;
}

void GridIndex::getIndexID(SfsPoint *pt, int *row, int *column)
{

    double ty, by, lx, rx;
    ty = grids[0][0].getBbox()->getTopY();
    by = grids[int(this->row-1)][int(this->column-1)].getBbox()->getBottomY();
    lx = grids[0][0].getBbox()->getLeftX();
    rx = grids[int(this->row-1)][int(this->column-1)].getBbox()->getRightX();

    double width = rx - lx;
    double height = ty - by;
    double gridWidth = width / this->column;
    double gridHeight = height / this->row;
    int index_j = floor((pt->x-lx)/gridWidth);
    if(index_j<0)index_j=0;
    if(index_j>=this->column)index_j=this->column-1;
    int index_i = floor((ty-pt->y)/gridHeight);
    if(index_i<0)index_i=0;
    if(index_i>=this->row)index_i = this->row-1;
    if(index_i>=this->row)
        qDebug()<<"Error";
    else if(index_j>=this->column)
        qDebug()<<"Error";
    (*row) = index_i;
    (*column) = index_j;
}

