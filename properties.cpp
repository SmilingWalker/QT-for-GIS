#include "properties.h"
#include <QMessageBox>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

Properties::Properties()
{
    ProName = new QVector<QString>;
    ProType = new QVector<int>;
    ProValue = new QVector<void*>;
}

Properties::~Properties()
{
    if(ProValue!=nullptr)
    {
        int size = ProValue->size();
        for(int i = 0; i < size; i++){
            if(ProType->at(i) == Bool_PRO)
                delete (bool*)ProValue->at(i);
            else if(ProType->at(i) == Double_PRO)
                delete (double*)ProValue->at(i);
            else if(ProType->at(i) == String_PRO)
                delete (QString*)ProValue->at(i);
        }
        delete ProValue;
        ProValue = nullptr;
    }
    if(ProName!=nullptr)
    {
        delete ProName;
        ProName = nullptr;
    }
    if(ProType!=nullptr)
    {
        delete ProType;
        ProType = nullptr;
    }
}

bool Properties::addProperty(QString name, int type, void *value)
{
    int size = ProName->size();
    if (size != 0){
        for(int i = 0; i < size; i++){
            if(ProName->at(i) == name){
                QMessageBox::warning(nullptr,"Error!","Property already exists!", QMessageBox::Yes);
                return false;
            }
        }
    }
    if(type != Bool_PRO && type != String_PRO && type != Double_PRO){
        qDebug()<<"Type of attribute \"" << name <<"\" not supported."<<endl;
        return false;
    }
    ProName->append(name);
    ProType->append(type);
    ProValue->append(value);
    return true;
}

bool Properties::addProperty(QJsonObject* properties)
{
    int propertySize = properties->size();
    QStringList keyNames = properties->keys();
    for(int i = 0; i < propertySize; i++){
        QJsonValue property = properties->value(keyNames[i]);
        if(property.type() == Bool_PRO){
            bool* boolVal = new bool;
            *boolVal = property.toBool();
            ProName->append(keyNames[i]);
            ProType->append(Bool_PRO);
            ProValue->append(boolVal);
        }
        //QJsonValue::type只能区分开double，string, bool等，无法区分是否为int
        else if(property.type() == Double_PRO){
            double* doubleVal = new double;
            *doubleVal = property.toDouble();
            ProName->append(keyNames[i]);
            ProType->append(Double_PRO);
            ProValue->append(doubleVal);
        }
        else if(property.type() == String_PRO){
            QString* strVal = new QString();
            *strVal = property.toString();
            ProName->append(keyNames[i]);
            ProType->append(String_PRO);
            ProValue->append(strVal);
        }
    }

    return true;
}

//目前被用于向核密度对话框中添加图层属性字段名
void Properties::getPropertyAt(int index, QVariant* value)
{
    if( index < 0 || index >= ProName->size()){
        qDebug()<<"Property out of bound"<<endl;
        return;
    }
    value->setValue(ProName->at(index));
}

void Properties::getPropertyValAt(int index, QVariant *value)
{
    if( index < 0 || index >= ProName->size()){
        qDebug()<<"Property out of bound"<<endl;
        return;
    }
    int type = getProprtyTypeAt(index);
    if(type == Double_PRO){
        value->setValue(*((double*)(ProValue->at(index))));
    }
    if(type == String_PRO){
        value->setValue(*((QString*)(ProValue->at(index))));
    }
    if(type == Bool_PRO){
        value->setValue(*((bool*)(ProValue->at(index))));
    }
}

int Properties::getProprtyTypeAt(int index)
{
    if( index < 0 || index >= ProName->size()){
        qDebug()<<"Property out of bound"<<endl;
        return -1;
    }
    return ProType->at(index);
}
