#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QFileDialog>
#include<qjsonarray.h>
#include<qjsondocument.h>
#include<qmessagebox.h>
#include<qdebug.h>
#include<qjsonobject.h>
#include<qjsonvalue.h>
#include<ogrsf_frmts.h>
#include<gdal.h>
#include<iostream>
#include<string.h>
#include<SFS/sfsgeometry.h>
#include<SFS/sfslayer.h>
#include<SFS/sfsmap.h>
#include<SFS/sfsgeometrycollection.h>
#include<SFS/sfslinestring.h>
#include<SFS/sfsmultilinestring.h>
#include<SFS/sfsmultipoint.h>
#include<SFS/sfsmultipoint.h>
#include<SFS/sfsmultipolygon.h>
#include<SFS/sfspoint.h>
#include<SFS/sfspolygon.h>
#include<SFS/sfsrender.h>
#include<float.h>
#include<ogr_libs.h>
#include<filereader.h>
#include<ui_ConnectToPostGIS.h>
#include<connect.h>
#include<layertree.h>
#include<QLineEdit>
#include<qpushbutton.h>
#include<contentdb.h>
#include<qtablewidget.h>
#include<retrievetable.h>
#include<prquadtree.h>
#include<QTreeWidget>
#include<QTreeWidgetItem>
#include<QAction>
#include<QMenu>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void LoadGeoJsonFile(QString filename);
    void LoadShpfile(QString filename);

private slots:
    void on_actionGeoJSON_triggered();
    void on_actionShapfile_triggered();
    void on_actionPostGIS_triggered();
    void on_actionSearch_triggered();
    void on_actionClear_triggered();
    void on_QuarTree_triggered();
    void on_actionSelect_triggered();

private:
    Ui::MainWindow *ui;
    QVector<SfsRender*> render;
    QLineEdit *searchEdit;
    QPushButton *searchButton;
    retrieveTable *SearchTable;
    bool search;


public:
    FileReader *fileReader;
    ContentDB *DataBase;
    QTableWidget * searchRes;//检索结果返回，显示
    PRQuadTree *QuarTree;
    bool Selection;
signals:
    void SelectionChange();
    void RenderMap(SfsLayer *layer);
    void SetTree(SfsLayer *layer);
    void retrieveNew(SfsMap *map,QString query);
    void clearSelect();
    void ChangeSelect(bool selection);
    void ShowClick(Metadata * meta);

public slots:
    void LoadPostgreSQL(OGRLayer *layer);
    void LayerNone();
    void StatusBarXY(SfsPoint* s_pt,QPoint* q_pt);
    void retrieve();
    void ClickSelect(Metadata * meta);//点选结果只能有一个，所以传递的数据只是一个元数据类型


};
#endif // MAINWINDOW_H
