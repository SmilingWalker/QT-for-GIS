#include "variousDialogs.h"
#include <SLD/StringToColor.h>
#include <QByteArray>
#include <QRegExp>
#include <QMessageBox>
#include <QDebug>

SldSettingForm::SldSettingForm(OGCSld* sld) : QDialog()
{
    this->setWindowTitle(tr("设置SLD"));
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
    this->setFixedSize(450, 310);

    trigger = -1;
    bandWidth = -1;
    defaultBandWidth = -1;
    propertyIndex = -1;
    layerIndex = -1;

    layerIndexLbl = new QLabel("Layer Index:", this);
    layerIndexLbl->move(50, 40);
    layerIndexCb = new QComboBox(this);
    layerIndexCb->move(155, 40);
    int layerSize = map->layers->size();
    for(int i = 0; i < layerSize; i++){
        layerIndexCb->addItem(map->layers->at(i)->getName());
    }

    layerIndex = layerIndexCb->currentIndex();
    this->layer = map->layers->at(layerIndex);

    propertyLbl = new QLabel("Property:", this);
    propertyLbl->move(60, 120);
    propertyCb = new QComboBox(this);
    propertyCb->move(155, 120);


    Properties* property = layer->geometries->at(0)->getProperties();
    int propertySize = property->ProName->size();
    for(int i = 0; i < propertySize; i++){
        //未考虑属性类型是否合法
        propertyCb->addItem(property->ProName->at(i));
    }

    bandWidthLbl = new QLabel("Band-Width:", this);
    bandWidthLbl->move(55, 200);
    bandWidthCb = new QComboBox(this);
    bandWidthCb->move(155, 200);

    applyBtn = new QPushButton("Apply", this);
    applyBtn->move(50, 260);
    cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->move(180, 260);

    //选择图层后，触发属性添加函数
    connect(layerIndexCb, SIGNAL(currentIndexChanged(int)), this, SLOT(on_LayerIndex_Changed()));
    //点击apply按钮提交输入，然后对输入格式进行检查。如果检查通过，设置sld；如果不通过，更改输入
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
void KernelDensityForm::applyArgs(){
    accept();
}

int KernelDensityForm::getIndex() const{
    return trigger;
}

float KernelDensityForm::getBandWidth() const{
    if(bandWidth > -1.00001f && bandWidth < -0.99999f){
        return defaultBandWidth;
    }
    else{
        return bandWidth;
    }
}

float KernelDensityForm::computeDefaultBandWidth(SfsLayer* layer){

    return 0;
}

SfsLayer *KernelDensityForm::getLayer() const
{
    return layer;
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
    for(int i = 0; i < size; i++){
        //未考虑属性类型是否合法
        propertyCb->addItem(property->ProName->at(i));
    }
}
