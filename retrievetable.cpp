#include "retrievetable.h"

retrieveTable::retrieveTable(QWidget *parent):QTableView(parent)
{
    resultModel = new QStandardItemModel(this);

    //��ӱ�ͷ,��Ҫ�����ݺ��ֶ���

    this->setSelectionBehavior(QAbstractItemView::SelectRows); //����ѡ��ʱ����ѡ��
    this->setEditTriggers(QAbstractItemView::NoEditTriggers); //����ֻ�ܲ��Ҳ��ܱ༭
    this->setModel(resultModel);
    selection = this->selectionModel();
    connect(selection,&QItemSelectionModel::selectionChanged,this,&retrieveTable::ResSelect);

}

void retrieveTable::showData()
{
    resultModel->clear();//��Ϊitem�����ڴ棬������Խ���clear������������ڲ�item������ڴ�
    //ÿ��clear���������ͷ��Ϣ��������Ҫ���¼���
    resultModel->setColumnCount(2);
    //ͷ�� ��ʽ����
    QStandardItem *headRes = new QStandardItem(QString::fromLocal8Bit("�������"));
    headRes->setTextAlignment(Qt::AlignCenter);
    headRes->setSizeHint(QSize(this->geometry().width()/2,25));
    QStandardItem *headName = new QStandardItem(QString::fromLocal8Bit("ͼ����"));
    headName->setTextAlignment(Qt::AlignCenter);
    headName->setSizeHint(QSize(this->geometry().width()/2,25));
    resultModel->setHorizontalHeaderItem(0,headRes);
    resultModel->setHorizontalHeaderItem(1,headName);
    Metadata *data;
    for(int i=0;i<result.size();i++){
        //ÿ�μ������¹�������
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
    //��������
    if(result.size()!=0){

        for(int j=0;j<result.size();j++){
            delete result.value(j);

        }
        result.remove(0,result.size());//Qvector ���
    }

}

void retrieveTable::RetrieveRes(SfsMap *map, QString query)
{
    //�����һ�εļ�������
    if(result.size()!=0)
    {
        for(int j=0;j<result.size();j++){
            delete result.value(j);
        }
        //ÿ�����¼����������״̬���ݣ������ͷſռ�����vector
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
                    //������ı���Ϣ�Ļ� �ͽ���ƥ��
                    if(geometry->properties->ProName->value(k)=="NAME"){
                        //������С������Χ��ֻ����Ҫ������
                        QString param = (*(QString*)geometry->properties->ProValue->value(k));
                        if((*(QString*)geometry->properties->ProValue->value(k)).contains(query))
                        {

                            //�������ڼ���ƥ�䣬����ֻ�Ǽ򵥵�ȫ�ļ���
                            //������������Ϣ����չʾ
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
    //ÿ�ν��붼���¹�����
    QVector<Metadata *> select;//������ѡ���,���Ǳ�������ʵ�ʶ���
    QVector<Metadata *> diselect;//����û�б�ѡ��

    //�����¼��ֻ�иĶ������ݣ�Ҳ����˵��selected�������ģ�deseleted����ǰ������û�е�
    QModelIndexList list = selected.indexes();//ѡ�е�
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
    this->RetrievePaint(select,diselect);//�����źţ�����ͼ��������ͼ�λ���
}

void retrieveTable::ClickSelect(Metadata *meta)
{
    //�⺯��������Ӧ��ѡ���������иò���ʱ��������е��������ݣ����ҽ����µ�������
    resultModel->clear();
    //ÿ��clear���������ͷ��Ϣ��������Ҫ���¼���

    resultModel->setColumnCount(2);
    //ͷ�� ��ʽ����
    QStandardItem *headRes = new QStandardItem(QString::fromLocal8Bit("�������"));
    headRes->setTextAlignment(Qt::AlignCenter);
    headRes->setSizeHint(QSize(this->geometry().width()/2,25));
    QStandardItem *headName = new QStandardItem(QString::fromLocal8Bit("ͼ����"));
    headName->setTextAlignment(Qt::AlignCenter);
    headName->setSizeHint(QSize(this->geometry().width()/2,25));
    resultModel->setHorizontalHeaderItem(0,headRes);
    resultModel->setHorizontalHeaderItem(1,headName);

    //ÿ�μ������¹�������,��ѡ֮��Ὣ������Ϣ����ʾ����
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
