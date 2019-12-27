#ifndef SFSRENDER_H
#define SFSRENDER_H
#include<SLD/ogcsld.h>

// 一个render,一个sld指针,layer中包含渲染器, 二者分离。
//渲染器内可以配置 更多的属性，也可以集成到一个sld内

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
