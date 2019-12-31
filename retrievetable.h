#ifndef RETRIVETABLE_H
#define RETRIVETABLE_H

#include <QWidget>
#include<qtableview.h>
#include<SFS/sfsmap.h>
#include<metadata.h>
#include<qvector.h>
#include<qdebug.h>
#include<QStandardItem>
#include<QStandardItemModel>
#include<qheaderview.h>
#include<qitemselectionmodel.h>
#include<QItemSelection>
#include<QModelIndexList>
#include<NLPIR.h>
#include<libhanz2piny/Hanz2Piny.h>

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
    void ClickSelect(Metadata* meta);

public:
    QVector<Metadata*> result;//这是用于存储检索出来的数据的
    QStandardItemModel *resultModel;
    QItemSelectionModel *selection;
private:
    void QueryByIndex(SfsLayer *layer,QString query);
    void QueryByAll(SfsLayer *layer,QString query);
signals:
    void RetrievePaint(QVector<Metadata*> selectNew,QVector<Metadata*> deselect);
};

#endif // RETRIVETABLE_H
