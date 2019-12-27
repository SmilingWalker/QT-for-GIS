#ifndef SLDSETTINGFORM_H
#define SLDSETTINGFORM_H

#include <QObject>
#include <QLabel>
#include <QColor>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <SLD/ogcsld.h>

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

#endif // SLDSETTINGFORM_H
