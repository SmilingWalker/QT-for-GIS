#ifndef FILEREADER_H
#define FILEREADER_H
#include<SFS/sfsgeometry.h>
#include<SFS/sfslayer.h>
#include<SFS/sfsmap.h>
#include<SFS/sfsgeometrycollection.h>
#include<SFS/sfslinestring.h>
#include<SFS/sfsmultilinestring.h>
#include<SFS/sfsmultipoint.h>
#include<SFS/sfsmultipoint.h>
#include<SFS/sfsmultipolygon.h>
#include<SFS/sfspoint.h>
#include<SFS/sfspolygon.h>
#include<qjsondocument.h>
#include<gdal.h>
#include<ogrsf_frmts.h>
#include<qdebug.h>
#include<qmessagebox.h>
#include<properties.h>
#include<metadata.h>
#include<contentdb.h>



class FileReader: public QObject
{
    Q_OBJECT
public:
    FileReader();
    void GeoJsonReader(QJsonDocument *geojson,SfsLayer* layer);
    void ShpfileReader(GDALDataset *pDoc,SfsLayer* layer);
    void LoadPostGIS(OGRLayer *ogrlayer, SfsLayer* layer);
signals:
    void LayerNone();
};

#endif // FILEREADER_H
