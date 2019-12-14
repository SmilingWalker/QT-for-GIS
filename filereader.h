#ifndef FILEREADER_H
#define FILEREADER_H
#include<sfsgeometry.h>
#include<sfslayer.h>
#include<sfsmap.h>
#include<sfsgeometrycollection.h>
#include<sfslinestring.h>
#include<sfsmultilinestring.h>
#include<sfsmultipoint.h>
#include<sfsmultipoint.h>
#include<sfsmultipolygon.h>
#include<sfspoint.h>
#include<sfspolygon.h>
#include<qjsondocument.h>
#include<gdal.h>
#include<ogrsf_frmts.h>
#include<qdebug.h>
#include<qmessagebox.h>
#include<properties.h>



class FileReader: public QObject
{
    Q_OBJECT
public:
    FileReader();
    void GeoJsonReader(QJsonDocument *geojson,SfsLayer* layer);
    void ShpfileReader(GDALDataset *pDoc,SfsLayer* layer);
    void LoadPostGIS(GDALDataset *pDoc, SfsLayer* layer, QString layerName);
signals:
    void LayerNone();
};

#endif // FILEREADER_H
