#ifndef KERNEL_SHOWWIDGET_H
#define KERNEL_SHOWWIDGET_H

#include <QWidget>
#include<SFS/sfspoint.h>
#include<qimage.h>
#include<qpainter.h>
#include<qvector.h>
#include<qcolor.h>
#include<qrect.h>

class Kernel_ShowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Kernel_ShowWidget(QWidget *parent = nullptr);
    QVector<QVector<QColor>> colors;
    QImage *image;
    void Generate_Image();
private:


signals:

public slots:


    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // KERNEL_SHOWWIDGET_H
