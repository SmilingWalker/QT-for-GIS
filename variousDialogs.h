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
private:
    QLabel* strokeLbl;
    QLabel* stroke_widthLbl;
    QLabel* fillLbl;
    QLabel* sizeLbl;
    QLineEdit* strokeLet;
    QLineEdit* stroke_widthLet;
    QLineEdit* fillLet;
    QLineEdit* sizeLet;
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
    int getIndex() const;
    float getBandWidth() const;

    SfsLayer *getLayer() const;
    void setLayer(SfsLayer *value);

public slots:
    void cancelSetting();
    void on_LayerIndex_Changed();
    void applyArgs();

private:
    float computeDefaultBandWidth(SfsLayer* layer);
    SfsMap* map;
    SfsLayer* layer;
    QLabel* layerIndexLbl;
    QLabel* bandWidthLbl;
    QLabel* propertyLbl;
    QComboBox* bandWidthCb;
    QComboBox* layerIndexCb;
    QComboBox* propertyCb;
    QPushButton* applyBtn;
    QPushButton* cancelBtn;
    float defaultBandWidth;
    float bandWidth;
    int trigger;
    int propertyType;
    int propertyIndex;
    int layerIndex;
    int max;
};

#endif // SLDSETTINGFORM_H
