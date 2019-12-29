#include "SFS/sfsrender.h"

SfsRender::SfsRender()
{
    sld = new OGCSld();
}

SfsRender::~SfsRender()
{
    if(sld!=nullptr)
        delete sld;
    sld = nullptr;
}

OGCSld *SfsRender::getSld() const
{
    return sld;
}

void SfsRender::setSld(OGCSld *value)
{
    sld = value;
}


