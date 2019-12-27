#include "SFS/sfsrender.h"

SfsRender::SfsRender()
{
}

OGCSld *SfsRender::getSld() const
{
    return sld;
}

void SfsRender::setSld(OGCSld *value)
{
    sld = value;
}


