#ifndef TREEINDEX_SELECT_H
#define TREEINDEX_SELECT_H

#include <QWidget>
#include"SFS/sfsmap.h"

namespace Ui {
class treeIndex_Select;
}

class treeIndex_Select : public QWidget
{
    Q_OBJECT

public:
    explicit treeIndex_Select(QWidget *parent = nullptr,SfsMap *map=nullptr);
    ~treeIndex_Select();

private slots:
    void on_yes_clicked();

    void on_cancel_clicked();
signals:
    void TreeIndex(SfsLayer *layer,int limitation);

private:
    Ui::treeIndex_Select *ui;
    SfsMap *map;
};

#endif // TREEINDEX_SELECT_H
