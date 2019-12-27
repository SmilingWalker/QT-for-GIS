#ifndef OGCSLD_H
#define OGCSLD_H

#include <QObject>
#include <QColor>

class OGCSld : public QObject
{
    Q_OBJECT
public:
    explicit OGCSld(QObject *parent = nullptr);

    float getStroke_width() const;
    void setStroke_width(float value);

    float getSize() const;
    void setSize(float value);

    QColor getStroke() const;
    void setStroke(const QColor &value);

    QColor getFill() const;
    void setFill(const QColor &value);

signals:

public slots:

private:
    //部分sld样式参数：fill为面填充颜色， size为点半径，stroke为线填充颜色， stroke-width为线宽
    float stroke_width;
    float size;
    QColor fill;
    QColor stroke;
};

#endif // OGCSLD_H
