#include "layertree.h"
#include<qdebug.h>

LayerTree::LayerTree(QWidget *parent):QTreeWidget(parent)
{
    this->setHeaderHidden(true);
    fileReader = new FileReader();
//    设置前面的标志
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
    //因为TreeItem没有响应itemcheck的信号，这里为它添加一个属性值(偷懒，用自带的属性，不过居然没有pro属性）2代表checked 0代表unchecked
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
            //如何来重绘，这里应该不需要再去将数据重新生成VAO和VBO，只需要绘制的模式即可
            //所以只需要update一下glwidget即可，而不是animate
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
    //只需要刷新重绘即可
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
        //调整图层顺序
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
    //缩放到图层函数，这里主要是得到pressitem
    SfsLayer *layer = map->layers->at(triggerdItemIndex);
    LayerZoom(layer);
}

void LayerTree::Remove()
{
    //删除图层
    this->takeTopLevelItem(triggerdItemIndex);//移除树结构内的图层数据
    SfsLayer *layer = map->layers->at(triggerdItemIndex);//移除地图内的图层数据
    //发出信号，让glwidget来处理 图层数据，（管理和删除更改数据，都给glwidget）
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
    setLayerVisibility(item);//调整可见性，是否调整丢给set*** 来判断(第一次调动也会触发这个函数）
}

void LayerTree::mouseDoubleClickEvent(QMouseEvent *event)
{
    //鼠标双击响应
}

void LayerTree::contextMenuEvent(QContextMenuEvent *event)
{
    //鼠标点击响应(一定是右键单击事件）
    //根据当前鼠标的坐标位置得到item元素
    QTreeWidgetItem *pressedItem = this->itemAt(event->pos());
    if(pressedItem!=nullptr)//如果能成功得到
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
        //菜单栏出现在这里
        treeWidgetItemMenu->exec(QCursor::pos());
        delete treeWidgetItemMenu;
    }

    event->accept();
}

