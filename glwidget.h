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


    bool translate;//ƽ���ж�
    bool first,move;
    bool left_bt,right_bt;//������ҽ�����

    double ty,by,lx,rx,change_x,change_y;//map����Ӿ���洢
    double scale;//���ų߶�
    GLint width,height;

    SfsPoint S_pt1,S_pt2;//��Ӧ��ʵ������㣬
    QPoint q_Pt1,q_Pt2;//��Ļ�ĵ㣬1��ʾ��ѹ��2��ʾ̧��
    QPoint move_pt;//ƽ�ƹ����еĵ�

    GLubyte* PixelData;//����
    GLuint MapTexture;//��ͼ��������

    int attrPos;//��ɫ���������������ֵ
    QVector<QOpenGLBuffer *> VBOs,IBOs;// ���㻺�壬����������
    QOpenGLShaderProgram *m_shaderProgram;//��ɫ������
    QVector<QOpenGLVertexArrayObject*> VAOs;//��������
    QOpenGLShader *m_FragmentShader,*m_VertexShader;//��ɫ��
    QMatrix4x4 Project,ModelView;//ͶӰ���󣬺�ģ��ת������

private:
    //˽������
    SfsRender* render;

public:
    //���к���
    void map2Vao();//�����ݴ洢�� VAO���棬�ػ�ʱֻ��VAO�����ٴ��� map����
private:
    //˽�к���
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
