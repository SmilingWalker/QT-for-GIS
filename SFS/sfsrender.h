#ifndef SFSRENDER_H
#define SFSRENDER_H
#include<SLD/ogcsld.h>

// һ��render,һ��sldָ��,layer�а�����Ⱦ��, ���߷��롣
//��Ⱦ���ڿ������� ��������ԣ�Ҳ���Լ��ɵ�һ��sld��

class SfsRender
{
public:
    SfsRender();
public:
    OGCSld *getSld() const;
    void setSld(OGCSld *value);

private:
    OGCSld* sld;


};

#endif // SFSRENDER_H
