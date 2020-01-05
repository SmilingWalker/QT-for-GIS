#ifndef PROPERTIES_H
#define Bool_PRO 0x1
#define String_PRO 0x3
#define Double_PRO 0x2
#define Int_PRO 1000


#define PROPERTIES_H

#include<qstring.h>
#include<qvector.h>
#include<QVariant>
#include<QJsonValue>
class Properties
{
public:
    Properties();
    ~Properties();
    bool addProperty(QString name, int type, void* value);
    bool addProperty(QJsonObject* properties);
    //获取property name, 将QVariant*作为参数，name写到QVairant 里。当初马虎，很不合理。但由于时间关系没有改
    void getPropertyAt(int index, QVariant* value);
    //获取属性值
    void getPropertyValAt(int index, QVariant* value);
    //获取属性类型
    int getProprtyTypeAt(int index);
public:
    QVector<QString> *ProName;
    QVector<int> *ProType;
    QVector<void*> *ProValue;
};

#endif // PROPERTIES_H
