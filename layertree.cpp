#include "layertree.h"
#include<qdebug.h>

LayerTree::LayerTree(QWidget *parent):QTreeWidget(parent)
{
    this->setHeaderHidden(true);
    fileReader = new FileReader();
//    ����ǰ��ı�־
    this->setRootIsDecorated(false);
    connect(this,&LayerTree::itemChanged,this,&LayerTree::itemChangeSlot);

}

LayerTree::~LayerTree()
{
    if(fileReader!=nullptr)
        delete fileReader;
    fileReader = nullptr;
}

void LayerTree::AddLayer(SfsLayer *layer)
{

    QTreeWidgetItem *layerItem = new QTreeWidgetItem(this,QStringList(QStringList(layer->getName())));
    layerItem->setIcon(0,QIcon("D:/QtProject/GeoJSON/icos/layer1.ico"));
    layerItem->setCheckState(0,Qt::Checked);
    layerItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    //��ΪTreeItemû����Ӧitemcheck���źţ�����Ϊ�����һ������ֵ(͵�������Դ������ԣ�������Ȼû��pro���ԣ�2����checked 0����unchecked
    layerItem->setWhatsThis(0,"2");
}
void LayerTree::LoadSld()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        if(fileName.right(4)!=".sld")
        QMessageBox::critical(this,"ERROR","File is not .sld file");
        else {
            QFile sldFile(fileName);
            if(!sldFile.open(QFile::ReadOnly | QFile::Text)){
                qDebug()<<"LoadSld: Opening file failed!"<<endl;
                return;
            }
            QDomDocument* sldDoc = new QDomDocument();
            if(!sldDoc->setContent(&sldFile, false)){
                qDebug()<<"LoadSld: setting content failed!"<<endl;
                return;
            }
            sldFile.close();
            fileReader->SldReader(sldDoc, map->layers->at(triggerdItemIndex));
            //������ػ棬����Ӧ�ò���Ҫ��ȥ��������������VAO��VBO��ֻ��Ҫ���Ƶ�ģʽ����
            //����ֻ��Ҫupdateһ��glwidget���ɣ�������animate
            //ui->glwidget->animate(render.at(triggerdItemIndex));
            delete sldDoc;
            updateMap();
        }
    }
}

void LayerTree::SetStyle()
{
    OGCSld* sld = map->layers->at(triggerdItemIndex)->render->getSld();
    SldSettingForm* settingForm = new SldSettingForm(sld);
    settingForm->exec();
    updateMap();
    //ֻ��Ҫˢ���ػ漴��
    //ui->glwidget->animate(render.at(triggerdItemIndex));
}

void LayerTree::LayerToUpstair()
{
    if(triggerdItemIndex!=0)
    {
        map->layers->move(triggerdItemIndex,triggerdItemIndex-1);
        QTreeWidgetItem *item=this->takeTopLevelItem(triggerdItemIndex);
        this->insertTopLevelItem(triggerdItemIndex-1,item);
        updateMap();
    }
}

void LayerTree::LayerToTop()
{

    if(triggerdItemIndex!=0)
    {
        //����ͼ��˳��
        map->layers->move(triggerdItemIndex,0);
        QTreeWidgetItem *item=this->takeTopLevelItem(triggerdItemIndex);
        this->insertTopLevelItem(0,item);
        updateMap();
    }
}

void LayerTree::LayerToDown()
{
    if(triggerdItemIndex<map->layers->size()-1)
    {
        map->layers->move(triggerdItemIndex,triggerdItemIndex+1);
        QTreeWidgetItem *item=this->takeTopLevelItem(triggerdItemIndex);
        this->insertTopLevelItem(triggerdItemIndex+1,item);
        updateMap();
    }
}

void LayerTree::Zoom()
{
    //���ŵ�ͼ�㺯����������Ҫ�ǵõ�pressitem
    SfsLayer *layer = map->layers->at(triggerdItemIndex);
    LayerZoom(layer);
}

