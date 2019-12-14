#ifndef GLWIDGET_H
#define GLWIDGET_H
#include<qdebug.h>
#include<qopenglwidget.h>
#include<qpainter.h>
#include<sfsrender.h>
#include<gl/GL.h>
#include<qopenglfunctions.h>
#include<gl/GLU.h>
#include<qglfunctions.h>
#include<qevent.h>
#include<QMouseEvent>
#include<qpoint.h>
#include<sfspoint.h>
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


class GLwidget : public QOpenGLWidget,protected QOpenGLFunctions_3_3_Core
{
        Q_OBJECT
public:
    GLwidget(QWidget *parent);
    ~ GLwidget() override;

public slots:
    void animate(SfsRender *render);
public:


    bool translate;//平移判断
    bool first,move;
    bool left_bt,right_bt;//鼠标左右健按下

    double ty,by,lx,rx,change_x,change_y;//map的外接矩阵存储
    double scale;//缩放尺度
    GLint width,height;

    SfsPoint S_pt1,S_pt2;//对应的实际坐标点，
    QPoint q_Pt1,q_Pt2;//屏幕的点，1表示按压，2表示抬起
    QPoint move_pt;//平移过程中的点

    GLubyte* PixelData;//纹理
    GLuint MapTexture;//地图纹理数据

    int attrPos;//着色器程序的数据索引值
    QVector<QOpenGLBuffer *> VBOs,IBOs;// 顶点缓冲，和索引缓冲
    QOpenGLShaderProgram *m_shaderProgram;//着色器程序
    QVector<QOpenGLVertexArrayObject*> VAOs;//顶点数组
    QOpenGLShader *m_FragmentShader,*m_VertexShader;//着色器
    QMatrix4x4 Project,ModelView;//投影矩阵，和模视转换矩阵

private:
    //私有数据
    SfsRender* render;

public:
    //公有函数
    void map2Vao();//将数据存储到 VAO里面，重绘时只管VAO而不再处理 map数据
private:
    //私有函数
    void getOriginBox(bool MapBox);
    void View2World(SfsPoint *s_pt,QPoint *q_pt);
    void World2View(QPoint *q_pt,SfsPoint *s_pt);



signals:
    void StatsXY(SfsPoint* s_pt,QPoint* q_pt);
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
