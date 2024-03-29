#ifndef SLDSETTINGFORM_H
#define SLDSETTINGFORM_H

#include <QObject>
#include <QLabel>
#include <QColor>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <SLD/ogcsld.h>
#include <SFS/sfslayer.h>
#include <QComboBox>
#include <SFS/sfsmap.h>
#include <QPainter>
#include <QStyleOption>

class SldSettingForm : public QDialog
{
    Q_OBJECT
    //继承自QDialog
public:
    //初始化时传入sld
    explicit SldSettingForm(OGCSld* sld);
    ~SldSettingForm();
signals:

public slots:
    void check();
    void cancelSetting();
    void on_FillColorSelection_Triggered();
    void on_StrokeColorSelection_Triggered();

private:
    void paintEvent(QPaintEvent *e);
    QLabel* strokeLbl;
    QLabel* stroke_widthLbl;
    QLabel* fillLbl;
    QLabel* sizeLbl;
    QLineEdit* strokeLet;
    QLineEdit* stroke_widthLet;
    QLineEdit* fillLet;
    QLineEdit* sizeLet;
    QPushButton* fillColorSelection;
    QPushButton* strokeColorSelection;
    QPushButton* apply;
    QPushButton* cancel;
    void applyArgs(bool i1, bool i2, bool i3, bool i4);
    //每个对话框有一个sld指针，方便设置所选图层的sld
    OGCSld* sld;
};

class IndexSettingForm: public QDialog{
    Q_OBJECT
public:
    explicit IndexSettingForm(int max);
    int getIndex();
    int getColumn() const;
    int getRow() const;

public slots:
    void check();
    void cancelSetting();
private:
    QLabel* layerIndexLbl;
    QLabel* columnLbl;
    QLabel* rowLbl;
    QLineEdit* columnLet;
    QLineEdit* rowLet;
    QLineEdit* layerIndexLet;
    QPushButton* applyBtn;
    QPushButton* cancelBtn;
    int trigger = -1;
    int column;
    int row = 0;
    int max = 0;
    void applyArgs();
};

class KernelDensityForm: public QDialog{
    Q_OBJECT
public:
    KernelDensityForm(SfsMap* map);
    ~KernelDensityForm();
    int getPropertyIndex() const;
    float getBandWidth() const;
    int getLayerIndex() const;
    double getCellSize() const;
    void setLayer(SfsLayer *value);

public slots:
    void cancelSetting();
    void on_LayerIndex_Changed();
    void on_PropertyIndex_Changed();
    void applyArgs();

private:
    SfsMap* map;
    SfsLayer* layer;
    QLabel* layerIndexLbl;
    QLabel* bandWidthLbl;
    QLabel* propertyLbl;
    QLabel* cellSizeLbl;
    QLabel* savePathLbl;
    QComboBox* bandWidthCb;
    QComboBox* layerIndexCb;
    QComboBox* propertyCb;
    QLineEdit* cellSizeLet;
    //QLineEdit* savePathLet;
    QPushButton* applyBtn;
    QPushButton* cancelBtn;
    float defaultBandWidth;
    float bandWidth;
    int trigger;
    int propertyType;
    int propertyIndex;
    int layerIndex;
    int max;
    double cellSize;
    //记录选中图层每个属性是否为double类型
    QVector<bool> isDouble;
};

#endif // SLDSETTINGFORM_H
