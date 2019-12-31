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
#include<SFS/sfsmap.h>
#include<qfiledialog.h>
#include<filereader.h>
#include<QMessageBox>
#include<SLD/ogcsld.h>
#include<QDomDocument>
#include<variousDialogs.h>

class LayerTree : public QTreeWidget
{
    Q_OBJECT
public:
    LayerTree(QWidget *parent);
    ~LayerTree() override ;
public slots:
    void AddLayer(SfsLayer *layer);
    //SLD设置部分
    //load sld与set style 两个action被激活后触发以下两个槽函数
    void LoadSld();
    void SetStyle();

    //移动图层顺序
    void LayerToUpstair();
    void LayerToTop();
    void LayerToDown();
    void Zoom();
    void Remove();

    void setLayerVisibility(QTreeWidgetItem *item);
    void itemChangeSlot(QTreeWidgetItem *item, int column);
private:
    QMenu *toolMenu;
    // QWidget interface
    QAction *toTop;
    QAction *toUpstair;
    QAction *toDownStair;
    QAction* loadSld;
    QAction* setStyle;
    QAction *ZoomToLayer;
    QAction *remove;
    FileReader *fileReader;

    //SLD配置
    QMenu* treeWidgetItemMenu;
    int triggerdItemIndex;
public:
    SfsMap *map;
    // QWidget interface
protected:
    //右键单击QTreeWidgetItem, 显示菜单栏（包括set style, load sld两个action), 左键单击无反应
    //void mousePressEvent(QMouseEvent *event) override;（不能用这个，这个会导致其他单击事件失效），比如展开节点等事件
    //双击，则重绘，绘制所点击的图层
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    //打开menu的响应函数，QT自身封装（//右键单击QTreeWidgetItem, 显示菜单栏（包括set style, load sld两个action), 左键单击无反应
    void contextMenuEvent(QContextMenuEvent *event) override;
signals:
    void updateMap();//需要提醒glwidget 更新地图绘制
    void LayerZoom(SfsLayer*);
    void RemoveLayer(SfsLayer*);
};

#endif // LAYERTREE_H
