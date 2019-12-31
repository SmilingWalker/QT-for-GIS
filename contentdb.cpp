#include "contentdb.h"
#include<SFS/sfslayer.h>

ContentDB::ContentDB(QObject *parent) : QObject(parent)
{
    QString filepath = "D:/QtProject/NLPIR-master/NLPIR SDK/NLPIR-ICTCLAS";
    NLPIR_Init(filepath.toStdString().c_str(),UTF8_CODE,"0f7977c44f2a601dffa078c14aeadbfc4ddc2990");
    for(char a='a';a<='z';a++){
        QVector<Metadata*>* data = new QVector<Metadata*>();
        DB[a] = data;
    }
}

ContentDB::~ContentDB()
{
    for(char a='a';a<='z';a++){
        DB.find(a);
        QVector<Metadata*>* data = DB.find(a).value();
        if(data!=nullptr)
        qDeleteAll(data->begin(),data->end());//删除所有数据
    }
}

bool ContentDB::Headchar(QString str, char &c)
{
    //对输入的字符串进行判断，如果是中文则输出汉语拼音的第一个字符，如果不是中文则输出false
    if(str.size()==0)
        return false;
    Hanz2Piny pin;
    if(pin.isHanziUnicode(str.at(0).unicode()))
    {
        if(pin.isUtf8(str.toStdString().c_str())){
            //如果是utf-8 编码
            std::vector<std::pair<bool, std::vector<std::string>>> result;
            result = pin.toPinyinFromUtf8(str.toStdString(),false,false,"");
            if(result.at(0).first)
            {
                if(result.at(0).second.at(0).size()==0)
                    return false;
                c = result.at(0).second.at(0).at(0);

                if(c>'z'||c<'a'){
                    c = c+32;
                }
                return true;
            }
            else
                return false;
        }
    }
    else
        return false;
    return false;
}

QStringList ContentDB::HanzSplit(QString str)
{
//    QString filepath = "D:/QtProject/NLPIR-master/NLPIR SDK/NLPIR-ICTCLAS";
//    NLPIR_Init(filepath.toStdString().c_str(),UTF8_CODE,"0f7977c44f2a601dffa078c14aeadbfc4ddc2990");
    QString query_clip = NLPIR_ParagraphProcess(str.toStdString().c_str(),ICT_POS_MAP_SECOND);
    QStringList list = query_clip.split(" ");//拆分
    if(list.size()>1)
        list.removeLast();
    return list;
}

bool ContentDB::Query(QVector<Metadata *> * result, QString query,SfsLayer *layer)
{
    int num = 0;
    //根据索引查询
    //字符串分词
    QStringList query_list = ContentDB::HanzSplit(query);
    //查找分词的首字母
    for (int k=0;k<query_list.size();k++) {
        QString query_split = query_list.at(k);
        char a;
        if(Headchar(query_split,a)){
            //如果成功差找到首字符串
          QVector<Metadata *> * Index_list = DB.find(a).value();
          for(int i=0;i<Index_list->size();i++){
              QString param = *(QString*)layer->geometries->value(Index_list->value(i)->ID)->properties->ProValue->value(Index_list->at(i)->ContentID);
              num ++;
              if(param.contains(query_split)){
                  //如果成功包含
                  Metadata *data = new Metadata();
                  data->ID = Index_list->value(i)->ID;
                  data->layer = layer;
                  data->content = param;
                  data->ContentID = Index_list->at(i)->ContentID;
                  result->append(data);
              }
          }
        }
   }
    qDebug()<<num;
    return true;
}

bool ContentDB::GenerateIndex(SfsLayer *layer)
{
    int num = 0;
    QTime t;
    t.start();
    if(layer==nullptr)
        return false;
    //为图层内的中文数据建立文本索引（拼音首字母索引）
    Hanz2Piny pin;
    for(int i=0;i<layer->geometries->size();i++){
        Properties *pro = layer->geometries->value(i)->properties;
        for(int j=0;j<pro->ProType->size();j++)
        {
            if(pro->ProType->value(j)==String_PRO){
                //如果是字符串的话
                //首先进行分词，然后进行存储
                QString content = (*(QString*)pro->ProValue->value(j));
                if(content.size()==0)
                    continue;
                QStringList list = HanzSplit(content);
                for(int k=0;k<list.size();k++){

                    QString str = list.value(k);
                    if(str.size()==0)continue;
                    char a;
                    if(Headchar(str,a)){
                        num++;
                    //转拼音需要20s 数据量在50万级别
                        //已经成功返回数据，构建数据组
                        Metadata *meta = new Metadata();
                        meta->ID = i;
                        meta->layer = layer;
                        meta->ContentID = j;
                        DB.find(a).value()->append(meta);
                    }
                }
            }
        }
    }
    qDebug()<<num;
    qDebug("Index elapsed: %d ms", t.elapsed());
    return true;
}

void ContentDB::init()
{
    QString filepath = "D:/QtProject/NLPIR-master/NLPIR SDK/NLPIR-ICTCLAS";
    NLPIR_Init(filepath.toStdString().c_str(),UTF8_CODE,"0f7977c44f2a601dffa078c14aeadbfc4ddc2990");
}
