﻿#include "variousDialogs.h"
#include <SLD/StringToColor.h>
#include <QByteArray>
#include <QRegExp>
#include <QMessageBox>
#include <QDebug>
#include <QColorDialog>

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
    fillColorSelection = new QPushButton(this);
    fillColorSelection->move(320, 83);
    fillColorSelection->resize(25,20);
    //QString fillColor = QString("");
    fillColorSelection->setStyleSheet(QString("QPushButton{background-color:rgb(122, 122, 122)}"));


    strokeLbl = new QLabel(this);
    strokeLbl->move(60, 130);
    strokeLbl->setText("Stroke: ");
    strokeLet = new QLineEdit(this);
    strokeLet->move(130, 130);
    strokeColorSelection = new QPushButton(this);
    strokeColorSelection->move(320, 133);
    strokeColorSelection->resize(25,20);
    strokeColorSelection->setStyleSheet(QString("QPushButton{background-color:rgb(122, 122, 122)}"));

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
    cancel->move(220, 270);
    //点击apply按钮提交输入，然后对输入格式进行检查。如果检查通过，设置sld；如果不通过，更改输入
    connect(apply, SIGNAL(clicked()), this, SLOT(check()));
    //点击cancel取消设置
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelSetting()));
    //点击颜色方框按钮，显示QColorDialog
    connect(fillColorSelection, SIGNAL(clicked()), this, SLOT(on_FillColorSelection_Triggered()));
    connect(strokeColorSelection, SIGNAL(clicked()), this, SLOT(on_StrokeColorSelection_Triggered()));
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
    delete fillColorSelection;
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

void SldSettingForm::on_FillColorSelection_Triggered()
{
    QColor color = QColorDialog::getColor();
    QString colorHtml = color.name();
    int r, g, b;
    color.getRgb(&r, &g, &b);
    QString colorRGB = QString("QPushButton{background-color:rgb(%1, %2, %3)}").arg(r).arg(g).arg(b);
    //sld->setFill(color);
    fillLet->setText(colorHtml);
    fillColorSelection->setStyleSheet(colorRGB);
}

void SldSettingForm::on_StrokeColorSelection_Triggered()
{
    QColor color = QColorDialog::getColor();
    QString colorHtml = color.name();
    int r, g, b;
    color.getRgb(&r, &g, &b);
    QString colorRGB = QString("QPushButton{background-color:rgb(%1, %2, %3)}").arg(r).arg(g).arg(b);
    //sld->setStroke(color);
    strokeLet->setText(colorHtml);
    strokeColorSelection->setStyleSheet(colorRGB);
}

void SldSettingForm::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}

IndexSettingForm::IndexSettingForm(int max){
    this->setWindowTitle(tr("Index Setting"));
    setFixedSize(350, 210);

    layerIndexLbl = new QLabel("Index:", this);
    layerIndexLbl->move(40,35);
    layerIndexLet = new QLineEdit(this);
    layerIndexLet->move(130,35);

    rowLbl = new QLabel("Row:", this);
    rowLbl->move(50, 75);
    rowLet = new QLineEdit(this);
    rowLet->move(130, 75);

    columnLbl = new QLabel("Column:", this);
    columnLbl->move(30, 115);
    columnLet = new QLineEdit(this);
    columnLet->move(130, 115);

    applyBtn = new QPushButton("apply",this);
    applyBtn->move(50, 160);
    cancelBtn = new QPushButton("cancel", this);
    cancelBtn->move(180, 160);

    trigger = -1;
    this->max = max;

    //点击apply触发参数检查函数
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(check()));
    //点击cancel取消设置
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(cancelSetting()));
}

int IndexSettingForm::getIndex(){
    //返回用户输入的参数
    return trigger;
}

void IndexSettingForm::check(){
    QRegExp integer("^(\\d+)$");
    QString input = layerIndexLet->text();
    if(integer.indexIn(input) == -1){
       QMessageBox::warning(this, tr("Warning!"),tr("Index invalid!"), QMessageBox::Yes);
       return;
    }
    int input_ = input.toInt();
    if(input_ > max){
        QMessageBox::warning(this, tr("Warning!"),tr("Index too large!"), QMessageBox::Yes);
        return;
    }
    QString column = columnLet->text();
    if(integer.indexIn(column) == -1 ||column.toInt() == 0){
        QMessageBox::warning(this, tr("Warning!"),tr("Column invalid!"), QMessageBox::Yes);
        return;
    }
    QString row = rowLet->text();
    if(integer.indexIn(row) == -1 || row.toInt() == 0){
        QMessageBox::warning(this, tr("Warning!"),tr("Row invalid!"), QMessageBox::Yes);
        return;
    }
    //通过参数检查，则设置参数
    applyArgs();
    accept();
}

void IndexSettingForm::cancelSetting(){
    reject();
}
void IndexSettingForm::applyArgs(){
    QString inputStr = layerIndexLet->text();
    trigger = inputStr.toInt();
    QString columnStr = columnLet->text();
    column = columnStr.toInt();
    QString rowStr = rowLet->text();
    row = rowStr.toInt();
}
int IndexSettingForm::getRow() const
{
    return row;
}

int IndexSettingForm::getColumn() const
{
    return column;
}

