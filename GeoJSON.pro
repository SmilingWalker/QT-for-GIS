QT       += core gui
QT       += positioning
QT       += gui
QT       += widgets
QT       += opengl
QT       += xml
LIBS += -lopengl32 -lglu32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GPC/gpc.c \
    Index/grid.cpp \
    Index/gridindex.cpp \
    Index/spatialindex.cpp \
    Projection/maplambert.cpp \
    Projection/mapproject.cpp \
    SFS/sfsgeometry.cpp \
    SFS/sfsgeometrycollection.cpp \
    SFS/sfslayer.cpp \
    SFS/sfslinestring.cpp \
    SFS/sfsmap.cpp \
    SFS/sfsmultilinestring.cpp \
    SFS/sfsmultipoint.cpp \
    SFS/sfsmultipolygon.cpp \
    SFS/sfspoint.cpp \
    SFS/sfspolygon.cpp \
    SFS/sfsrender.cpp \
    SLD/StringToColor.cpp \
    SLD/ogcsld.cpp \
    Tool/Kernel/SpatialCalculation.cpp \
    Tool/Kernel/kerneldensity.cpp \
    Tool/Kernel/spatialstatistics.cpp \
    boundarybox.cpp \
    connect.cpp \
    contentdb.cpp \
    filereader.cpp \
    glwidget.cpp \
    kernel_show.cpp \
    kernel_showwidget.cpp \
    layertree.cpp \
    libhanz2piny/Hanz2Piny.cpp \
    main.cpp \
    mainwindow.cpp \
    metadata.cpp \
    properties.cpp \
    prquadtree.cpp \
    retrievetable.cpp \
    std_reader.cpp \
    treeindex_select.cpp \
    variousDialogs.cpp

HEADERS += \
    ../NLPIR-master/NLPIR SDK/NLPIR-ICTCLAS/include/NLPIR.h \
    GPC/gpc.h \
    Index/grid.h \
    Index/gridindex.h \
    Index/spatialindex.h \
    NLPIR.h \
    Projection/maplambert.h \
    Projection/mapproject.h \
    SFS/sfsgeometry.h \
    SFS/sfsgeometrycollection.h \
    SFS/sfslayer.h \
    SFS/sfslinestring.h \
    SFS/sfsmap.h \
    SFS/sfsmultilinestring.h \
    SFS/sfsmultipoint.h \
    SFS/sfsmultipolygon.h \
    SFS/sfspoint.h \
    SFS/sfspolygon.h \
    SFS/sfsrender.h \
    SLD/StringToColor.h \
    SLD/ogcsld.h \
    Tool/Kernel/SpatialCalculation.h \
    Tool/Kernel/kerneldensity.h \
    Tool/Kernel/spatialstatistics.h \
    boundarybox.h \
    connect.h \
    contentdb.h \
    filereader.h \
    glwidget.h \
    kernel_show.h \
    kernel_showwidget.h \
    layertree.h \
    libhanz2piny/Hanz2Piny.h \
    libhanz2piny/checked.h \
    libhanz2piny/core.h \
    libhanz2piny/unchecked.h \
    libhanz2piny/utf8.h \
    mainwindow.h \
    metadata.h \
    properties.h \
    prquadtree.h \
    retrievetable.h \
    std_reader.h \
    treeindex_select.h \
    variousDialogs.h

FORMS += \
    connect.ui \
    kernel_show.ui \
    mainwindow.ui \
    treeindex_select.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../gdal/lib/ -lgdal_i
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../gdal/lib/ -lgdal_i
#else:unix: LIBS += -L$$PWD/../../gdal/lib/ -lgdal_i

#INCLUDEPATH += $$PWD/../../gdal/include
#DEPENDPATH += $$PWD/../../gdal/include


unix|win32: LIBS += -L$$PWD/../../gdal2.4/lib/ -lgdal_i

INCLUDEPATH += $$PWD/../../gdal2.4/include
DEPENDPATH += $$PWD/../../gdal2.4/include

unix|win32: LIBS += -LE:/pg/lib/ -llibpq

INCLUDEPATH += E:/pg/include
DEPENDPATH += E:/pg/include

DISTFILES += \
    QtIco.rc \
    fragmentShaderSource.frag \
    vertexShaderSource.vert

RESOURCES += \
    resource.qrc


unix|win32: LIBS += -L$$PWD/'../NLPIR-master/NLPIR SDK/NLPIR-ICTCLAS/lib/win64/' -lNLPIR

INCLUDEPATH += $$PWD/'../NLPIR-master/NLPIR SDK/NLPIR-ICTCLAS/include'
DEPENDPATH += $$PWD/'../NLPIR-master/NLPIR SDK/NLPIR-ICTCLAS/include'
