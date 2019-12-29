#include "sldsettingform.h"
#include <SLD/StringToColor.h>
#include <QByteArray>
#include <QRegExp>
#include <QMessageBox>
#include <QDebug>

SldSettingForm::SldSettingForm(OGCSld* sld) : QDialog()
{
    this->setWindowTitle(QString::fromLocal8Bit("设置SLD"));
    setFixedSize(400, 340);
    this->sld = sld;

    fillLbl = new QLabel(this);
    fillLbl->move(50, 80);
    fillLbl->setText("Fill: ");
    fillLet = new QLineEdit(this);
    fillLet->move(130, 80);


    strokeLbl = new QLabel(this);
    strokeLbl->move(60, 130);
    strokeLbl->setText("Stroke: ");
    strokeLet = new QLineEdit(this);
    strokeLet->move(130, 130);


    stroke_widthLbl = new QLabel(this);
    stroke_widthLbl->move(10, 180);
    stroke_widthLbl->setText("Stroke-Width: ");
    stroke_widthLet = new QLineEdit(this);
    stroke_widthLet->move(130, 180);

    sizeLbl = new QLabel(this);
    sizeLbl->move(70, 230);
    sizeLbl->setText("Size: ");
    sizeLet = new QLineEdit(this);
    sizeLet->move(130, 230);

    apply = new QPushButton("Apply",this);
    apply->move(100, 270);
    cancel = new QPushButton("Cancel", this);
    cancel->move(190, 270);
    //点击apply按钮提交输入，然后对输入格式进行检查。如果检查通过，设置sld；如果不通过，更改输入
    connect(apply, SIGNAL(clicked()), this, SLOT(check()));
    //点击cancel取消设置
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelSetting()));
}

SldSettingForm::~SldSettingForm(){
    delete fillLbl;
    delete fillLet;
    delete strokeLbl;
    delete strokeLet;
    delete stroke_widthLbl;
    delete stroke_widthLet;
    delete sizeLbl;
    delete sizeLet;
    delete apply;
    delete cancel;
}

void SldSettingForm::check(){
    //提交sld参数后, 进行检查。检查参数是否合法，确定哪些参数非空。
    //fill等四个参数是否非空的指示变量
    bool i1 = false, i2 = false, i3 = false, i4 = false;
    QString fill_str = fillLet->text();
    QString stroke_str = strokeLet->text();
    QString stroke_width_str = stroke_widthLet->text();
    QString size_str = sizeLet->text();
    //正则表达式， 匹配sld颜色格式“#nnnnnn”与浮点数格式
    QRegExp colorExp("^#([A-Fa-f0-9]{6,6})$");
    QRegExp floatExp("^(\\d+)(\\.?)(\\d?)$");
    if(fill_str != "" ){
        i1 = true; //非空则将指示变量设为true
        if(colorExp.indexIn(fill_str)== -1){
            QMessageBox::warning(this, tr("Warning!"),tr("\"Fill\" format error!"), QMessageBox::Yes);
            fillLet->clear();
            fillLet->setFocus();
            return;
        }
    }
    if(stroke_str != "" ){
        i2 = true;
        if(colorExp.indexIn(stroke_str)== -1){
            QMessageBox::warning(this, tr("Warning!"),tr("\"Stroke\" format error!"), QMessageBox::Yes);
            strokeLet->clear();
            strokeLet->setFocus();
            return;
        }
    }
    if(stroke_width_str != ""){
        i3 = true;
        if(floatExp.indexIn(stroke_width_str)== -1){
            QMessageBox::warning(this, tr("Warning!"),tr("\"Stroke-Width\" format error!"), QMessageBox::Yes);
            stroke_widthLet->clear();
            stroke_widthLet->setFocus();
            return;
        }
    }
    if(size_str != ""){
        i4 = true;
        if(floatExp.indexIn(size_str) == -1){
            QMessageBox::warning(this, tr("Warning!"),tr("\"Size\" format error!"),QMessageBox::Yes);
            sizeLet->clear();
            sizeLet->setFocus();
            return;
        }
    }
    applyArgs(i1, i2, i3, i4);
}

void SldSettingForm::applyArgs(bool i1, bool i2, bool i3, bool i4){
    //根据传入的非空指示变量设置sld参数
    if(i1){
        //stringToColor(QString) 将sld颜色格式文本转化为QColor，见StringToColor.cpp/.h
        QColor fill = stringToColor(fillLet->text());
        sld->setFill(fill);
    }
    if(i2){
        QColor stroke = stringToColor(strokeLet->text());
        sld->setStroke(stroke);
    }
    if(i3){
        QString stroke_width_str = stroke_widthLet->text();
        QByteArray stroke_width_ba = stroke_width_str.toLatin1();
        char* stroke_width_c = stroke_width_ba.data();
        char* end;
        float stroke_width = strtof(stroke_width_c,&end);
        sld->setStroke_width(stroke_width);
    }
    if(i4){
        QString size_str = sizeLet->text();
        QByteArray size_ba = size_str.toLatin1();
        char* size_c = size_ba.data();
        char* end;
        float size = strtof(size_c, &end);
        sld->setSize(size);
    }
    accept();
}

void SldSettingForm::cancelSetting(){
    reject();
}
