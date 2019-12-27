#include "connect.h"
#include "ui_connect.h"

Connect::Connect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connect)
{
    ui->setupUi(this);
    QString k;
    ui->host->setText("localhost");
    ui->user->setText("postgres");
    ui->port->setText("5432");
    ui->dbname->setText("pgdemo1");
    ui->layerBox->setEditable(false);

}

Connect::~Connect()
{
    delete ui;
}


void Connect::on_connect_clicked()
{
    QString host = ui->host->text();
    QString port = ui->port->text();
    QString user = ui->user->text();
    QString password = ui->password->text();
    QString dbname = ui->dbname->text();
    QString layername =this->ui->layerBox->currentText();
    if(layername=="")
    {
        QMessageBox::critical(this,"link error","database open failed");
        return;
    }
    QString param = "PG:dbname="+dbname+" host="+host+" port="+port+" user="+user+" password="+password;
    GDALAllRegister();
    GDALDataset *pDos =nullptr;
    try {

        //异常捕获语句
            pDos = (GDALDataset *)GDALOpenEx(param.toStdString().c_str(),GDAL_OF_VECTOR,nullptr,nullptr,nullptr);//这里开始加载数据库
            OGRLayer *ogrlayer = pDos->GetLayerByName(layername.toStdString().c_str());//图层
            DBconnet(ogrlayer);
            close();
    } catch (...) {
      QMessageBox::critical(this,"link error","database open failed");
    }
}



void Connect::on_cancel_clicked()
{
    close();
}


void Connect::on_searchLayer_clicked()
{
    QString host = ui->host->text();
    QString port = ui->port->text();
    QString user = ui->user->text();
    QString password = ui->password->text();
    QString dbname = ui->dbname->text();
    QString param = "PG:dbname="+dbname+" host="+host+" port="+port+" user="+user+" password="+password;
    //加载数据库
    GDALAllRegister();
    GDALDataset *pDos =nullptr;
    pDos = (GDALDataset *)GDALOpenEx(param.toStdString().c_str(),GDAL_OF_VECTOR,nullptr,nullptr,nullptr);//这里开始加载数据库
    if(pDos==nullptr)
        QMessageBox::critical(this,"link error","database open failed");
    else{
        for (int j=0;j<pDos->GetLayers().size();j++) {
            this->ui->layerBox->insertItem(j,QString(pDos->GetLayer(j)->GetName()));
        }
    }
}
