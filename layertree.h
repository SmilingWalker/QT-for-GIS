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
    //SLD���ò���
    //load sld��set style ����action������󴥷����������ۺ���
    void LoadSld();
    void SetStyle();

    //�ƶ�ͼ��˳��
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

    //SLD����
    QMenu* treeWidgetItemMenu;
    int triggerdItemIndex;
public:
    SfsMap *map;
    // QWidget interface
protected:
    //�Ҽ�����QTreeWidgetItem, ��ʾ�˵���������set style, load sld����action), ��������޷�Ӧ
    //void mousePressEvent(QMouseEvent *event) override;�����������������ᵼ�����������¼�ʧЧ��������չ���ڵ���¼�
    //˫�������ػ棬�����������ͼ��
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    //��menu����Ӧ������QT�����װ��//�Ҽ�����QTreeWidgetItem, ��ʾ�˵���������set style, load sld����action), ��������޷�Ӧ
    void contextMenuEvent(QContextMenuEvent *event) override;
signals:
    void updateMap();//��Ҫ����glwidget ���µ�ͼ����
    void LayerZoom(SfsLayer*);
    void RemoveLayer(SfsLayer*);
};

#endif // LAYERTREE_H
