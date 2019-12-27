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

    //SLD���ò���

//    //�Ҽ�����QTreeWidgetItem, ��ʾ�˵���������set style, load sld����action), ��������޷�Ӧ
//    void TreeWidgetItemPressed(QTreeWidgetItem* item);
//    //˫�������ػ棬�����������ͼ��
//    void TreeWidgetItemDoubleClicked(QTreeWidgetItem* item);
//    //load sld��set style ����action������󴥷����������ۺ���
    void LoadSld();
    void SetStyle();

signals:

public slots:
};

#endif // LAYERVIEW_H
