#include "ogcsld.h"

OGCSld::OGCSld(QObject *parent) : QObject(parent)
{
    setFill(QColor(120,120,120));//Ìî³ä
    setStroke(QColor(120,120,120));//ÏßÌî³ä
    setSize(3);//µãsize
    setStroke_width(3);//Ïßsize
}


float OGCSld::getStroke_width() const
{
    return stroke_width;
}

void OGCSld::setStroke_width(float value)
{
    stroke_width = value;
}

float OGCSld::getSize() const
{
    return size;
}

void OGCSld::setSize(float value)
{
    size = value;
}

QColor OGCSld::getStroke() const
{
    return stroke;
}

void OGCSld::setStroke(const QColor &value)
{
    stroke = value;
}

QColor OGCSld::getFill() const
{
    return fill;
}

void OGCSld::setFill(const QColor &value)
{
    fill = value;
}
