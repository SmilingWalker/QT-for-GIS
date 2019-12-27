#ifndef LAYERTREE_H
#define LAYERTREE_H

#include <QWidget>
#include<qtreewidget.h>
#include<SFS/sfslayer.h>
#include<qdrag.h>
#include<qmimedata.h>
#include<qmenu.h>
#include<qcursor.h>
#include<QContextMenuEvent>

class LayerTree : public QTreeWidget
{
public:
    LayerTree(QWidget *parent);
public slots:
    void AddLayer(SfsLayer *layer);

private:
    QMenu *toolMenu;
    // QWidget interface
    QAction *toTop;
    QAction *toUpstair;
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // LAYERTREE_H
