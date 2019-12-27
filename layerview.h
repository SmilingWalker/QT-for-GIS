#ifndef LAYERVIEW_H
#define LAYERVIEW_H

#include <QObject>
#include <QWidget>
#include<qtreeview.h>
#include<qaction.h>
#include<qmenu.h>

class LayerView :public QTreeView
{
    Q_OBJECT
public:
    explicit LayerView(QWidget *parent = nullptr);
    QAction* loadSld;
    QAction* setStyle;
    QMenu* treeWidgetItemMenu;
    int triggerdItemIndex;

    //SLD设置部分

//    //右键单击QTreeWidgetItem, 显示菜单栏（包括set style, load sld两个action), 左键单击无反应
//    void TreeWidgetItemPressed(QTreeWidgetItem* item);
//    //双击，则重绘，绘制所点击的图层
//    void TreeWidgetItemDoubleClicked(QTreeWidgetItem* item);
//    //load sld与set style 两个action被激活后触发以下两个槽函数
    void LoadSld();
    void SetStyle();

signals:

public slots:
};

#endif // LAYERVIEW_H
