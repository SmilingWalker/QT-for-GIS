#ifndef CONNECT_H
#define CONNECT_H

#include <QDialog>
#include<qdebug.h>
#include<mainwindow.h>
namespace Ui {
class Connect;
}

class Connect : public QDialog
{
    Q_OBJECT

public:
    explicit Connect(QWidget *parent = nullptr);
    ~Connect();
signals:
    void DBconnet(QString param,QString layerName);

private slots:
    void on_connect_clicked();

    void on_cancel_clicked();
private:
    Ui::Connect *ui;    
};

#endif // CONNECT_H
