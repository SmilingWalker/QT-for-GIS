#ifndef GLWIDGET_H
#define GLWIDGET_H
#include<qdebug.h>
#include<qopenglwidget.h>
#include<qpainter.h>
#include<SFS/sfsrender.h>
#include<gl/GL.h>
#include<qopenglfunctions.h>
#include<gl/GLU.h>
#include<qglfunctions.h>
#include<qevent.h>
#include<QMouseEvent>
#include<qpoint.h>
#include<SFS/sfspoint.h>
#include<qobject.h>
#include<GLES3/gl3.h>
#include<qopenglfunctions_3_3_core.h>
#include<qopenglfunctions_4_5_core.h>
#include<qopenglbuffer.h>
#include<qopenglvertexarrayobject.h>
#include<qopenglshaderprogram.h>
#include<QOpenGLShader>
#include<qvector.h>
#include<MtxAttr.h>
#include<QMatrix4x4>
#include<metadata.h>
#include<metadata.h>
#include<prquadtree.h>
#include<SFS/sfsmap.h>
#include<SFS/sfslayer.h>
#include<qmap.h>





class GLwidget : public QOpenGLWidget,protected QOpenGLFunctions_3_3_Core
{
        Q_OBJECT
public:
    GLwidget(QWidget *parent);
    ~ GLwidget() override;

public slots:
    void animate(SfsLayer *layer);
    void RetrievePaint(QVector<Metadata*> selectNew,QVector<Metadata*> deselect);
    void clearSelect();
    void ChangeSelect();
    void updateMap();
    void ZoomToLayer(SfsLayer *layer);
    void RemoveLayer(SfsLayer* layer);


public:
    bool Selection;
    QVector<Metadata *> RetrieveResult;//实际变量
    QVector<Metadata *> temp;//临时变量
    bool translate;//平移判断
    bool first,move;
    bool left_bt,right_bt;//鼠标左右健按下

    double ty,by,lx,rx,change_x,change_y;//
    double tem_x,tem_y;//平移过程中的临时存放量
    double scale;//缩放尺度
    GLint width,height;

    SfsPoint S_pt1,S_pt2;//对应的实际坐标点，
    QPoint q_Pt1,q_Pt2;//屏幕的点，1表示按压，2表示抬起
    QPoint move_pt;//平移过程中的点
    SfsPoint move_spt;//平移中的实际运算点

    GLubyte* PixelData;//纹理
    GLuint MapTexture;//地图纹理数据

    int attrPos;//着色器程序的数据索引值
//    QVector<QOpenGLBuffer *> *VBOs,*IBOs;// 顶点缓冲，和索引缓冲 //这些对象都是不需要的，因为在实际过程中，数据都会绑定到VAO内部，不会再去从内存中读取IBO和VBO的值
    QOpenGLShaderProgram *m_shaderProgram;//着色器程序
    QVector<QOpenGLVertexArrayObject*> *VAOs;//顶点数组,(个人感觉应该不能把所有的图层内容都放置到一个VAOs里，毕竟基本上的操作都是针对图层的，所有每一个图层有一个VAOs才对，这里直接建立一个VAOs对象数组
    QVector<QVector<QOpenGLVertexArrayObject*>*> *VAO_Layer;
    QOpenGLShader *m_FragmentShader,*m_VertexShader;//着色器
    QMatrix4x4 Project,ModelView;//投影矩阵，和模视转换矩阵
    bool selectChange;
    PRQuadTree *PRtree;
    //需要一个绑定数据，将layer 和VAOs绑定，实际上移动图层顺序的时候VAOs不用换顺序，只是绘制顺序变一下，所以设置一下绑定，Layer绑定VAos
    QMap<SfsLayer*,QVector<QOpenGLVertexArrayObject*>*> LayerBingVAOs;//用这个来管理VAOs了，暂时不用VAOsLayer管理，主要是VAO_layer是vector，如果要交换顺序的话内部存储空间也需要交换，对于图层顺序调整比较麻烦
private:
    //私有数据
    SfsMap *map;

public:
    //公有函数
    void map2Vao(SfsLayer *layer);//将数据存储到 VAO里面，重绘时只管VAO而不再处理 map数据
    void transform();//进行坐标转换的函数
    void ModelTrans();//进行平移坐标转换
    void ProjectTrans();//进行投影坐标转换
    SfsMap *getMap() const;
    void setMap(SfsMap *value);

private:
    //私有函数
    void getOriginBox(bool MapBox);
    void View2World(SfsPoint *s_pt,QPoint *q_pt);
    void World2View(QPoint *q_pt,SfsPoint *s_pt);



signals:
    void StatsXY(SfsPoint* s_pt,QPoint* q_pt);
    void SetClick(Metadata * data);
    // QOpenGLWidget interface
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

};

#endif // GLWIDGET_H