KernelDensityForm::KernelDensityForm(SfsMap* map){

    this->map = map;
    this->max = map->layers->size() - 1;
    layer = nullptr;

    this->setWindowTitle("Kernel Density Estimation");
    this->setFixedSize(450, 380);

    bandWidth = -1;
    propertyIndex = -1;
    layerIndex = -1;
    cellSize = -1;

    layerIndexLbl = new QLabel("Layer Index:", this);
    layerIndexLbl->move(50, 40);
    layerIndexCb = new QComboBox(this);
    layerIndexCb->move(155, 40);
    int layerSize = map->layers->size();
    for(int i = 0; i < layerSize; i++){
        layerIndexCb->addItem(map->layers->at(i)->getName());
    }
    layerIndexCb->setCurrentIndex(-1);


    propertyLbl = new QLabel("Property:", this);
    propertyLbl->move(60, 120);
    propertyCb = new QComboBox(this);
    propertyCb->move(155, 120);
    propertyCb->setCurrentIndex(-1);


    /*Properties* property = layer->geometries->at(0)->getProperties();
    int propertySize = property->ProName->size();
    for(int i = 0; i < propertySize; i++){
        //未考虑属性类型是否合法
        propertyCb->addItem(property->ProName->at(i));
    }*/

    bandWidthLbl = new QLabel("Band-Width:", this);
    bandWidthLbl->move(55, 200);
    bandWidthCb = new QComboBox(this);
    bandWidthCb->move(155, 200);
    bandWidthCb->setEditable(true);
    bandWidthCb->addItem("Default");
    bandWidthCb->setCurrentIndex(-1);

    cellSizeLbl = new QLabel("Cell Size:", this);
    cellSizeLbl->move(55, 280);
    cellSizeLet = new QLineEdit(this);
    cellSizeLet->move(155, 280);

    applyBtn = new QPushButton("Apply", this);
    applyBtn->move(50, 340);
    cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->move(180, 340);

    //选择图层后，触发属性添加函数
    connect(layerIndexCb, SIGNAL(currentIndexChanged(int)), this, SLOT(on_LayerIndex_Changed()));
    //选择属性后，触发属性选择和检查函数
    connect(propertyCb, SIGNAL(currentIndexChanged(int)), this, SLOT(on_PropertyIndex_Changed()));
    //点击apply按钮提交输入
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(applyArgs()));
    //点击cancel取消设置
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(cancelSetting()));
}
KernelDensityForm::~KernelDensityForm(){
    layer = nullptr;
    map = nullptr;
    delete layerIndexLbl;
    delete bandWidthLbl;
    delete propertyLbl;
    delete bandWidthCb;
    delete layerIndexCb;
    delete propertyCb;
    delete applyBtn;
    delete cancelBtn;
}

int KernelDensityForm::getPropertyIndex() const
{
    return propertyIndex;
}
void KernelDensityForm::applyArgs(){
    //检查输入合法性
    if(bandWidthCb->currentIndex()!= -1){
        QString text = bandWidthCb->currentText();
        bool ok;
        double bandWidth = text.toDouble(&ok);
        if(ok){
            this->bandWidth = bandWidth;
        }
        else if(text == "Default"){
            this->bandWidth = -1;
        }
        else{
            QMessageBox::warning(this, tr("Warning!"),tr("Bandwidth invalid!"), QMessageBox::Yes);
            return;
        }
    }
    else if(bandWidthCb->currentIndex()== -1){
        this->bandWidth = -1;
    }
    Properties* property = layer->geometries->at(0)->getProperties();
    if(property->getProprtyTypeAt(propertyIndex)!= Double_PRO){
        propertyIndex = -1;
        QMessageBox::warning(this, tr("Warning!"),tr("Property is not double!"), QMessageBox::Yes);
        return;
    }
    QString cellSizeTxt;
    cellSizeTxt = cellSizeLet->text();
    if(layerIndex == -1 || propertyIndex == -1 || cellSizeTxt == ""){
        QMessageBox::warning(this, tr("Warning!"),tr("Input incomplete!"), QMessageBox::Yes);
        return;
    }
    bool ok;
    cellSize = cellSizeTxt.toDouble(&ok);
    if(!ok){
        cellSize = -1;
        QMessageBox::warning(this, tr("Warning!"),tr("Cell Size illegal!"), QMessageBox::Yes);
        return;
    }

    accept();
}


int KernelDensityForm::getLayerIndex() const{
    return layerIndex;
}

double KernelDensityForm::getCellSize() const
{
    return cellSize;
}

float KernelDensityForm::getBandWidth() const{
    return bandWidth;
}

void KernelDensityForm::setLayer(SfsLayer *value)
{
    layer = value;
}

void KernelDensityForm::cancelSetting(){
    reject();
}

void KernelDensityForm::on_LayerIndex_Changed()
{
    layerIndex = layerIndexCb->currentIndex();
    this->layer = map->layers->at(layerIndex);
    Properties* property = layer->geometries->at(0)->getProperties();
    int size = property->ProName->size();

    propertyCb->clear();
    isDouble.clear();
    for(int i = 0; i < size; i++){
        //未考虑属性类型是否合法
        if(property->getProprtyTypeAt(i)!= Double_PRO){
            isDouble.append(false);
        }
        else{
            propertyCb->addItem(property->ProName->at(i));
            isDouble.append(true);
        }
    }
}

void KernelDensityForm::on_PropertyIndex_Changed()
{
    int currentIndex = propertyCb->currentIndex();
    if(currentIndex != -1){
        Properties* property = layer->geometries->at(0)->getProperties();
        int propertySize = property->ProName->size();
        int listIndex = -1;
        int i;
        for(i = 0; i < propertySize; i++){
            if(listIndex < currentIndex){
                if(property->getProprtyTypeAt(i) == Double_PRO){
                    listIndex++;
                }
            }
            else{
                break;
            }
        }
        propertyIndex = i-1;
        if(property->getProprtyTypeAt(propertyIndex)!= Double_PRO){
            propertyIndex = -1;
            QMessageBox::warning(this, tr("Warning!"),tr("Property is not double!"), QMessageBox::Yes);
            return;
        }
    }
    else{
        qDebug()<<"Why it happens?"<<endl;
    }
}
