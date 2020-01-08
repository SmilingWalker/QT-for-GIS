#include "kernel_show.h"
#include "ui_kernel_show.h"


Kernel_show::Kernel_show(QWidget *parent,KernelDensity* KDE) :
    QWidget(parent),
    ui(new Ui::Kernel_show)
{
    ui->setupUi(this);
    ui->BeginColor->setStyleSheet(tr("background-color: rgb(0,0,0);"));
    beginColor = QColor(0,0,0);
    ui->End_Color->setStyleSheet(tr("background-color: rgb(255,255,255);"));
    EndColor = QColor(255,255,255);
    ui->Kernel_Show->setStyleSheet(tr("background-color: rgb(0,0,0);"));
    classfication = 5;
    first = true;
    this->KDE =KDE;
    Add_Classification();
    if(KDE!=nullptr)
    {
        Normalization();
        GetColors();
        ui->Kernel_Show->colors = colors;
        ui->Kernel_Show->Generate_Image();
    }
}

Kernel_show::~Kernel_show()
{
    delete ui;
}

void Kernel_show::on_setBegin_clicked()
{
    //起始颜色配置
    //弹出颜色对话框
    beginColor = QColorDialog::getColor();
    ui->BeginColor->setStyleSheet(tr("background-color:%1").arg(beginColor.name()));

}

void Kernel_show::on_Set_End_clicked()
{
    //结束颜色配置
    //设置背景色
    EndColor = QColorDialog::getColor();
    ui->End_Color->setStyleSheet(tr("background-color:%1").arg(EndColor.name()));
}

void Kernel_show::Add_Classification()
{
    for(int i=1;i<15;i++){
        ui->classify->insertItem(i,QString::number(i));
    }
    ui->classify->setCurrentText("5");
}

KernelDensity *Kernel_show::getKDE() const
{
    return KDE;

}

void Kernel_show::setKDE(KernelDensity *value)
{
    KDE = value;
    Normalization();
}

void Kernel_show::Normalization()
{
    for(int i=0;i<KDE->getRow();i++){
        QVector<int> row;
        for(int j=0;j<KDE->getColumn();j++){
            double c=0;
            for(int p=0;p<KDE->pixels[i][j].getProperties()->ProName->size();p++){
                if(KDE->pixels[i][j].getProperties()->ProName->at(p).contains("density"))
                {
                    c = (*(double*)KDE->pixels[i][j].getProperties()->ProValue->at(p));
                    break;
                }
            }
            c = ((c-KDE->getDensityMin())/(KDE->getDensityMax()-KDE->getDensityMin()))*255;
            row.append(int(c));
        }
        data.append(row);
    }
}

void Kernel_show::GetColors()
{
    colors.clear();
    if(KDE==nullptr)
        return;
    int R_interval = (EndColor.red()-beginColor.red())/(classfication-1);
    int G_interval = (EndColor.green()-beginColor.green())/(classfication-1);
    int B_interval = (EndColor.blue()-beginColor.blue())/(classfication-1);
    for(int i=0;i<KDE->getRow();i++){
        QVector<QColor> row;
        for(int j=0;j<KDE->getColumn();j++){
            QColor color;
            if(classfication==1)
                color = beginColor;
            else{
                int lei = ((data[i][j])/( 255 / (classfication-1))+1);
                int R,G,B;
                R = lei*R_interval+beginColor.red();
                G = lei*G_interval+beginColor.green();
                B = lei*B_interval+beginColor.blue();
                color.setRgb(R,G,B);
            }
            row.append(color);
        }
        colors.append(row);
    }
}

void Kernel_show::on_classify_show_clicked()
{
    classfication = ui->classify->currentText().toInt();
    GetColors();
    ui->Kernel_Show->colors.clear();
    ui->Kernel_Show->colors = colors;
    ui->Kernel_Show->Generate_Image();
    ui->Kernel_Show->update();
}

void Kernel_show::on_save_clicked()
{
    ui->Kernel_Show->image->save(QString(KDE->getLayer()->getName())+"Kernel.BMP","BMP",100);
}
