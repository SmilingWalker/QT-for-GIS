#ifndef CONTENTDB_H
#define CONTENTDB_H

#include <QObject>
//���ڴ洢���ݣ��������ݼ���
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
