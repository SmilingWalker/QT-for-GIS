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
    connect(this,&MainWindow::clearSelect,ui->glwidget,&GLwidget::clearSelect);
    connect(this,&MainWindow::SelectionChange,ui->glwidget,&GLwidget::ChangeSelect);
    ui->layerTree->setStyleSheet( "QTreeView::item:hover{background-color:rgb(0,255,0)}"
                                                 "QTreeView::item:selected{background-color:rgb(255,0,0)}");
    CPLSetConfigOption("GDAL_DATA","D:/gdal2.4/data");


    map->bbox->setBoundary(DBL_MIN,DBL_MAX,DBL_MAX,DBL_MIN);
    search = false;
    SearchTable = nullptr;
    DataBase = new ContentDB(this);//建立文本数据库
    Selection = false;
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

void MainWindow::retrieve()
{
    QString query = searchEdit->text();
    connect(this,&MainWindow::retrieveNew,SearchTable,&retrieveTable::RetrieveRes);
    retrieveNew(map,query);
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

void MainWindow::on_actionSearch_triggered()
{
//    if(searchEdit!=nullptr)
//    {

//        searchEdit->hide();
//        delete searchEdit;
//        searchEdit = nullptr;
//    }
//    else{
//        QRect rect =  ui->toolBar->geometry();
//        searchEdit = new searchWidget(this,QRect(rect.x()+rect.width()-150,rect.y(),150,rect.height()));
//        searchEdit->show();
//    }
//    searchEdit->show();
//    searchButton->show();
    if(!search)
    {
        QIcon ico = QIcon("D:/QtProject/GeoJSON/search.ico");
        searchEdit = new QLineEdit(this);
        searchButton = new QPushButton(this);
        QRect rect =  ui->toolBar->geometry();
        searchEdit->setGeometry(rect.x()+rect.width()*0.7,rect.y(),rect.width()*0.3,rect.height());
        searchButton->setGeometry(rect.x()+rect.width()*0.95,rect.y(),rect.width()*0.05,rect.height());
        searchButton->setIcon(ico);
        searchButton->show();
        searchEdit->show();
        SearchTable = new retrieveTable(this);
        SearchTable->setGeometry(rect.x()+rect.width()*0.7,rect.y()+rect.height(),rect.width()*0.3,rect.height()*5);
        connect(searchButton,&QPushButton::clicked,this,&MainWindow::retrieve);
        connect(SearchTable,SIGNAL(RetrievePaint(QVector<Metadata*>,QVector<Metadata*>)),ui->glwidget,SLOT(RetrievePaint(QVector<Metadata*>,QVector<Metadata*>)));
        //connect(SearchTable,&retrieveTable::RetrievePaint,ui->glwidget,&GLwidget::RetrievePaint);
        search = true;
    }
    else{
        delete  searchEdit;
        searchEdit = nullptr;
        delete searchButton;
        searchButton = nullptr;
        delete SearchTable;
        SearchTable = false;
        search = false;
    }
}

void MainWindow::on_actionClear_triggered()
{
    //用于清除当前选择的内容
    clearSelect();
}

void MainWindow::on_QuarTree_triggered()
{
    //索引应该是一个图层的，每一个图层可以有一个索引
    for(int i=0;i<map->layers->size();i++){
        SfsLayer * layer = map->layers->value(i);
        BoundaryBox *bbox = layer->bbox;
        if(layer->TreeIndex==nullptr)
        {
            PRQuadTree *tree = new PRQuadTree(this);//新建一个四叉树索引
            //递归计算，
            tree->GenerateTree(layer,*bbox);

            layer->TreeIndex = tree;//交给图层来管理 索引数据
        }
    }
}


void MainWindow::on_actionSelect_triggered()
{
    SelectionChange();
}
