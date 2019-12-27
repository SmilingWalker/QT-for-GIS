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
    fileReader = new FileReader();
    connect(this,&MainWindow::RenderMap,ui->glwidget,&GLwidget::animate);
    connect(this,&MainWindow::SetTree,ui->layerTree,&LayerTree::AddLayer);
    connect(fileReader,&FileReader::LayerNone,this,&MainWindow::LayerNone);
    connect(ui->glwidget,SIGNAL(StatsXY(SfsPoint*,QPoint*)),this,SLOT(StatusBarXY(SfsPoint*,QPoint*)));
    connect(this,&MainWindow::clearSelect,ui->glwidget,&GLwidget::clearSelect);
    connect(this,&MainWindow::SelectionChange,ui->glwidget,&GLwidget::ChangeSelect);
    connect(ui->glwidget,&GLwidget::SetClick,this,&MainWindow::ClickSelect);
    ui->layerTree->setStyleSheet( "QTreeView::item:hover{background-color:rgb(0,255,0)}"
                                                 "QTreeView::item:selected{background-color:rgb(255,0,0)}");
    CPLSetConfigOption("GDAL_DATA","D:/gdal2.4/data");

    search = false;
    SearchTable = nullptr;
    DataBase = new ContentDB(this);//�����ı����ݿ�
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
            //��Ҫת��ΪQJsonDocument ������ByteArray
            QByteArray geoarray = geoFile.readAll();
            geoFile.close();
            QJsonDocument geojson = QJsonDocument::fromJson(geoarray);
            SfsLayer *layer = new SfsLayer();
            fileReader->GeoJsonReader(&geojson,layer);
            SetTree(layer);
            RenderMap(layer);

        }
    }

}

void MainWindow::LoadGeoJsonFile(QString filename)
{
    //�Բ�����
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
    }

}

void MainWindow::LoadShpfile(QString filename)
{
    //ע����������ȡshp�ļ���Ϊ���ݼ�
    GDALAllRegister();
    GDALDataset *pShp;
    pShp = (GDALDataset *)GDALOpenEx(filename.toLatin1().data(),GDAL_OF_VECTOR,nullptr,nullptr,nullptr);
    if(pShp==nullptr){
        QMessageBox::critical(this,"ERROR","Shpfile opens failed");
    }
    SfsLayer *layer = new SfsLayer();
    fileReader->ShpfileReader(pShp,layer);
    SetTree(layer);//����ͼ����

    //**����ͼ�㵽glwidget
    RenderMap(layer);

}

void MainWindow::LoadPostgreSQL(OGRLayer *ogrlayer)
{

    SfsLayer *layer = new SfsLayer();
    fileReader->LoadPostGIS(ogrlayer,layer);
    //����ͼ�㵽glwidget
    RenderMap(layer);
}

void MainWindow::LayerNone()
{
    QMessageBox::critical(this,"����","���ݿ�����ʧ��");
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
    retrieveNew(ui->glwidget->getMap(),query);
}

void MainWindow::ClickSelect(Metadata * meta)
{
    //���ȣ���ѡ�������֮����Ҫ���м�������������Ѿ�������������ݴ��ݲ��������ǰ������������
    if(SearchTable==nullptr){
        //���û������������н����������������,ͬʱ���������������Ϣ����Ϊtrue
        search = true;
        ui->actionSearch->setChecked(true);
        QIcon ico = QIcon("D:/QtProject/GeoJSON/icos/search.ico");
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
        connect(this,&MainWindow::ShowClick,SearchTable,&retrieveTable::ClickSelect);
    }
    //Ŀǰ�Ѿ�������������Ҫ���������ǰ���������ݣ����ҽ������ݴ��ݹ�ȥ
    ShowClick(meta);

}

void MainWindow::on_actionPostGIS_triggered()
{
    //    OGRRegisterAll();
        //ԭ����libpq����postgresql���ݿ�
    //    const char *database_info = "hostaddr=127.0.0.1 dbname=pgdemo1 user=postgres password=1998+cq+*";
    //    PGconn *pg = PQconnectdb(database_info);

        //connection string

    GDALAllRegister();
    const char* path ="PG:dbname=pgdemo1 host=localhost port=5432 user=postgres password=1998+cq+*";
    GDALDataset *pDos =nullptr;
    pDos = (GDALDataset *)GDALOpenEx(path,GDAL_OF_VECTOR,nullptr,nullptr,nullptr);
    if(pDos==nullptr)
        qDebug()<<"database open failed";
    Connect *postlink = new Connect(this);
    connect(postlink,&Connect::DBconnet,this,&MainWindow::LoadPostgreSQL);
    postlink->exec();
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
        QIcon ico = QIcon("D:/QtProject/GeoJSON/icos/search.ico");
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
    //���������ǰѡ�������
    clearSelect();
}

void MainWindow::on_QuarTree_triggered()
{
    //����Ӧ����һ��ͼ��ģ�ÿһ��ͼ�������һ����������������
    SfsMap* map = ui->glwidget->getMap();
    for(int i=0;i<map->layers->size();i++){
        SfsLayer * layer = map->layers->value(i);
        if(layer->TreeIndex!=nullptr)
            continue;
        BoundaryBox *bbox = layer->bbox;
        if(layer->TreeIndex==nullptr)
        {
            PRQuadTree *tree = new PRQuadTree(this);//�½�һ���Ĳ�������
            //�ݹ���㣬
            tree->GenerateTree(layer,*bbox);
            layer->TreeIndex = tree;//����ͼ�������� ��������
        }
    }
}


void MainWindow::on_actionSelect_triggered()
{
    SelectionChange();
}