void LayerTree::Remove()
{
    //ɾ��ͼ��
    this->takeTopLevelItem(triggerdItemIndex);//�Ƴ����ṹ�ڵ�ͼ������
    SfsLayer *layer = map->layers->at(triggerdItemIndex);//�Ƴ���ͼ�ڵ�ͼ������
    //�����źţ���glwidget������ ͼ�����ݣ��������ɾ���������ݣ�����glwidget��
    RemoveLayer(layer);
    updateMap();

}

void LayerTree::setLayerVisibility(QTreeWidgetItem *item)
{
    int check = item->whatsThis(0).toInt();
    int itemcheck = item->checkState(0);
    if(check!=item->checkState(0))
    {
       int index = this->indexOfTopLevelItem(item);
       if(map->layers->size()-1>=index)
       {
           if(itemcheck==0){
               item->setWhatsThis(0,"0");
               map->layers->at(index)->setVisible(false);
           }
           else{
               item->setWhatsThis(0,"2");
               map->layers->at(index)->setVisible(true);
           }
           updateMap();
       }
    }
}

void LayerTree::itemChangeSlot(QTreeWidgetItem *item, int column)
{
    setLayerVisibility(item);//�����ɼ��ԣ��Ƿ��������set*** ���ж�(��һ�ε���Ҳ�ᴥ�����������
}

void LayerTree::mouseDoubleClickEvent(QMouseEvent *event)
{
    //���˫����Ӧ
}

void LayerTree::contextMenuEvent(QContextMenuEvent *event)
{
    //�������Ӧ(һ�����Ҽ������¼���
    //���ݵ�ǰ��������λ�õõ�itemԪ��
    QTreeWidgetItem *pressedItem = this->itemAt(event->pos());
    if(pressedItem!=nullptr)//����ܳɹ��õ�
    {
        this->setContextMenuPolicy(Qt::DefaultContextMenu);
        triggerdItemIndex = this->indexOfTopLevelItem(pressedItem);
        treeWidgetItemMenu = new QMenu(this);
        loadSld = new QAction("Load SLD", treeWidgetItemMenu);
        setStyle = new QAction("Set Layer Style", treeWidgetItemMenu);
        toTop = new QAction(QString::fromLocal8Bit("To top"),treeWidgetItemMenu);
        toUpstair = new QAction(QString::fromLocal8Bit("To upstair"),treeWidgetItemMenu);
        toDownStair = new QAction(QString::fromLocal8Bit("To downstair"),treeWidgetItemMenu);
        ZoomToLayer = new QAction(QString::fromLocal8Bit("Zoom to layer"),treeWidgetItemMenu);
        remove = new QAction(QString::fromLocal8Bit("remove layer"),treeWidgetItemMenu);
        treeWidgetItemMenu->addAction(loadSld);
        treeWidgetItemMenu->addAction(setStyle);
        treeWidgetItemMenu->addAction(toTop);
        treeWidgetItemMenu->addAction(toUpstair);
        treeWidgetItemMenu->addAction(toDownStair);
        treeWidgetItemMenu->addAction(ZoomToLayer);
        treeWidgetItemMenu->addAction(remove);

        //connect signals and slots
        connect(loadSld,SIGNAL(triggered()),this, SLOT(LoadSld()));
        connect(setStyle, SIGNAL(triggered()), this, SLOT(SetStyle()));
        connect(ZoomToLayer,SIGNAL(triggered()),this, SLOT(Zoom()));
        connect(toTop, SIGNAL(triggered()), this, SLOT(LayerToTop()));
        connect(toUpstair,SIGNAL(triggered()),this, SLOT(LayerToUpstair()));
        connect(toDownStair,SIGNAL(triggered()),this, SLOT(LayerToDown()));
        connect(remove,SIGNAL(triggered()),this, SLOT(Remove()));
        //�˵�������������
        treeWidgetItemMenu->exec(QCursor::pos());
        delete treeWidgetItemMenu;
    }

    event->accept();
}

