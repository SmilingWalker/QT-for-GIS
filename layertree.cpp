#include "layertree.h"
#include<qdebug.h>

LayerTree::LayerTree(QWidget *parent):QTreeWidget(parent)
{
    this->setHeaderHidden(true);
    toolMenu = new QMenu(this);
    toTop = new QAction(QString::fromLocal8Bit("到图层最顶部"),this);
    toUpstair = new QAction(QString::fromLocal8Bit("到上一层"),this);
}

void LayerTree::AddLayer(SfsLayer *layer)
{
    QTreeWidgetItem *layerItem = new QTreeWidgetItem(this,QStringList(QStringList(layer->getName())));
    for (int i=0;i<layer->geometries->size();i++) {
        QTreeWidgetItem *child = new QTreeWidgetItem(QStringList(layer->geometries->value(i)->getName()));
        layerItem->addChild(child);

    }
}

void LayerTree::contextMenuEvent(QContextMenuEvent *event)
{
    toolMenu->addAction(toTop);
    toolMenu->addAction(toUpstair);
    QTreeWidgetItem *item = this->itemAt(event->pos());
    if(item!=nullptr)
    {
        this->setContextMenuPolicy(Qt::DefaultContextMenu);
        toolMenu->exec(QCursor::pos());
    }
    event->accept();

}

