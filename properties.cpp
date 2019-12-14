#include "properties.h"

Properties::Properties()
{
    ProName = new QVector<QString>;
    ProType = new QVector<int>;
    ProValue = new QVector<void*>;
}

Properties::~Properties()
{
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
    if(ProValue!=nullptr)
    {
        delete ProValue;
        ProValue = nullptr;
    }
}
