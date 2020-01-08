#include "treeindex_select.h"
#include "ui_treeindex_select.h"

treeIndex_Select::treeIndex_Select(QWidget *parent,SfsMap *map) :
    QWidget(parent),
    ui(new Ui::treeIndex_Select)
{
    ui->setupUi(this);
    for(int i=1;i<30;i++){
        ui->limitation->insertItem(i,QString::number(i));
    }
    this->map = map;
    ui->limitation->setCurrentText("5");
    if(map!=nullptr&&map->layers->size()>0){
        for(int i=0;i<map->layers->size();i++){
            ui->layers->insertItem(i,map->layers->at(i)->getName());
        }
    }
}

treeIndex_Select::~treeIndex_Select()
{
    delete ui;
}

void treeIndex_Select::on_yes_clicked()
{
   SfsLayer *layer =map->layers->at(ui->layers->currentIndex());
   int limite = ui->limitation->currentText().toInt();
   TreeIndex(layer,limite);
   this->close();
}

void treeIndex_Select::on_cancel_clicked()
{
    this->close();
}
