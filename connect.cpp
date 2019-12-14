#include "connect.h"
#include "ui_connect.h"

Connect::Connect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connect)
{
    ui->setupUi(this);
    QString k;
    connect(this,SIGNAL(DBconnet(QString ,QString )),parent,SLOT(LoadPostgreSQL(QString,QString)));
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
    QString layername = ui->layername->text();
    QString param = "PG:dbname="+dbname+" host="+host+" port="+port+" user="+user+" password="+password;
    DBconnet(param,layername);
    close();
}



void Connect::on_cancel_clicked()
{
    close();
}
