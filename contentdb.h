#ifndef CONTENTDB_H
#define CONTENTDB_H

//用于存储数据，用于数据检索
//这里主要是拼音索引，并且针对的是每一个图层 有一个文字的拼音索引
//索引内还是存储图层内的ID 和layer 指针
//根据分词的结果的首字母进行存储
//为了方便将分词模块进行封装，变成文本数据库静态函数

#include <QObject>
#include<qvector.h>
#include<metadata.h>
#include<qmap.h>
#include<libhanz2piny/Hanz2Piny.h>
#include<qstring.h>
#include<qstringlist.h>
#include<NLPIR.h>
#include<metadata.h>
#include<qdebug.h>
#include<QTime>

class SfsLayer;
class ContentDB : public QObject
{
    Q_OBJECT
public:
    explicit ContentDB(QObject *parent = nullptr);
    ~ContentDB();
    static bool Headchar(QString str,char & c);
    static QStringList HanzSplit(QString str);
public:
    QMap<char,QVector<Metadata*>*>DB;//这里建立所有的数据对象，26个英文拼音建立索引库
    bool Query(QVector<Metadata*>*,QString query,SfsLayer *layer);
    bool GenerateIndex(SfsLayer * layer);
private:
    void init();



signals:

public slots:
};

#endif // CONTENTDB_H
