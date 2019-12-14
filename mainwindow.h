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
#include<sfsgeometry.h>
#include<sfslayer.h>
#include<sfsmap.h>
#include<sfsgeometrycollection.h>
#include<sfslinestring.h>
#include<sfsmultilinestring.h>
#include<sfsmultipoint.h>
#include<sfsmultipoint.h>
#include<sfsmultipolygon.h>
#include<sfspoint.h>
#include<sfspolygon.h>
#include<sfsrender.h>
#include<float.h>
#include<ogr_libs.h>
#include<filereader.h>
#include<ui_ConnectToPostGIS.h>
#include<connect.h>
#include<layertree.h>

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

private:
    Ui::MainWindow *ui;
    SfsMap *map;
    QVector<SfsRender*> render;
public:
    FileReader *fileReader;
signals:
    void RenderMap(SfsRender * render);
    void SetTree(SfsLayer *layer);
public slots:
    void LoadPostgreSQL(QString filename,QString layerName);
    void LayerNone();
    void StatusBarXY(SfsPoint* s_pt,QPoint* q_pt);


};
#endif // MAINWINDOW_H
