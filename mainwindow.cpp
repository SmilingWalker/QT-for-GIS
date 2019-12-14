#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<gdal.h>
#include "ogrsf_frmts.h"
#include<qlayout.h>
#include<QVBoxLayout>
#include<libpq-fe.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    map = new SfsMap();
    fileReader = new FileReader();
    connect(this,&MainWindow::RenderMap,ui->glwidget,&GLwidget::animate);
    connect(this,&MainWindow::SetTree,ui->layerTree,&LayerTree::AddLayer);
    connect(fileReader,&FileReader::LayerNone,this,&MainWindow::LayerNone);
    connect(ui->glwidget,SIGNAL(StatsXY(SfsPoint*,QPoint*)),this,SLOT(StatusBarXY(SfsPoint*,QPoint*)));
    ui->layerTree->setStyleSheet( "QTreeView::item:hover{background-color:rgb(0,255,0)}"
                                                 "QTreeView::item:selected{background-color:rgb(255,0,0)}");
    CPLSetConfigOption("GDAL_DATA","D:/gdal2.4/data");
    map->bbox->setBoundary(DBL_MIN,DBL_MAX,DBL_MAX,DBL_MIN);


}

MainWindow::~MainWindow()
{
    delete ui;
    if(fileReader!=nullptr)
        delete fileReader;
}

void MainWindow::on_actionGeoJSON_triggered()
{
    //open GeoJSON file

    QString fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        if(fileName.right(8)!=".geojson")
        QMessageBox::critical(this,"ERROR","File is not geojson file");
        else {
            QFile geoFile(fileName);
            geoFile.open(QIODevice::ReadOnly);
            //需要转换为QJsonDocument 先生成ByteArray
            QByteArray geoarray = geoFile.readAll();
            geoFile.close();
            QJsonDocument geojson = QJsonDocument::fromJson(geoarray);
            SfsLayer *layer = new SfsLayer();
            map->layers->append(layer);
            fileReader->GeoJsonReader(&geojson,layer);
            SetTree(layer);
            //map的范围更新
            map->bbox->setTopY(map->bbox->getTopY()>layer->bbox->getTopY()?map->bbox->getTopY():layer->bbox->getTopY());
            map->bbox->setBottomY(map->bbox->getBottomY()<layer->bbox->getBottomY()?map->bbox->getBottomY():layer->bbox->getBottomY());
            map->bbox->setLeftX(map->bbox->getLeftX()<layer->bbox->getLeftX()?map->bbox->getLeftX():layer->bbox->getLeftX());
            map->bbox->setRightX(map->bbox->getRightX()>layer->bbox->getRightX()?map->bbox->getRightX():layer->bbox->getRightX());
        }

        SfsRender *rd = new SfsRender(map);
        render.append(rd);
        RenderMap(render.last());
    }

}
void MainWindow::LoadGeoJsonFile(QString filename)
{
    //以不复用
}

void MainWindow::on_actionShapfile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        if(fileName.right(4)!=".shp")
        QMessageBox::critical(this,"ERROR","File is not a shpfile");
        else {
            LoadShpfile(fileName);
        }
        SfsRender *rd = new SfsRender(map);
        render.append(rd);
        RenderMap(render.last());
    }

}

void MainWindow::LoadShpfile(QString filename)
{
    //注册驱动，读取shp文件作为数据集
    GDALAllRegister();
    GDALDataset *pShp;
    pShp = (GDALDataset *)GDALOpenEx(filename.toLatin1().data(),GDAL_OF_VECTOR,nullptr,nullptr,nullptr);
    if(pShp==nullptr){
        QMessageBox::critical(this,"ERROR","Shpfile opens failed");
    }
    SfsLayer *layer = new SfsLayer();
    fileReader->ShpfileReader(pShp,layer);
    SetTree(layer);//设置图层树
    //map的范围更新
    map->bbox->setTopY(map->bbox->getTopY()>layer->bbox->getTopY()?map->bbox->getTopY():layer->bbox->getTopY());
    map->bbox->setBottomY(map->bbox->getBottomY()<layer->bbox->getBottomY()?map->bbox->getBottomY():layer->bbox->getBottomY());
    map->bbox->setLeftX(map->bbox->getLeftX()<layer->bbox->getLeftX()?map->bbox->getLeftX():layer->bbox->getLeftX());
    map->bbox->setRightX(map->bbox->getRightX()>layer->bbox->getRightX()?map->bbox->getRightX():layer->bbox->getRightX());
    map->layers->append(layer);

}

void MainWindow::LoadPostgreSQL(QString param,QString layerName)
{
    //加载数据库
    GDALAllRegister();
    GDALDataset *pDos =nullptr;
    qDebug()<<layerName;
    pDos = (GDALDataset *)GDALOpenEx(param.toStdString().c_str(),GDAL_OF_VECTOR,nullptr,nullptr,nullptr);
    if(pDos==nullptr)
        qDebug()<<"database open failed";
    else
        {
        SfsLayer *layer = new SfsLayer();
        map->layers->append(layer);
        //map的范围更新
        map->bbox->setTopY(map->bbox->getTopY()>layer->bbox->getTopY()?map->bbox->getTopY():layer->bbox->getTopY());
        map->bbox->setBottomY(map->bbox->getBottomY()<layer->bbox->getBottomY()?map->bbox->getBottomY():layer->bbox->getBottomY());
        map->bbox->setLeftX(map->bbox->getLeftX()<layer->bbox->getLeftX()?map->bbox->getLeftX():layer->bbox->getLeftX());
        map->bbox->setRightX(map->bbox->getRightX()>layer->bbox->getRightX()?map->bbox->getRightX():layer->bbox->getRightX());


        fileReader->LoadPostGIS(pDos,layer,layerName);
    }
}

void MainWindow::LayerNone()
{
    QMessageBox::critical(this,"错误","数据库连接失败");
}

void MainWindow::StatusBarXY(SfsPoint* s_pt, QPoint* q_pt)
{
    QStatusBar *bar = statusBar();
    bar->showMessage("View X: "+QString::number(q_pt->x(), 10)+"  Y: "+QString::number(q_pt->y(), 10)+"  World X: "+QString::number(s_pt->x, 10,4)+"  Y: "+QString::number(s_pt->y, 10,4),3000);
}


void MainWindow::on_actionPostGIS_triggered()
{
    //    OGRRegisterAll();
        //原生了libpq连接postgresql数据库
    //    const char *database_info = "hostaddr=127.0.0.1 dbname=pgdemo1 user=postgres password=1998+cq+*";
    //    PGconn *pg = PQconnectdb(database_info);

        //connection string

    GDALAllRegister();
    const char* path ="PG:dbname=pgdemo1 host=localhost port=5432 user=postgres password=1998+cq+*";
    GDALDataset *pDos =nullptr;
    pDos = (GDALDataset *)GDALOpenEx(path,GDAL_OF_VECTOR,nullptr,nullptr,nullptr);
    if(pDos==nullptr)
        qDebug()<<"database open failed";
    Connect *gis = new Connect(this);
    gis->exec();
}
