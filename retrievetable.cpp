#include "retrievetable.h"

retrieveTable::retrieveTable(QWidget *parent):QTableView(parent)
{
    resultModel = new QStandardItemModel(this);

    //添加表头,主要是内容和字段名

    this->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中时整行选中
    this->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置只能查找不能编辑
    this->setModel(resultModel);
    selection = this->selectionModel();
    connect(selection,&QItemSelectionModel::selectionChanged,this,&retrieveTable::ResSelect);

}

void retrieveTable::showData()
{
    resultModel->clear();//会为item管理内存，这里可以进行clear就主动清除了内部item申请的内存
    //每次clear会清除掉表头信息，这里需要重新加入
    resultModel->setColumnCount(2);
    //头部 样式管理
    QStandardItem *headRes = new QStandardItem(QString::fromLocal8Bit("检索结果"));
    headRes->setTextAlignment(Qt::AlignCenter);
    headRes->setSizeHint(QSize(this->geometry().width()/2,25));
    QStandardItem *headName = new QStandardItem(QString::fromLocal8Bit("图层名"));
    headName->setTextAlignment(Qt::AlignCenter);
    headName->setSizeHint(QSize(this->geometry().width()/2,25));
    resultModel->setHorizontalHeaderItem(0,headRes);
    resultModel->setHorizontalHeaderItem(1,headName);
    Metadata *data;
    for(int i=0;i<result.size();i++){
        //每次检索重新构建数据
        data = result.value(i);
        QStandardItem *itemRes = new QStandardItem(data->content);
        QStandardItem *itemName = new QStandardItem(data->layer->getName());
        itemRes->setTextAlignment(Qt::AlignCenter);
        itemRes->setSizeHint(QSize(this->geometry().width()/2,25));
        itemName->setTextAlignment(Qt::AlignCenter);
        itemName->setSizeHint(QSize(this->geometry().width()/2,25));
        resultModel->setItem(i,0,itemRes);
        resultModel->setItem(i,1,itemName);
    }
    this->show();
}

retrieveTable::~retrieveTable()
{
    //析构函数
    if(result.size()!=0){

        for(int j=0;j<result.size();j++){
            delete result.value(j);

        }
        result.remove(0,result.size());//Qvector 清空
    }

}

void retrieveTable::RetrieveRes(SfsMap *map, QString query)
{
    //清除上一次的检索内容
    if(result.size()!=0)
    {
        for(int j=0;j<result.size();j++){
            delete result.value(j);
        }
        //每次重新检索都会清空状态数据，包括释放空间和清空vector
        result.remove(0,result.size());//
    }
    if(query=="")return;
    QString type,res;
    for(int i=0;i<map->layers->size();i++){
        SfsLayer *layer = map->layers->value(i);
        for(int j=0;j<layer->geometries->size();j++){
            SfsGeometry *geometry = layer->geometries->value(j);
            for(int k=0;k<geometry->properties->ProValue->size();k++){
                if(geometry->properties->ProType->value(k)==String_PRO){
                    //如果是文本信息的话 就进行匹配
                    if(geometry->properties->ProName->value(k)=="NAME"){
                        //这里缩小检索范围，只检索要素名称
                        QString param = (*(QString*)geometry->properties->ProValue->value(k));
                        if((*(QString*)geometry->properties->ProValue->value(k)).contains(query))
                        {

                            //这里用于检索匹配，现在只是简单的全文检索
                            //将检索到的信息进行展示
                            res = (*(QString*)geometry->properties->ProValue->value(k));
                            type = geometry->properties->ProName->value(k);
                            Metadata *data = new Metadata();
//                            qDebug()<< res.left(1);
                            data->ID = j;
                            data->layer = layer;
                            data->content = res;
                            result.append(data);

                        }
                    }

                }
            }
        }
    }

    if(result.size()!=0)
        showData();
}

void retrieveTable::ResSelect(QItemSelection selected, QItemSelection deselected)
{
    //每次进入都是新构建的
    QVector<Metadata *> select;//被附加选择的,但是本身不管理实际对象
    QVector<Metadata *> diselect;//用于没有被选择

    //这里记录的只有改动的数据，也就是说，selected是新增的，deseleted是以前有现在没有的
    QModelIndexList list = selected.indexes();//选中的
    for(int i=0;i<list.size();){
        QModelIndex index = list.value(i);
//        qDebug()<<index.data(0).toString()+" selected";
        select.append(result.value(index.row()));
        i+=2;
    }
    QModelIndexList dislist = deselected.indexes();
    for (int j=0;j<dislist.size();j++) {
        QModelIndex index = dislist.value(j);
        //qDebug()<<index.data(0).toString()+" deselected";
        diselect.append(result.value(index.row()));
        j+=2;
    }
    this->RetrievePaint(select,diselect);//发送信号，给绘图函数进行图形绘制
}

void retrieveTable::ClickSelect(Metadata *meta)
{
    //这函数用来响应点选操作，运行该操作时会清除已有的搜索内容，并且建立新的数据项
    resultModel->clear();
    //每次clear会清除掉表头信息，这里需要重新加入

    resultModel->setColumnCount(2);
    //头部 样式管理
    QStandardItem *headRes = new QStandardItem(QString::fromLocal8Bit("检索结果"));
    headRes->setTextAlignment(Qt::AlignCenter);
    headRes->setSizeHint(QSize(this->geometry().width()/2,25));
    QStandardItem *headName = new QStandardItem(QString::fromLocal8Bit("图层名"));
    headName->setTextAlignment(Qt::AlignCenter);
    headName->setSizeHint(QSize(this->geometry().width()/2,25));
    resultModel->setHorizontalHeaderItem(0,headRes);
    resultModel->setHorizontalHeaderItem(1,headName);

    //每次检索重新构建数据,点选之后会将所有信息都显示出来
    QStandardItem *itemRes = new QStandardItem(*((QString*)meta->layer->geometries->value(meta->ID)->properties->ProValue->value(8)));
    QStandardItem *itemName = new QStandardItem(meta->layer->getName());
    itemRes->setTextAlignment(Qt::AlignCenter);
    itemRes->setSizeHint(QSize(this->geometry().width()/2,25));
    itemName->setTextAlignment(Qt::AlignCenter);
    itemName->setSizeHint(QSize(this->geometry().width()/2,25));
    resultModel->setItem(0,0,itemRes);
    resultModel->setItem(0,1,itemName);

    this->show();
}
