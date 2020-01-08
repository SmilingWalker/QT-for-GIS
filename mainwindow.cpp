#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<gdal.h>
#include "ogrsf_frmts.h"
#include<qlayout.h>
#include<QVBoxLayout>
#include<libpq-fe.h>
#include<variousDialogs.h>
#include<QTextCodec>
#include<stdlib.h>
#include<QColorDialog>
#include<qimage.h>
#include<kernel_show.h>
#include <Tool/Kernel/kerneldensity.h>

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
    connect(ui->layerTree,&LayerTree::updateMap,ui->glwidget,&GLwidget::updateMap);
    connect(ui->layerTree,&LayerTree::LayerZoom,ui->glwidget,&GLwidget::ZoomToLayer);
    connect(ui->layerTree,&LayerTree::RemoveLayer,ui->glwidget,&GLwidget::RemoveLayer);
    connect(ui->layerTree,&LayerTree::TreeIndex,ui->glwidget,&GLwidget::ShowIndex);

    CPLSetConfigOption("GDAL_DATA","D:/gdal2.4/data");
    //CPLSetConfigOption("SHAPE_ENCODING", "");//对shpfile 的字符串编码方式设置，这里要选择能够支持中文，之后才能成功分词

    search = false;
    SearchTable = nullptr;
    Selection = false;
    ui->layerTree->map = ui->glwidget->getMap();
    //分词系统连接
    QString filepath = "D:/QtProject/NLPIR-master/NLPIR SDK/NLPIR-ICTCLAS";
    NLPIR_Init(filepath.toStdString().c_str(),UTF8_CODE,"0f7977c44f2a601dffa078c14aeadbfc4ddc2990");
//    std::string string = "湖北武汉汉南疆南昌";
//    const char *str = "智能化信息处理";
//    qDebug()<<(QString::fromLocal8Bit(NLPIR_ParagraphProcess(string.data(),ICT_POS_MAP_SECOND)));
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

            QString layerName = fileName;
            layerName.chop(8);
            int index1 = layerName.lastIndexOf("/");
            int length = layerName.length();
            layerName = layerName.right(length - index1 - 1);
            QVector<SfsLayer*> *layers = new QVector<SfsLayer*>;
            fileReader->GeoJsonReader(&geojson,layers);
            for(int i=0;i<layers->size();i++){
                SfsLayer *layer = layers->at(i);
                if(layer->getGeometype()==Sfs_Point)
                    layer->setName(layerName+"_point");
                else if(layer->getGeometype()==Sfs_Polygon)
                    layer->setName(layerName+"_Polygon");
                else if(layer->getGeometype()==Sfs_LineString)
                    layer->setName(layerName+"_polyline");
                SetTree(layer);
                RenderMap(layer);
            }
        }
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

    //**传递图层到glwidget
    RenderMap(layer);

}

void MainWindow::LoadPostgreSQL(OGRLayer *ogrlayer)
{

    SfsLayer *layer = new SfsLayer();
    fileReader->LoadPostGIS(ogrlayer,layer);
    //传递图层到glwidget
    RenderMap(layer);
    ui->layerTree->AddLayer(layer);
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
    SearchTable->RetrieveRes(ui->glwidget->getMap(),query);
}

