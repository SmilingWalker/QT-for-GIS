#ifndef RETRIVETABLE_H
#define RETRIVETABLE_H

#include <QWidget>
#include<qtableview.h>
#include<sfsmap.h>
#include<metadata.h>
#include<qvector.h>
#include<qdebug.h>
#include<QStandardItem>
#include<QStandardItemModel>
#include<qheaderview.h>
#include<qitemselectionmodel.h>
#include<QItemSelection>
#include<QModelIndexList>

class retrieveTable : public QTableView
{
        Q_OBJECT
public:
    retrieveTable(QWidget *parent);
    void showData();
    ~retrieveTable();
public slots:
    void RetrieveRes(SfsMap *map,QString query);
    void ResSelect(QItemSelection selected,QItemSelection deselected);

public:
    QVector<Metadata*> result;//这是用于存储检索出来的数据的
    QStandardItemModel *resultModel;
    QItemSelectionModel *selection;
signals:
    void RetrievePaint(QVector<Metadata*> selectNew,QVector<Metadata*> deselect);
};

#endif // RETRIVETABLE_H
