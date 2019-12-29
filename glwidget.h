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
    QVector<Metadata *> RetrieveResult;//ʵ�ʱ���
    QVector<Metadata *> temp;//��ʱ����
    bool translate;//ƽ���ж�
    bool first,move;
    bool left_bt,right_bt;//������ҽ�����

    double ty,by,lx,rx,change_x,change_y;//
    double tem_x,tem_y;//ƽ�ƹ����е���ʱ�����
    double scale;//���ų߶�
    GLint width,height;

    SfsPoint S_pt1,S_pt2;//��Ӧ��ʵ������㣬
    QPoint q_Pt1,q_Pt2;//��Ļ�ĵ㣬1��ʾ��ѹ��2��ʾ̧��
    QPoint move_pt;//ƽ�ƹ����еĵ�
    SfsPoint move_spt;//ƽ���е�ʵ�������

    GLubyte* PixelData;//����
    GLuint MapTexture;//��ͼ��������

    int attrPos;//��ɫ���������������ֵ
//    QVector<QOpenGLBuffer *> *VBOs,*IBOs;// ���㻺�壬���������� //��Щ�����ǲ���Ҫ�ģ���Ϊ��ʵ�ʹ����У����ݶ���󶨵�VAO�ڲ���������ȥ���ڴ��ж�ȡIBO��VBO��ֵ
    QOpenGLShaderProgram *m_shaderProgram;//��ɫ������
    QVector<QOpenGLVertexArrayObject*> *VAOs;//��������,(���˸о�Ӧ�ò��ܰ����е�ͼ�����ݶ����õ�һ��VAOs��Ͼ������ϵĲ����������ͼ��ģ�����ÿһ��ͼ����һ��VAOs�Ŷԣ�����ֱ�ӽ���һ��VAOs��������
    QVector<QVector<QOpenGLVertexArrayObject*>*> *VAO_Layer;
    QOpenGLShader *m_FragmentShader,*m_VertexShader;//��ɫ��
    QMatrix4x4 Project,ModelView;//ͶӰ���󣬺�ģ��ת������
    bool selectChange;
    PRQuadTree *PRtree;
    //��Ҫһ�������ݣ���layer ��VAOs�󶨣�ʵ�����ƶ�ͼ��˳���ʱ��VAOs���û�˳��ֻ�ǻ���˳���һ�£���������һ�°󶨣�Layer��VAos
    QMap<SfsLayer*,QVector<QOpenGLVertexArrayObject*>*> LayerBingVAOs;//�����������VAOs�ˣ���ʱ����VAOsLayer������Ҫ��VAO_layer��vector�����Ҫ����˳��Ļ��ڲ��洢�ռ�Ҳ��Ҫ����������ͼ��˳������Ƚ��鷳
private:
    //˽������
    SfsMap *map;

public:
    //���к���
    void map2Vao(SfsLayer *layer);//�����ݴ洢�� VAO���棬�ػ�ʱֻ��VAO�����ٴ��� map����
    void transform();//��������ת���ĺ���
    void ModelTrans();//����ƽ������ת��
    void ProjectTrans();//����ͶӰ����ת��
    SfsMap *getMap() const;
    void setMap(SfsMap *value);

private:
    //˽�к���
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
