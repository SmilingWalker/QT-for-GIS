#include "kernel_showwidget.h"

Kernel_ShowWidget::Kernel_ShowWidget(QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet(tr("background-color: rgb(0,0,0);"));
    image = nullptr;
}

void Kernel_ShowWidget::Generate_Image()
{
    if(image!=nullptr)
    {
        delete image;
        image=nullptr;
    }
    image = new QImage(colors.first().size(),colors.size(),QImage::Format_RGB32);
    for(int i=0;i<colors.size();i++){
        for (int j=0;j<colors.first().size();j++) {
            image->setPixelColor(j,i,colors[i][j]);
        }
    }
}



void Kernel_ShowWidget::paintEvent(QPaintEvent *event)
{

    if(image!=nullptr)
    {
        QPainter painter(this);        
        QRectF source(0.0, 0.0, image->width(),image->height());
        QRect rect = this->rect();
        painter.drawImage(this->rect(),*image);
    }
}
