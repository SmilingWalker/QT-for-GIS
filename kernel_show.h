#ifndef KERNEL_SHOW_H
#define KERNEL_SHOW_H

#include <QWidget>
#include<qcolordialog.h>
#include<qimage.h>
#include<QColor>
#include<Tool/Kernel/kerneldensity.h>
#include<qvector.h>
#include<qdebug.h>

namespace Ui {
class Kernel_show;
}

class Kernel_show : public QWidget
{
    Q_OBJECT

public:
    explicit Kernel_show(QWidget *parent = nullptr,KernelDensity* KDE=nullptr);
    ~Kernel_show();
    KernelDensity *getKDE() const;
    void setKDE(KernelDensity *value);
    QVector<QVector<QColor>> colors;
    KernelDensity* KDE;



private slots:
    void on_setBegin_clicked();
    void on_Set_End_clicked();
    void on_classify_show_clicked();

    void on_save_clicked();

private:
    bool first;
    void Add_Classification();
    void Normalization();
    void GetColors();
    Ui::Kernel_show *ui;
    QColor beginColor;
    QColor EndColor;
    QVector<QVector<int>> data;
    int classfication;

};

#endif // KERNEL_SHOW_H
