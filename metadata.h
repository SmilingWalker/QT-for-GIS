#ifndef METADATA_H
#define METADATA_H

//���ڴ洢�����
#include<SFS/sfslayer.h>
class Metadata
{
public:
    Metadata();
public:
    SfsLayer *layer;    //ͼ����
    QString content;    //������
    int ID;         //������ͼ���ڵ�˳��
    int ContentID;  //�����������ݲ�
};

#endif // METADATA_H
