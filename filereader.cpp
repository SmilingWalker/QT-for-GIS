#include "filereader.h"
#include <gdal_priv.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include<QTextCodec>
#include<stdlib.h>

FileReader::FileReader()
{

}

void FileReader::GeoJsonReader(QJsonDocument *geojson, QVector<SfsLayer*> *layers){

    double ptmaxY = DBL_MIN, ptminY=DBL_MAX, ptmaxX=DBL_MIN, ptminX=DBL_MAX;
    double lnmaxY = DBL_MIN, lnminY=DBL_MAX, lnmaxX=DBL_MIN, lnminX=DBL_MAX;
    double polymaxY = DBL_MIN,polyminY=DBL_MAX, polymaxX=DBL_MIN, polyminX=DBL_MAX;
    QString type = (*geojson)["type"].toString();
    //type 类型为两种 一种为几何对象，一种为要素对象
    unsigned int id = 0;
    SfsLayer* pointLayer = new SfsLayer();
    SfsLayer* lineStringLayer = new SfsLayer();
    SfsLayer* polygonLayer = new SfsLayer();
    bool pointLayerAdded = false, lineStringLayerAdded = false, polygonLayerAdded = false;
    if(type=="Point")
    {
        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
        SfsPoint *pt = new SfsPoint();
        pt->setId(id);
        id++;
        QJsonArray array = (*geojson)["coordinates"].toArray();
        pt->x = array[0].toDouble();
        pt->y = array[1].toDouble();
        pointLayer->geometries->append(pt);
        topY = pt->y>topY?pt->y:topY;
        buttomY = buttomY<pt->y?buttomY:pt->y;
        leftX = leftX<pt->x?leftX:pt->x;
        rightX = rightX>pt->x?rightX:pt->x;
        pointLayer->bbox->setBoundary(topY,buttomY,leftX,rightX);
    }
    else if (type=="FeatureCollection")
    {
        QJsonArray Features =(*geojson)["features"].toArray();
        QJsonObject feature;
        QString Geometry;
        for(int i=0;i<Features.size();i++){
            feature = Features[i].toObject();
            QJsonObject geometry = feature["geometry"].toObject();
            Geometry = geometry["type"].toString();
            if(Geometry=="Point")
            {
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                SfsPoint *pt = new SfsPoint();

                //添加id，可以删去
                pt->setId(id++);

                QJsonArray crds = geometry["coordinates"].toArray();
                pt->x = crds[0].toDouble();
                pt->y = crds[1].toDouble();
                pointLayer->geometries->append(pt);
                topY = pt->y>topY?pt->y:topY;
                buttomY = buttomY<pt->y?buttomY:pt->y;
                leftX = leftX<pt->x?leftX:pt->x;
                rightX = rightX>pt->x?rightX:pt->x;

                ptmaxY = topY>ptmaxY?topY:ptmaxY;
                ptminY = buttomY<ptminY?buttomY:ptminY;
                ptmaxX = rightX>ptmaxX?rightX:ptmaxX;
                ptminX = leftX<ptminX?leftX:ptminX;

                //读取property. 可能和你读property的方式不同。
                QJsonObject properties = feature["properties"].toObject();
                Properties* currentProperty = new Properties();
                currentProperty->addProperty(&properties);
                pt->setProperties(currentProperty);
            }
            if(Geometry=="Polygon"){
               double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
               QJsonArray boundary_array = geometry["coordinates"].toArray();//单个多边形层次，可能为有内点的，比如有孔的多边形
               SfsPolygon *polygon = new SfsPolygon();
               polygon->setId(id++);
               polygonLayer->geometries->append(polygon);



               for(int m1=0;m1<boundary_array.size();m1++){
                   SfsLineString *boundary = new SfsLineString();
                   polygon->boundaries->append(boundary);
                   //可以删去


                   QJsonArray points_array = boundary_array[m1].toArray();
                   for(int m2=0;m2<points_array.size();m2++)
                   {
                       //坐标点数组层次
                       QJsonArray point_array = points_array[m2].toArray();
                       for (int m3=0;m3<point_array.size();m3++) {
                           //坐标点层次
                           SfsPoint * pt = new SfsPoint();
                           boundary->pts->append(pt);
                           pt->x = point_array[0].toDouble();
                           pt->y = point_array[1].toDouble();
                           topY = pt->y>topY?pt->y:topY;
                           buttomY = buttomY<pt->y?buttomY:pt->y;
                           leftX = leftX<pt->x?leftX:pt->x;
                           rightX = rightX>pt->x?rightX:pt->x;
                       }
                   }

               }
               polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
               polymaxY = topY> polymaxY?topY:polymaxY;
               polyminY = buttomY<polyminY?buttomY:polyminY;
               polymaxX = rightX>polymaxX?rightX:polymaxX;
               polyminX = leftX<polyminX?leftX:polyminX;

               //读取property
               QJsonObject properties = feature["properties"].toObject();
               Properties* currentProperty = new Properties();
               currentProperty->addProperty(&properties);
               polygon->setProperties(currentProperty);
            }
            if(Geometry=="LineString"){
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                SfsLineString *lineString = new SfsLineString();
                lineString->setId(id++);
                lineStringLayer->geometries->append(lineString);
                //可以删去
                QJsonArray crds = geometry["coordinates"].toArray();
                for(int j=0;j<crds.size();j++){
                    QJsonArray crd = crds[j].toArray();
                    SfsPoint *pt = new SfsPoint();
                    lineString->pts->append(pt);
                    pt->x = crd[0].toDouble();
                    pt->y = crd[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                }
                lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                lnmaxY = topY>lnmaxY?topY:lnmaxY;
                lnminY = buttomY<lnminY?buttomY:lnminY;
                lnmaxX = rightX>lnmaxX?rightX:lnmaxX;
                lnminX = leftX<lnminX?leftX:lnminX;

                //读取property
                QJsonObject properties = feature["properties"].toObject();
                Properties* currentProperty = new Properties();
                currentProperty->addProperty(&properties);
                lineString->setProperties(currentProperty);
            }
            if(Geometry=="MultiPoint"){
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                QJsonArray crds = geometry["coordinates"].toArray();
                for(int j=0;j<crds.size();j++){
                    QJsonArray crd = crds[j].toArray();
                    SfsPoint *pt = new SfsPoint();
                    pt->setId(id++);
                    //修改，不再存储multi类数据
                    pointLayer->geometries->append(pt);
                    pt->x = crd[0].toDouble();
                    pt->y = crd[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;

                    //读取property
                    QJsonObject properties = feature["properties"].toObject();
                    Properties* currentProperty = new Properties();
                    currentProperty->addProperty(&properties);
                    pt->setProperties(currentProperty);
                }
                ptmaxY = topY>ptmaxY?topY:ptmaxY;
                ptminY = buttomY<ptminY?buttomY:ptminY;
                ptmaxX = rightX>ptmaxX?rightX:ptmaxX;
                ptminX = leftX<ptminX?leftX:ptminX;

            }
            if(Geometry=="MultiLineString"){

                QJsonArray array = geometry["coordinates"].toArray();
                for(int k = 0;k<array.size();k++){
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsLineString *LineString = new SfsLineString();
                    lineStringLayer->geometries->append(LineString);
                    QJsonArray crd_array = array[k].toArray();
                    for (int j=0;j<crd_array.size();j++) {
                        //每次都会读到一个点数组（array表示）
                        QJsonArray crds = crd_array[i].toArray();
                        SfsPoint * pt = new SfsPoint();
                        pt->x = crds[0].toDouble();
                        pt->y = crds[1].toDouble();
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                        pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                        LineString->pts->append(pt);
                    }
                    LineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    lnmaxY = topY>lnmaxY?topY:lnmaxY;
                    lnminY = buttomY<lnminY?buttomY:lnminY;
                    lnmaxX = rightX>lnmaxX?rightX:lnmaxX;
                    lnminX = leftX<lnminX?leftX:lnminX;

                    //读取property
                    QJsonObject properties = feature["properties"].toObject();
                    Properties* currentProperty = new Properties();
                    currentProperty->addProperty(&properties);
                    LineString->setProperties(currentProperty);
                }
            }
            if(Geometry=="MultiPolygon"){
                QJsonArray polygons_array = geometry["coordinates"].toArray();//多 个 多边形层次
                for(int m1 =0;m1<polygons_array.size();m1++){
                   double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                   SfsPolygon *polygon = new SfsPolygon();
                   polygon->setId(id++);
                   polygonLayer->geometries->append(polygon);
                   QJsonArray polygon_array = polygons_array[m1].toArray();//单个多边形层次
                   for(int m2=0;m2<polygon_array.size();m2++){
                       //单个多边形层次，可能为有内点的，比如有孔的多边形
                       SfsLineString *boundary = new SfsLineString();
                       polygon->boundaries->append(boundary);
                       QJsonArray points_array = polygon_array[m2].toArray();
                       for(int m3=0;m3<points_array.size();m3++)
                       {
                           //坐标点数组层次
                           QJsonArray point_array = points_array[m3].toArray();
                           for (int m4=0;m4<point_array.size();m4++) {
                               //每次都会读到一个点数组（array表示）
                               SfsPoint * pt = new SfsPoint();
                               boundary->pts->append(pt);
                               pt->x = point_array[0].toDouble();
                               pt->y = point_array[1].toDouble();
                               buttomY = buttomY<pt->y?buttomY:pt->y;
                               leftX = leftX<pt->x?leftX:pt->x;
                               rightX = rightX>pt->x?rightX:pt->x;
                           }
                       }

                   }
                   polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                   polymaxY = topY>polymaxY?topY:polymaxY;
                   polyminY = buttomY<polyminY?buttomY:polyminY;
                   polymaxX = rightX>polymaxX?rightX:polymaxX;
                   polyminX = leftX<polyminX?leftX:polyminX;

                   //读取属性
                   QJsonObject properties = feature["properties"].toObject();
                   Properties* currentProperty = new Properties();
                   currentProperty->addProperty(&properties);
                   polygon->setProperties(currentProperty);
                }
            }
            //图层范围值
        }
        //设置三个图层的最小外接矩形
        pointLayer->bbox->setBoundary(ptmaxY, ptminY, ptminX, ptmaxX);
        polygonLayer->bbox->setBoundary(polymaxY, polyminY, polyminX, polymaxX);
        lineStringLayer->bbox->setBoundary(lnmaxY, lnminY, lnminX, lnmaxX);
    }
    else if (type=="GeometryCollection") {
        QJsonArray geometry_array = (*geojson)["geometries"].toArray();
        for(int j=0;j<geometry_array.size();j++){
            QJsonObject geometry = geometry_array[j].toObject();//几何要素集合
            QString Geometry = geometry["type"].toString();//几何要素的类型
            if(Geometry=="Point")
            {
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                SfsPoint *pt = new SfsPoint();
                pt->setId(id++);
                QJsonArray crds = geometry["coordinates"].toArray();
                pt->x = crds[0].toDouble();
                pt->y = crds[1].toDouble();
                topY = pt->y>topY?pt->y:topY;
                buttomY = buttomY<pt->y?buttomY:pt->y;
                leftX = leftX<pt->x?leftX:pt->x;
                rightX = rightX>pt->x?rightX:pt->x;

                pointLayer->geometries->append(pt);
                ptmaxY = topY>ptmaxY?topY:ptmaxY;
                ptminY = buttomY<ptminY?buttomY:ptminY;
                ptmaxX = rightX>ptmaxX?rightX:ptmaxX;
                ptminX = leftX<ptminX?leftX:ptminX;
            }
            if(Geometry=="Polygon"){
               double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
               QJsonArray polygon_array = geometry["coordinates"].toArray();//
               SfsPolygon *polygon = new SfsPolygon();
               polygon->setId(id++);
               polygonLayer->geometries->append(polygon);


               for(int m1=0;m1<polygon_array.size();m1++){
                   //面的边界层次，内边界和外边界
                   SfsLineString *boundary = new SfsLineString();
                   polygon->boundaries->append(boundary);
                   QJsonArray points_array = polygon_array[m1].toArray();
                   for(int m2=0;m2<points_array.size();m2++)
                   {
                       //坐标点数组层次
                       QJsonArray point_array = points_array[m2].toArray();
                       SfsPoint * pt = new SfsPoint();
                       pt->x = point_array[0].toDouble();
                       pt->y = point_array[1].toDouble();
                       boundary->pts->append(pt);
                       topY = pt->y>topY?pt->y:topY;
                       buttomY = buttomY<pt->y?buttomY:pt->y;
                       leftX = leftX<pt->x?leftX:pt->x;
                       rightX = rightX>pt->x?rightX:pt->x;

                   }
               }
               polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
               polymaxY = topY>polymaxY?topY:polymaxY;
               polyminY = buttomY<polyminY?buttomY:polyminY;
               polymaxX = rightX>polymaxX?rightX:polymaxX;
               polyminX = leftX<polyminX?leftX:polyminX;
            }
            if(Geometry=="LineString"){
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                SfsLineString *lineString = new SfsLineString();
                lineString->setId(id++);
                lineStringLayer->geometries->append(lineString);
                QJsonArray crds = geometry["coordinates"].toArray();
                for(int j=0;j<crds.size();j++){
                    QJsonArray crd = crds[j].toArray();
                    SfsPoint *pt = new SfsPoint();
                    lineString->pts->append(pt);
                    pt->x = crd[0].toDouble();
                    pt->y = crd[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                }
                lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                lnmaxY = topY>lnmaxY?topY:lnmaxY;
                lnminY = buttomY<lnminY?buttomY:lnminY;
                lnmaxX = rightX>lnmaxX?rightX:lnmaxX;
                lnminX = leftX<lnminX?leftX:lnminX;
            }
            if(Geometry=="MultiPoint"){
                double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                QJsonArray crds = geometry["coordinates"].toArray();
                for(int j=0;j<crds.size();j++){
                    QJsonArray crd = crds[j].toArray();
                    SfsPoint *pt = new SfsPoint();
                    pt->setId(id++);
                    pointLayer->geometries->append(pt);
                    pt->x = crd[0].toDouble();
                    pt->y = crd[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                }
                ptmaxY = topY>ptmaxY?topY:ptmaxY;
                ptminY = buttomY<ptminY?buttomY:ptminY;
                ptmaxX = rightX>ptmaxX?rightX:ptmaxX;
                ptminX = leftX<ptminX?leftX:ptminX;
            }
            if(Geometry=="MultiLineString"){
                QJsonArray array = geometry["coordinates"].toArray();
                for(int k = 0;k<array.size();k++){
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsLineString *lineString = new SfsLineString();
                    lineStringLayer->geometries->append(lineString);
                    QJsonArray crd_array = array[k].toArray();
                    for (int j=0;j<crd_array.size();j++) {
                        //每次都会读到一个点数组（array表示）
                        QJsonArray crds = crd_array[j].toArray();
                        SfsPoint * pt = new SfsPoint();
                        pt->x = crds[0].toDouble();
                        pt->y = crds[1].toDouble();
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;

                        lineString->pts->append(pt);
                    }
                    lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    lnmaxY = topY>lnmaxY?topY:lnmaxY;
                    lnminY = buttomY<lnminY?buttomY:lnminY;
                    lnmaxX = rightX>lnmaxX?rightX:lnmaxX;
                    lnminX = leftX<lnminX?leftX:lnminX;
                }
            }
            if(Geometry=="MultiPolygon"){
                QJsonArray polygons_array = geometry["coordinates"].toArray();//多 个 多边形层次
                for(int m1 =0;m1<polygons_array.size();m1++){
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsPolygon *polygon = new SfsPolygon();
                    polygon->setId(id++);
                    polygonLayer->geometries->append(polygon);
                    QJsonArray polygon_array = polygons_array[m1].toArray();//单个多边形层次
                    for(int m2=0;m2<polygon_array.size();m2++){
                        //单个多边形层次，可能为有内点的，比如有孔的多边形
                        SfsLineString *boundary = new SfsLineString();
                        polygon->boundaries->append(boundary);
                        QJsonArray points_array = polygon_array[m2].toArray();
                        for(int m3=0;m3<points_array.size();m3++)
                        {
                            //坐标点数组层次
                            QJsonArray point_array = points_array[m3].toArray();
                            for (int m4=0;m4<point_array.size();m4++) {
                                //每次都会读到一个点数组（array表示）
                                SfsPoint * pt = new SfsPoint();
                                boundary->pts->append(pt);
                                pt->x = point_array[0].toDouble();
                                pt->y = point_array[1].toDouble();
                                topY = pt->y>topY?pt->y:topY;
                                buttomY = buttomY<pt->y?buttomY:pt->y;
                                leftX = leftX<pt->x?leftX:pt->x;
                                rightX = rightX>pt->x?rightX:pt->x;
                            }
                        }
                    }
                    polymaxY = topY>polymaxY?topY:polymaxY;
                    polyminY = buttomY<polyminY?buttomY:polyminY;
                    polymaxX = rightX>polymaxX?rightX:polymaxX;
                    polyminX = leftX<polyminX?leftX:polyminX;
                    polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                 }
            }
        }
        pointLayer->bbox->setBoundary(ptmaxY, ptminY, ptminX, ptmaxX);
        polygonLayer->bbox->setBoundary(polymaxY, polyminY, polyminX, polymaxX);
        lineStringLayer->bbox->setBoundary(lnmaxY, lnminY, lnminX, lnmaxX);
    }
    else if (type=="Polygon") {
        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
        SfsPolygon *polygon = new SfsPolygon();
        polygon->setId(id++);
        polygonLayer->geometries->append(polygon);


        QJsonArray boundary_array = (*geojson)["coordinates"].toArray();//单个多边形层次，可能为有内点的，比如有孔的多边形
        for(int m1=0;m1<boundary_array.size();m1++){
            SfsLineString *boundary = new SfsLineString();
            polygon->boundaries->append(boundary);

            QJsonArray points_array = boundary_array[m1].toArray();
            for(int m2=0;m2<points_array.size();m2++)
            {
                //坐标点数组层次
                QJsonArray point_array = points_array[m2].toArray();
                for (int m3=0;m3<point_array.size();m3++) {
                    //坐标点层次
                    SfsPoint * pt = new SfsPoint();
                    boundary->pts->append(pt);
                    pt->x = point_array[0].toDouble();
                    pt->y = point_array[1].toDouble();
                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                    pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                }
            }

        }
        polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
        polygonLayer->bbox->setBoundary(topY,buttomY,leftX,rightX);
    }
    else if (type=="LineString") {
        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
        SfsLineString *lineString = new SfsLineString();
        lineString->setId(id++);
        lineStringLayer->geometries->append(lineString);
        QJsonArray crds = (*geojson)["coordinates"].toArray();
        for(int j=0;j<crds.size();j++){
            QJsonArray crd = crds[j].toArray();
            SfsPoint *pt = new SfsPoint();
            lineString->pts->append(pt);
            pt->x = crd[0].toDouble();
            pt->y = crd[1].toDouble();
            topY = pt->y>topY?pt->y:topY;
            buttomY = buttomY<pt->y?buttomY:pt->y;
            leftX = leftX<pt->x?leftX:pt->x;
            rightX = rightX>pt->x?rightX:pt->x;
            pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
        }
        lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
        lineStringLayer->bbox->setBoundary(topY,buttomY,leftX,rightX);
    }
    else if (type=="MultiPoint") {
        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
        QJsonArray crds = (*geojson)["coordinates"].toArray();
        for(int j=0;j<crds.size();j++){
            QJsonArray crd = crds[j].toArray();
            SfsPoint *pt = new SfsPoint();
            pt->setId(id++);
            pointLayer->geometries->append(pt);
            pt->x = crd[0].toDouble();
            pt->y = crd[1].toDouble();
            topY = pt->y>topY?pt->y:topY;
            buttomY = buttomY<pt->y?buttomY:pt->y;
            leftX = leftX<pt->x?leftX:pt->x;
            rightX = rightX>pt->x?rightX:pt->x;
        }
        pointLayer->bbox->setBoundary(topY,buttomY,leftX,rightX);
    }
    else if (type=="MultiLineString") {
        QJsonArray array = (*geojson)["coordinates"].toArray();
        for(int k = 0;k<array.size();k++){
            double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
            SfsLineString *lineString = new SfsLineString();
            lineString->setId(id++);
            lineStringLayer->geometries->append(lineString);
            QJsonArray crd_array = array[k].toArray();
            for (int j=0;j<crd_array.size();j++) {
                //每次都会读到一个点数组（array表示）
                QJsonArray crds = crd_array[j].toArray();
                SfsPoint * pt = new SfsPoint();
                pt->x = crds[0].toDouble();
                pt->y = crds[1].toDouble();
                topY = pt->y>topY?pt->y:topY;
                buttomY = buttomY<pt->y?buttomY:pt->y;
                leftX = leftX<pt->x?leftX:pt->x;
                rightX = rightX>pt->x?rightX:pt->x;
                pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                lineString->pts->append(pt);
            }
            lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
            lnmaxY = topY>lnmaxY?topY:lnmaxY;
            lnminY = buttomY<lnminY?buttomY:lnminY;
            lnmaxX = rightX>lnmaxX?rightX:lnmaxX;
            lnminX = leftX<lnminX?leftX:lnminX;
        }
        lineStringLayer->bbox->setBoundary(lnmaxY, lnminY, lnminX, lnmaxX);

    }
    else if (type=="MultiPolygon") {
        QJsonArray polygons_array = (*geojson)["coordinates"].toArray();//多 个 多边形层次
        for(int m1 =0;m1<polygons_array.size();m1++){
            double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
            SfsPolygon *polygon = new SfsPolygon();
            polygon->setId(id++);
            polygonLayer->geometries->append(polygon);
            QJsonArray polygon_array = polygons_array[m1].toArray();//单个多边形层次
            for(int m2=0;m2<polygon_array.size();m2++){
                //单个多边形层次，可能为有内点的，比如有孔的多边形
                SfsLineString *boundary = new SfsLineString();
                polygon->boundaries->append(boundary);
                QJsonArray points_array = polygon_array[m2].toArray();
                for(int m3=0;m3<points_array.size();m3++)
                {
                    //坐标点数组层次
                    QJsonArray point_array = points_array[m3].toArray();
                    for (int m4=0;m4<point_array.size();m4++) {
                        //每次都会读到一个点数组（array表示）
                        SfsPoint * pt = new SfsPoint();
                        boundary->pts->append(pt);
                        pt->x = point_array[0].toDouble();
                        pt->y = point_array[1].toDouble();
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                        pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    }
                }

            }
            polymaxY = topY>polymaxY?topY:polymaxY;
            polyminY = buttomY<polyminY?buttomY:polyminY;
            polymaxX = rightX>polymaxX?rightX:polymaxX;
            polyminX = leftX<polyminX?leftX:polyminX;
            polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
         }
        polygonLayer->bbox->setBoundary(polymaxY,polyminY,polyminX,polymaxX);
    }
    if(pointLayer->geometries->size() == 0){
        delete pointLayer;
        pointLayer = nullptr;
    }
    else{
        pointLayer->setGeometype(Sfs_Point);
        layers->append(pointLayer);
    }
    if(polygonLayer->geometries->size() == 0){
        delete polygonLayer;
        polygonLayer = nullptr;
    }
    else{
        polygonLayer->setGeometype(Sfs_Polygon);
        layers->append(polygonLayer);
    }
    if(lineStringLayer->geometries->size() == 0){
        delete lineStringLayer;
        lineStringLayer = nullptr;
    }
    else{
        lineStringLayer->setGeometype(Sfs_LineString);
        layers->append(lineStringLayer);
    }
    qDebug()<<"Loading successed";
}

void FileReader::ShpfileReader(GDALDataset *pDoc, SfsLayer *layer)
{

    //读取shp文件
    double maxY = DBL_MIN,minY=DBL_MAX,maxX=DBL_MIN,minX=DBL_MAX;//整个图层的范围
    OGRLayer *ogrlayer;//shp图层
    OGRGeometry *poGeometry;//shp几何对象
    for (int i=0;i<pDoc->GetLayers().size();i++) {
        //图层循环
        //生成图层，每次读到一个图层就新建一个图层加入
        ogrlayer = pDoc->GetLayer(i);
        layer->setName(ogrlayer->GetName());
        OGRFeatureDefn *poFDefn = ogrlayer->GetLayerDefn();//图层要素的定义数据
        OGRFeature *ogrfeature;//图层内的要素
        int iGeomField;//几何要素的属性数据
        int proCount = poFDefn->GetFieldCount();//每个图层内部的属性域数据的个数
        int nGeomFieldCount;
        while ((ogrfeature=ogrlayer->GetNextFeature())!=nullptr) {
            Properties *properties = new Properties();
            nGeomFieldCount = ogrfeature->GetGeomFieldCount();
            for (int j=0;j<proCount;j++) {
                OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(j);
                switch (poFieldDefn->GetType()){
                case OFTInteger:
                    {
                        properties->ProType->append(Int_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        int *value = new int;
                        (*value) = ogrfeature->GetFieldAsInteger(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTInteger64:
                    {
                        properties->ProType->append(Int_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        int *value = new int;
                        (*value) = ogrfeature->GetFieldAsInteger64(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTString:
                    {
                        properties->ProType->append(String_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        QString *value = new QString;
                        (*value) = ogrfeature->GetFieldAsString(j);
                        properties->ProValue->append(value);

                        break;
                    }
                case OFTReal:
                    {
                        properties->ProType->append(Double_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        double *value = new double;
                        (*value) = ogrfeature->GetFieldAsDouble(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTWideString:
                    {
                        properties->ProType->append(String_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        QString *value = new QString;
                        (*value) = ogrfeature->GetFieldAsString(j);
                        properties->ProValue->append(value);
                        break;
                    }
                }
            }
              //循环建立
            for (iGeomField=0;iGeomField<nGeomFieldCount;iGeomField++) {
                poGeometry = ogrfeature->GetGeomFieldRef(iGeomField);
                //根据几何类型生成 对象
                if(poGeometry->getGeometryType()==wkbPolygon)
                {
                    //如果是单一的面要素
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsPolygon *polygon = new SfsPolygon();
                    layer->geometries->append(polygon);
                    OGRPolygon *ogrpolygon = poGeometry->toPolygon();   //单个多边形层次，可能为有内点的，比如有孔的多边形
                    SfsLineString *boundaryOut = new SfsLineString();//面要素都有外边界，先存储外边界，SfsPolygon 默认第一个就是外边界
                    polygon->boundaries->append(boundaryOut);
                    OGRLineString *ogrlineString =  ogrpolygon->getExteriorRing();
                    for(int m2=0;m2<ogrlineString->getNumPoints();m2++)
                    {
                        //坐标点数组层次
                        SfsPoint * pt = new SfsPoint();
                        boundaryOut->pts->append(pt);
                        pt->x = ogrlineString->getX(m2);
                        pt->y = ogrlineString->getY(m2);
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                    }
                    //内多边形循环
                    for (int m3=0;m3<ogrpolygon->getNumInteriorRings();m3++) {
                        SfsLineString *boundaryIn = new SfsLineString();
                        OGRLineString * linestring = ogrpolygon->getInteriorRing(m3);
                        polygon->boundaries->append(boundaryIn);
                        for(int m4=0;m4<linestring->getNumPoints();m4++)
                        {
                            //坐标点数组层次
                            SfsPoint * pt = new SfsPoint();
                            boundaryIn->pts->append(pt);
                            pt->x = linestring->getX(m4);
                            pt->y = linestring->getY(m4);

                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                        }

                    }
                    polygon->properties = properties;
                    polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbPoint) {
                    //如果是点数据
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    OGRPoint *point = poGeometry->toPoint();
                    SfsPoint *pt = new SfsPoint();
                    layer->geometries->append(pt);
                    pt->x = point->getX();
                    pt->y = point->getY();
                    //设置属性
                    pt->properties =  properties;

                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                    pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    //读取图层的范围
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbLineString) {
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsLineString *lineString = new SfsLineString();
                    layer->geometries->append(lineString);
                    OGRLineString *ogrLineString = poGeometry->toLineString();
                    for (int m1=0;m1<ogrLineString->getNumPoints();m1++) {
                        SfsPoint *pt = new SfsPoint();
                        lineString->pts->append(pt);
                        pt->x = ogrLineString->getX(m1);
                        pt->y = ogrLineString->getY(m1);

                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                    }
                    lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    //读取图层的范围
                    lineString->properties = properties;
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbMultiPoint) {
                    //如果是多点数据
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    OGRMultiPoint *ogrMultiPoint = poGeometry->toMultiPoint();
                    for (int m1=0;m1<ogrMultiPoint->getNumGeometries();m1++) {
                        SfsPoint *pt = new SfsPoint();
                        layer->geometries->append(pt);
                        pt->x = (*(ogrMultiPoint->begin()+m1))->getX();
                        pt->y = (*(ogrMultiPoint->begin()+m1))->getY();

                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;

                        pt->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    }
                    //读取图层的范围
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbMultiLineString) {
                    //如果为多线
                    OGRMultiLineString *ogrMultilineString = poGeometry->toMultiLineString();
                    for (int m1=0;m1<ogrMultilineString->getNumGeometries();m1++) {
                        //单线层次
                        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                        OGRLineString *ogrLineString = *(ogrMultilineString->begin()+m1);
                        SfsLineString *lineString = new SfsLineString();
                        layer->geometries->append(lineString);
                        for (int m2=0;m2<ogrLineString->getNumPoints();m2++) {
                            SfsPoint *pt = new SfsPoint();
                            lineString->pts->append(pt);
                            pt->x = ogrLineString->getX(m2);
                            pt->y = ogrLineString->getY(m2);
                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                        }
                        lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                        maxY = topY>maxY?topY:maxY;
                        minY = buttomY<minY?buttomY:minY;
                        maxX = rightX>maxX?rightX:maxX;
                        minX = leftX<minX?leftX:minX;
                    }

                }
                else if (poGeometry->getGeometryType()==wkbMultiPolygon) {
                    //多 多边形层次
                    OGRMultiPolygon *ogrMultiPolygon =  poGeometry->toMultiPolygon();
                    for (int m1=0;m1<ogrMultiPolygon->getNumGeometries();m1++) {
                        //单多边形层次
                        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                        SfsPolygon *polygon = new SfsPolygon();
                        layer->geometries->append(polygon);
                        OGRPolygon *ogrpolygon = *(ogrMultiPolygon->begin()+m1);
                        SfsLineString *boundaryOut = new SfsLineString();//面要素都有外边界，先存储外边界，SfsPolygon 默认第一个就是外边界
                        OGRLineString *ogrlineString =  ogrpolygon->getExteriorRing();
                        polygon->boundaries->append(boundaryOut);
                        for(int m2=0;m2<ogrlineString->getNumPoints();m2++)
                        {
                            //坐标点数组层次
                            SfsPoint * pt = new SfsPoint();
                            boundaryOut->pts->append(pt);
                            pt->x = ogrlineString->getX(m2);
                            pt->y = ogrlineString->getY(m2);

                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                        }
                        //内多边形循环
                        for (int m3=0;m3<ogrpolygon->getNumInteriorRings();m3++) {
                            SfsLineString *boundaryIn = new SfsLineString();
                            OGRLineString * linestring = ogrpolygon->getInteriorRing(m3);
                            polygon->boundaries->append(boundaryIn);
                            for(int m2=0;m2<linestring->getNumPoints();m2++)
                            {
                                //坐标点数组层次
                                SfsPoint * pt = new SfsPoint();
                                boundaryIn->pts->append(pt);
                                pt->x = linestring->getX(m2);
                                pt->y = linestring->getY(m2);

                                topY = pt->y>topY?pt->y:topY;
                                buttomY = buttomY<pt->y?buttomY:pt->y;
                                leftX = leftX<pt->x?leftX:pt->x;
                                rightX = rightX>pt->x?rightX:pt->x;
                            }
                        }
                        maxY = topY>maxY?topY:maxY;
                        minY = buttomY<minY?buttomY:minY;
                        maxX = rightX>maxX?rightX:maxX;
                        minX = leftX<minX?leftX:minX;
                        polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    }
                }
            }
        }
        layer->setGeometype(layer->geometries->value(0)->GeometryType());//为图层设置图层属性，图层至少有一个要素
        layer->bbox->setBoundary(maxY,minY,minX,maxX);
    }
    for(int n=0;n<layer->geometries->size();n++){
        layer->geometries->at(n)->setId(n);
    }
}

void FileReader::LoadPostGIS(OGRLayer *ogrlayer, SfsLayer *layer)
{
        double maxY = DBL_MIN,minY=DBL_MAX,maxX=DBL_MIN,minX=DBL_MAX;//整个图层的范围
        OGRGeometry *poGeometry;//shp几何对象
        layer->setName(ogrlayer->GetName());
        OGRFeatureDefn *poFDefn = ogrlayer->GetLayerDefn();//图层要素的定义数据
        OGRFeature *ogrfeature;//图层内的要素
        int iGeomField;//几何要素的属性数据
        int proCount = poFDefn->GetFieldCount();//每个图层内部的属性域数据的个数
        int nGeomFieldCount;
        while ((ogrfeature=ogrlayer->GetNextFeature())!=nullptr) {
            Properties *properties = new Properties();
            for (int j=0;j<proCount;j++) {
                OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(j);
                switch (poFieldDefn->GetType()){
                case OFTInteger:
                    {
                        properties->ProType->append(Int_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        int *value = new int;
                        (*value) = ogrfeature->GetFieldAsInteger(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTInteger64:
                    {
                        properties->ProType->append(Int_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        int *value = new int;
                        (*value) = ogrfeature->GetFieldAsInteger64(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTString:
                    {
                        properties->ProType->append(String_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        QString *value = new QString;
                        (*value) = ogrfeature->GetFieldAsString(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTReal:
                    {
                        properties->ProType->append(Int_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        double *value = new double;
                        (*value) = ogrfeature->GetFieldAsDouble(j);
                        properties->ProValue->append(value);
                        break;
                    }
                case OFTWideString:
                    {
                        properties->ProType->append(String_PRO);
                        properties->ProName->append(poFDefn->GetFieldDefn(j)->GetNameRef());
                        QString *value = new QString;
                        (*value) = ogrfeature->GetFieldAsString(j);
                        properties->ProValue->append(value);
                        break;
                    }
                }
            }
            nGeomFieldCount = ogrfeature->GetGeomFieldCount();//得到feature的几何对象的个数
            //循环建立
            for (iGeomField=0;iGeomField<nGeomFieldCount;iGeomField++) {
                poGeometry = ogrfeature->GetGeomFieldRef(iGeomField);
                //根据几何类型生成 对象
                if(poGeometry->getGeometryType()==wkbPolygon)
                {
                    //如果是单一的面要素
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsPolygon *polygon = new SfsPolygon();
                    layer->geometries->append(polygon);
                    OGRPolygon *ogrpolygon = poGeometry->toPolygon();   //单个多边形层次，可能为有内点的，比如有孔的多边形
                    SfsLineString *boundaryOut = new SfsLineString();//面要素都有外边界，先存储外边界，SfsPolygon 默认第一个就是外边界
                    polygon->boundaries->append(boundaryOut);
                    OGRLineString *ogrlineString =  ogrpolygon->getExteriorRing();
                    for(int m2=0;m2<ogrlineString->getNumPoints();m2++)
                    {
                        //坐标点数组层次
                        SfsPoint * pt = new SfsPoint();
                        boundaryOut->pts->append(pt);
                        pt->x = ogrlineString->getX(m2);
                        pt->y = ogrlineString->getY(m2);
                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                    }
                    //内多边形循环
                    for (int m3=0;m3<ogrpolygon->getNumInteriorRings();m3++) {
                        SfsLineString *boundaryIn = new SfsLineString();
                        OGRLineString * linestring = ogrpolygon->getInteriorRing(m3);
                        polygon->boundaries->append(boundaryIn);
                        for(int m4=0;m4<linestring->getNumPoints();m4++)
                        {
                            //坐标点数组层次
                            SfsPoint * pt = new SfsPoint();
                            boundaryIn->pts->append(pt);
                            pt->x = linestring->getX(m4);
                            pt->y = linestring->getY(m4);

                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                        }

                    }
                    polygon->properties = properties;
                    polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbPoint) {
                    //如果是点数据
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    OGRPoint *point = poGeometry->toPoint();
                    SfsPoint *pt = new SfsPoint();
                    layer->geometries->append(pt);
                    pt->x = point->getX();
                    pt->y = point->getY();
                    //设置属性
                    pt->properties =  properties;

                    topY = pt->y>topY?pt->y:topY;
                    buttomY = buttomY<pt->y?buttomY:pt->y;
                    leftX = leftX<pt->x?leftX:pt->x;
                    rightX = rightX>pt->x?rightX:pt->x;
                    //读取图层的范围
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbLineString) {
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    SfsLineString *lineString = new SfsLineString();
                    layer->geometries->append(lineString);
                    OGRLineString *ogrLineString = poGeometry->toLineString();
                    for (int m1=0;m1<ogrLineString->getNumPoints();m1++) {
                        SfsPoint *pt = new SfsPoint();
                        lineString->pts->append(pt);
                        pt->x = ogrLineString->getX(m1);
                        pt->y = ogrLineString->getY(m1);

                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                    }
                    lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    //读取图层的范围
                    lineString->properties = properties;
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbMultiPoint) {
                    //如果是多点数据
                    double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                    OGRMultiPoint *ogrMultiPoint = poGeometry->toMultiPoint();
                    for (int m1=0;m1<ogrMultiPoint->getNumGeometries();m1++) {
                        SfsPoint *pt = new SfsPoint();
                        layer->geometries->append(pt);
                        pt->x = (*(ogrMultiPoint->begin()+m1))->getX();
                        pt->y = (*(ogrMultiPoint->begin()+m1))->getY();

                        topY = pt->y>topY?pt->y:topY;
                        buttomY = buttomY<pt->y?buttomY:pt->y;
                        leftX = leftX<pt->x?leftX:pt->x;
                        rightX = rightX>pt->x?rightX:pt->x;
                    }
                    //读取图层的范围
                    maxY = topY>maxY?topY:maxY;
                    minY = buttomY<minY?buttomY:minY;
                    maxX = rightX>maxX?rightX:maxX;
                    minX = leftX<minX?leftX:minX;
                }
                else if (poGeometry->getGeometryType()==wkbMultiLineString) {
                    //如果为多线
                    OGRMultiLineString *ogrMultilineString = poGeometry->toMultiLineString();
                    for (int m1=0;m1<ogrMultilineString->getNumGeometries();m1++) {
                        //单线层次
                        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                        OGRLineString *ogrLineString = *(ogrMultilineString->begin()+m1);
                        SfsLineString *lineString = new SfsLineString();
                        layer->geometries->append(lineString);
                        for (int m2=0;m2<ogrLineString->getNumPoints();m2++) {
                            SfsPoint *pt = new SfsPoint();
                            lineString->pts->append(pt);
                            pt->x = ogrLineString->getX(m2);
                            pt->y = ogrLineString->getY(m2);
                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                        }
                        lineString->bbox->setBoundary(topY,buttomY,leftX,rightX);
                        maxY = topY>maxY?topY:maxY;
                        minY = buttomY<minY?buttomY:minY;
                        maxX = rightX>maxX?rightX:maxX;
                        minX = leftX<minX?leftX:minX;
                    }

                }
                else if (poGeometry->getGeometryType()==wkbMultiPolygon) {
                    //多 多边形层次
                    OGRMultiPolygon *ogrMultiPolygon =  poGeometry->toMultiPolygon();
                    for (int m1=0;m1<ogrMultiPolygon->getNumGeometries();m1++) {
                        //单多边形层次
                        double topY=DBL_MIN,buttomY= DBL_MAX,leftX= DBL_MAX,rightX=DBL_MIN;
                        SfsPolygon *polygon = new SfsPolygon();
                        layer->geometries->append(polygon);
                        OGRPolygon *ogrpolygon = *(ogrMultiPolygon->begin()+m1);
                        SfsLineString *boundaryOut = new SfsLineString();//面要素都有外边界，先存储外边界，SfsPolygon 默认第一个就是外边界
                        OGRLineString *ogrlineString =  ogrpolygon->getExteriorRing();
                        polygon->boundaries->append(boundaryOut);
                        for(int m2=0;m2<ogrlineString->getNumPoints();m2++)
                        {
                            //坐标点数组层次
                            SfsPoint * pt = new SfsPoint();
                            boundaryOut->pts->append(pt);
                            pt->x = ogrlineString->getX(m2);
                            pt->y = ogrlineString->getY(m2);

                            topY = pt->y>topY?pt->y:topY;
                            buttomY = buttomY<pt->y?buttomY:pt->y;
                            leftX = leftX<pt->x?leftX:pt->x;
                            rightX = rightX>pt->x?rightX:pt->x;
                        }
                        //内多边形循环
                        for (int m3=0;m3<ogrpolygon->getNumInteriorRings();m3++) {
                            SfsLineString *boundaryIn = new SfsLineString();
                            OGRLineString * linestring = ogrpolygon->getInteriorRing(m3);
                            polygon->boundaries->append(boundaryIn);
                            for(int m2=0;m2<linestring->getNumPoints();m2++)
                            {
                                //坐标点数组层次
                                SfsPoint * pt = new SfsPoint();
                                boundaryIn->pts->append(pt);
                                pt->x = linestring->getX(m2);
                                pt->y = linestring->getY(m2);

                                topY = pt->y>topY?pt->y:topY;
                                buttomY = buttomY<pt->y?buttomY:pt->y;
                                leftX = leftX<pt->x?leftX:pt->x;
                                rightX = rightX>pt->x?rightX:pt->x;
                            }
                        }
                        maxY = topY>maxY?topY:maxY;
                        minY = buttomY<minY?buttomY:minY;
                        maxX = rightX>maxX?rightX:maxX;
                        minX = leftX<minX?leftX:minX;
                        polygon->bbox->setBoundary(topY,buttomY,leftX,rightX);
                    }
                }
            }
        }
        for(int n=0;n<layer->geometries->size();n++){
            layer->geometries->at(n)->setId(n);
        }
        layer->setGeometype(layer->geometries->value(0)->GeometryType());//为图层设置图层属性，图层至少有一个要素
        layer->bbox->setBoundary(maxY,minY,minX,maxX);
}

void FileReader::SldReader(QDomDocument *sldDoc, SfsLayer *layer)
{
    //set sld
      QDomElement root = sldDoc->firstChildElement();
      QDomElement namedLayer = root.firstChildElement("NamedLayer");
      QDomElement userStyle = namedLayer.firstChildElement("UserStyle");
      QDomElement featureTypeStyle = userStyle.firstChildElement("se:FeatureTypeStyle");
      QDomElement rule = featureTypeStyle.firstChildElement("se:Rule");
      OGCSld* sld = layer->render->getSld();

      QDomElement symbolizer = rule.firstChildElement("se:PolygonSymbolizer");
      if(!symbolizer.isNull()){
          QDomElement fill = symbolizer.firstChildElement("se:Fill");
          QDomNodeList fill_ = fill.childNodes();
          int length = fill_.length();
          for(int i = 0; i < length; i++){
              QDomNode node = fill_.at(i);
              QDomElement element = node.toElement();
              if(element.attribute("name") == "fill"){
                  QString color (element.text());
                  QColor fillColor = stringToColor(color);
                  sld->setFill(fillColor);
              }
              else if(element.attribute("name") == "fill-opacity"){

              }
          }

          QDomElement stroke = symbolizer.firstChildElement("se:Stroke");
          QDomNodeList stroke_ = stroke.childNodes();
          length = stroke_.length();
          for(int i = 0; i < length; i++){
              QDomNode node = stroke_.at(i);
              QDomElement element = node.toElement();
              if(element.attribute("name") == "stroke"){
                  QString color (element.text());
                  QColor strokeColor = stringToColor(color);
                  sld->setStroke(strokeColor);
              }
              else if(element.attribute("name") == "stroke-width"){
                  QString width (element.text());
                  int width_ = width.toInt();
                  sld->setStroke_width(width_);
              }
          }
      }

      symbolizer = rule.firstChildElement("se:LineSymbolizer");
      if(!symbolizer.isNull()){
          QDomElement stroke = symbolizer.firstChildElement("se:Stroke");
          QDomNodeList stroke_ = stroke.childNodes();
          int length = stroke_.length();
          for(int i = 0; i < length; i++){
              QDomNode node = stroke_.at(i);
              QDomElement element = node.toElement();
              if(element.attribute("name") == "stroke"){
                  QString color (element.text());
                  QColor strokeColor = stringToColor(color);
                  sld->setStroke(strokeColor);
              }
              else if(element.attribute("name") == "stroke-width"){
                  QString width (element.text());
                  int width_ = width.toInt();
                  sld->setStroke_width(width_);
              }
          }
      }
      symbolizer = rule.firstChildElement("se:PointSymbolizer");
      if(!symbolizer.isNull()){
          QDomElement fill = symbolizer.firstChildElement("se:Fill");
          QDomNodeList fill_ = fill.childNodes();
          int length = fill_.length();
          for(int i = 0; i < length; i++){
              QDomNode node = fill_.at(i);
              QDomElement element = node.toElement();
              if(element.attribute("name") == "fill"){
                  QString color (element.text());
                  QColor fillColor = stringToColor(color);
                  sld->setFill(fillColor);
              }
              else if(element.attribute("name") == "fill-opacity"){

              }
          }

          QDomElement stroke = symbolizer.firstChildElement("se:Stroke");
          QDomNodeList stroke_ = stroke.childNodes();
          length = stroke_.length();
          for(int i = 0; i < length; i++){
              QDomNode node = stroke_.at(i);
              QDomElement element = node.toElement();
              if(element.attribute("name") == "stroke"){
                  QString color (element.text());
                  QColor strokeColor = stringToColor(color);
                  sld->setStroke(strokeColor);
              }
              else if(element.attribute("name") == "stroke-width"){
                  QString width (element.text());
                  int width_ = width.toInt();
                  sld->setStroke_width(width_);
              }
          }

          QDomElement size = symbolizer.firstChildElement("se:Size");
          QString size_ = size.text();
          int Size = size_.toInt();
          sld->setSize(Size);
      }
}
