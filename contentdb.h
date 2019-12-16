#ifndef CONTENTDB_H
#define CONTENTDB_H

#include <QObject>
//用于存储数据，用于数据检索
#include<qvector.h>
#include<metadata.h>


class ContentDB : public QObject
{
    Q_OBJECT
public:
    explicit ContentDB(QObject *parent = nullptr);
public:
    QVector<Metadata *> DB;


signals:

public slots:
};

#endif // CONTENTDB_H
