#ifndef CONTENTDB_H
#define CONTENTDB_H

//���ڴ洢���ݣ��������ݼ���
//������Ҫ��ƴ��������������Ե���ÿһ��ͼ�� ��һ�����ֵ�ƴ������
//�����ڻ��Ǵ洢ͼ���ڵ�ID ��layer ָ��
//���ݷִʵĽ��������ĸ���д洢
//Ϊ�˷��㽫�ִ�ģ����з�װ������ı����ݿ⾲̬����

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
    QMap<char,QVector<Metadata*>*>DB;//���ｨ�����е����ݶ���26��Ӣ��ƴ������������
    bool Query(QVector<Metadata*>*,QString query,SfsLayer *layer);
    bool GenerateIndex(SfsLayer * layer);
private:
    void init();



signals:

public slots:
};

#endif // CONTENTDB_H
