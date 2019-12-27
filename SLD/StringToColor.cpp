#include<SLD/StringToColor.h>
QColor stringToColor(QString string){
    QString ox = "0x";
    QString red = string.mid(1,2);
    red  = ox + red;
    QString green = string.mid(3,2);
    green = ox + green;
    QString blue = string.mid(5,2);
    blue = ox + blue;
    char* end = nullptr;

    QByteArray rba = red.toLatin1();
    char* r = rba.data();
    QByteArray gba = green.toLatin1();
    char* g = gba.data();
    QByteArray bba = blue.toLatin1();
    char* b = bba.data();

    int R = strtol(r,&end, 16);
    int G = strtol(g,&end,16);
    int B = strtol(b,&end, 16);

    return QColor(R,G,B);
}

