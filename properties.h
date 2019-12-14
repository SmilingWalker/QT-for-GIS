#ifndef PROPERTIES_H
#define Int_PRO 1000
#define String_PRO 1001
#define Double_PRO 1002

#define PROPERTIES_H


#include<qstring.h>
#include<qvector.h>
class Properties
{
public:
    Properties();
    ~Properties();
public:
    QVector<QString> *ProName;
    QVector<int> *ProType;
    QVector<void*> *ProValue;
};

#endif // PROPERTIES_H
