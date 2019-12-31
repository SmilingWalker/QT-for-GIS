#ifndef SPATIALINDEX_H
#define SPATIALINDEX_H

#include <QObject>
class SfsLayer;

class SpatialIndex : public QObject
{
    Q_OBJECT
public:
    explicit SpatialIndex(QObject *parent = nullptr);
    virtual ~SpatialIndex();
    virtual void createIndex();
    virtual void query(QVector<unsigned int *> *id, QRect* );
    virtual void save(QString path);
signals:

public slots:
protected:
    SfsLayer* layer;
};

#endif // SPATIALINDEX_H