void MainWindow::ClickSelect(Metadata * meta)
{
    //首先，点选结果到了之后，需要进行激活搜索框，如果已经激活则进行数据传递并且清除当前，搜索框结果。
    if(SearchTable==nullptr){
        //如果没有搜索框则进行建立，如果有则跳过,同时将搜索框的搜索信息设置为true
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
    //目前已经有了搜索框，需要清除搜索框当前的搜索内容，并且将新数据传递过去
    ShowClick(meta);

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
    //用于清除当前选择的内容
    clearSelect();
}

void MainWindow::on_QuarTree_triggered()
{
    SfsMap *map = ui->glwidget->getMap();
    int maxLayerIndex = map->layers->size() - 1;
    if(maxLayerIndex == -1){
        QMessageBox::critical(this,"ERROR","No layer in the map!",QMessageBox::Yes);
        return;
    }
    treeIndex_Select *tree = new treeIndex_Select(nullptr,map);
    QPoint pt(this->rect().x()+this->geometry().width(),this->geometry().y()+this->geometry().height()*0.2);
    QWidget::mapToGlobal(pt);
    tree->setGeometry(pt.x(),pt.y(),this->geometry().width()*0.5,this->geometry().height()*0.5);
    connect(tree,SIGNAL(TreeIndex(SfsLayer*,int)),this,SLOT(Generate_TreeIndex(SfsLayer*,int)));
    tree->show();
}


void MainWindow::on_actionSelect_triggered()
{
    SelectionChange();
}

void MainWindow::on_GridIndex_triggered()
{
    SfsMap* map = ui->glwidget->getMap();
    qDebug()<<"createGridIndex called!"<<endl;
    int maxLayerIndex = map->layers->size() - 1;
    if(maxLayerIndex == -1){
        QMessageBox::critical(this,"ERROR","No layer in the map!",QMessageBox::Yes);
        return;
    }
    IndexSettingForm* indexSetting = new IndexSettingForm(maxLayerIndex);
    indexSetting->exec();
    int index = indexSetting->getIndex();
    int column = indexSetting->getColumn();
    int row = indexSetting->getRow();
    if(index == -1 || column == 0 || row == 0){
        QMessageBox::critical(this,"ERROR","Input incomplete!",QMessageBox::Yes);
        return;
    }
    SfsLayer* layer = map->layers->at(index);
    layer->createGridIndex(row, column);
}

void MainWindow::on_textIndex_triggered()
{
    SfsMap *map = ui->glwidget->getMap();
    for(int i=0;i<map->layers->size();i++){
        SfsLayer *layer = map->layers->value(i);
        if(layer->textDB!=nullptr)continue;
        ContentDB *textdb = new ContentDB();
        layer->textDB = textdb;
        textdb->GenerateIndex(layer);    }
}

void MainWindow::on_Kernel_caculate_triggered()
{
    if(ui->glwidget->getMap()->layers->size()>0)
    {
        KernelDensityForm* dlg = new KernelDensityForm(ui->glwidget->getMap());
        dlg->exec();
        int layerIndex = dlg->getLayerIndex();
        int propertyIndex = dlg->getPropertyIndex();
        double bandWidth = dlg->getBandWidth();
        double cellSize = dlg->getCellSize();

        KernelDensity* KDE = new KernelDensity(ui->glwidget->getMap()->layers->at(layerIndex), cellSize, propertyIndex);
        if(bandWidth == -1){
            KDE->compute(KDE->defaultBandWidth());
            }
        if(KDE!=nullptr){
            Kernel_show *window = new Kernel_show(nullptr,KDE);
            QPoint pt(this->rect().x()+this->geometry().width(),this->geometry().y()+this->geometry().height()*0.5);
            QWidget::mapToGlobal(pt);
            window->setGeometry(pt.x(),pt.y(),this->geometry().width(),this->geometry().height());
            window->show();
        }
    }
    else{
        QMessageBox::information(this,"information","No layer in map");
    }
}

void MainWindow::Generate_TreeIndex(SfsLayer *layer, int limitation)
{
        if(layer->TreeIndex!=nullptr)
        {
            QMessageBox::information(this,"information","TreeIndex exists,Generate new TreeIndex");
            delete  layer->TreeIndex;
            layer->TreeIndex=nullptr;
        }
        BoundaryBox *bbox = layer->bbox;
        if(layer->TreeIndex==nullptr)
        {
            PRQuadTree *tree = new PRQuadTree(this);//新建一个四叉树索引
            tree->setObjectsLimit(limitation);
            layer->TreeIndex = tree;//交给图层来管理 索引数据
            //递归计算，
            tree->bboxes = new QVector<BoundaryBox*>;
            tree->isroot = true;
            tree->GenerateTree(layer,*bbox);
        }

}

void MainWindow::on_LambertProj_triggered()
{
    MapLambert *Prj = new MapLambert();
    SfsMap *map = ui->glwidget->getMap();
    BoundaryBox *bbox = map->bbox;
    float left,right,top,bottom;
    float NS,NN,EW,EE;//用于传递经纬度
    NS = bbox->getBottomY();
    NN =bbox->getTopY();
    EW = bbox->getLeftX();
    EE = bbox->getRightX();
    Prj->setGraticule(NS,NN,EW,EE);
    Prj->getXY(EW,NS,&bottom,&left);
    Prj->getXY(EE,NN,&top,&right);
    map->bbox->setBoundary(top,bottom,left,right);
    SfsLayer* layer = map->layers->at(0);
    for (int i=0;i<layer->geometries->size();i++) {
        float x,y=0;
        SfsGeometry *geometry = layer->geometries->at(i);
       if(geometry->GeometryType()==Sfs_Point){
           SfsPoint *pt = (SfsPoint*)geometry;
           Prj->getXY(pt->x,pt->y,&y,&x);
           pt->x = x;
           pt->y = y;
       }
       else if(geometry->GeometryType()==Sfs_LineString){
           SfsLineString *line = (SfsLineString*)geometry;
           for(int m1=0;m1<line->pts->size();m1++){
               SfsPoint *pt = line->pts->at(m1);
               Prj->getXY(pt->x,pt->y,&y,&x);
               pt->x = x;
               pt->y = y;
           }
       }
       else if(geometry->GeometryType()==Sfs_Polygon){
           SfsPolygon *polygon = (SfsPolygon*)geometry;
           for(int m1=0;m1<polygon->boundaries->size();m1++){
               SfsLineString *line = polygon->boundaries->at(m1);
               for(int m2=0;m2<line->pts->size();m2++){
                   SfsPoint *pt = line->pts->at(m2);
                   Prj->getXY(pt->x,pt->y,&y,&x);
                   pt->x = x;
                   pt->y = y;
               }
           }
       }
   }
    map->layers->clear();
    RenderMap(layer);
}
