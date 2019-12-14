#include "glwidget.h"
#include<sfspolygon.h>

GLwidget::GLwidget(QWidget *parent):QOpenGLWidget(parent)
{
        setAutoFillBackground(false);
        tem_x =0;
        tem_y =0;
        render = nullptr;
        left_bt=false;
        right_bt=false;
        translate = false;
        first =true;
        scale = 1;
        move = false;
        //glEnable(GL_TEXTURE_2D);//����2D��ͼ
        PixelData =(GLubyte*) malloc (1024*768*4*4);
        attrPos = -1;
        //�����������
        //glGenTextures(1,&MapTexture)
        VAOs = new QVector<QOpenGLVertexArrayObject*>();
        VBOs = new QVector<QOpenGLBuffer*>();
        IBOs = new QVector<QOpenGLBuffer*>();

}

GLwidget::~GLwidget()
{
    delete m_shaderProgram;
    delete m_VertexShader;
    delete m_FragmentShader;
}


void GLwidget::initializeGL()
{
    this->makeCurrent();
    initializeOpenGLFunctions();
    m_shaderProgram = new QOpenGLShaderProgram(this);//��ɫ�������ʼ��
    m_VertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_FragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    //��ɫ�����ã����ӣ����ֻҪ����ִ�м���
    m_VertexShader->compileSourceFile("D:\\QtProject\\GeoJSON\\vertexShaderSource.vert");
    m_FragmentShader->compileSourceFile("D:\\QtProject\\GeoJSON\\fragmentShaderSource.frag");
    m_shaderProgram->addShader(m_VertexShader);
    m_shaderProgram->addShader(m_FragmentShader);
    m_shaderProgram->link();


    glClearColor(0.0, 0.2, 0.3, 1.0);//���ñ�����ɫ
    glShadeModel(GL_SMOOTH);//ƽ��
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);//������Ȳ���
    glDepthFunc(GL_LEQUAL);//������Ȳ�������
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//����͸��У��
    qDebug()<<"GLWidget::initializeGL";
}

void GLwidget::resizeGL(int w, int h)
{
    qDebug()<<"GLWidget::resizeGL";
    glViewport(0,0,(GLint)w,(GLint)h);//�����ӿڱ任
    width = w;
    height = h;
}

void GLwidget::paintGL()
{
        if(render!=nullptr&&!move&&first)
        {
            QOpenGLBuffer *VBO;
            glClearColor(0.4, 0.2, 0.9, 0.5);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            getOriginBox(first);
            //��ȡ��ͼ������ݣ�Ȼ������ݽ��л��棬���䱣�浽VBO�ڣ���ÿ�λ��Ƶ�ʱ�򶼵��ü���
            QVector<GLfloat> data;//�洢����
            SfsLayer *layer = render->map->layers->value(0);
            for(int j=0;j<layer->geometries->size();j++)
            {
                //ͼ���ڵ�����ѭ�����õ�ÿһ��Ҫ�صļ������ݣ�Ȼ����Щ���ݻ����VBO��
               if(layer->geometries->value(j)->GeometryType()!=Sfs_Polygon)
                   continue;
               SfsPolygon *polygon =(SfsPolygon *)layer->geometries->value(j);
               for(int i=0;i<polygon->boundaries->value(0)->pts->size();i++)
               {
                   GLfloat x= polygon->boundaries->value(0)->pts->value(i)->x;
                   GLfloat y = polygon->boundaries->value(0)->pts->value(i)->y;
                   data.append(x);
                   data.append(y);
               }
               m_shaderProgram->bind();
               VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
               VBOs->append(VBO);
               QOpenGLVertexArrayObject *VAO = new QOpenGLVertexArrayObject(this);
               VAOs->append(VAO);
               VAO->create();
               VAO->bind();
               VAO->setProperty("data_num",data.size());

               VBO->create();
               VBO->bind();
               VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
               VBO->allocate(data.data(),data.size()*sizeof (float));

               attrPos =  m_shaderProgram->attributeLocation("attrPos");
               m_shaderProgram->setAttributeBuffer(attrPos,GL_FLOAT,0,2,2*sizeof (float));
               m_shaderProgram->enableAttributeArray(attrPos);


               VBO->release();
               VAO->release();//�����
               m_shaderProgram->release();
               data.clear();
               ModelView.setToIdentity();
               Project.setToIdentity();
            }
            first = false;
            transform();
        }
//    ���ݶ�ȡ��ɣ�֮����Ҫ������ͼ���򣬽��л��ƣ�
    if(render!=nullptr)
    {
        glClearColor(0.4, 0.2, 0.9, 0.5);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        m_shaderProgram->bind();
        m_shaderProgram->setUniformValue("color",0.9,0.8,0.7);

        for(int i=0;i<VAOs->size();i++){
            QOpenGLVertexArrayObject *VAO = VAOs->value(i);
            VAO->bind();
           glDrawArrays(GL_LINE_LOOP,0,VAO->property("data_num").toInt()/2);
           VAO->release();
           //m_shaderProgram->release();//����Ҫ���н����������ʹ�õ���һ����ɫ������
        }
        m_shaderProgram->release();
    }

}

void GLwidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        //�ƶ�
        q_Pt1 = event->pos();
        View2World(&S_pt1,&q_Pt1);
        left_bt = true;
    }
}

void GLwidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton&&left_bt==true)
    {
        move = false;
        left_bt = false;
        q_Pt2 = event->pos();
        translate = true;
        View2World(&S_pt2,&q_Pt2);
        change_x += (S_pt2.x-S_pt1.x)*scale ;
        change_y += (S_pt2.y-S_pt1.y)*scale;
        tem_x = 0;
        tem_y = 0;
        ModelTrans();
        update();
    }
}

void GLwidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    change_x = 0;
    change_y = 0;
    scale = 1;
    update();
}

void GLwidget::mouseMoveEvent(QMouseEvent *event)
{
    if(left_bt)
    {
        move_pt = event->pos();
        move = true;
        View2World(&move_spt,&move_pt);
        tem_x = (move_spt.x - S_pt1.x)*scale;
        tem_y = (move_spt.y - S_pt1.y)*scale;
        ModelTrans();
        glClear(GL_COLOR_BUFFER_BIT);
        update();
    }
}

void GLwidget::wheelEvent(QWheelEvent *event)
{
    //���������жϼ��̵Ļ if(event->modifiers()==Qt::ControlModifier)
    //˼·��ָ�����Ŵ�ʵ���Ͼ��ǷŴ�ǰ�������Ӧ�ĵ����ͬһ��ʵ�ʵ㣬���Կ����ȷ�����ƽ�ƣ���¼�Ŵ�ǰ�����꣬Ȼ���¼�Ŵ������꣬��������ƽ��
    QPoint pt1,pt2,pt3;
    pt1 = event->pos();
    pt2 = event->pos();
    SfsPoint spt1,spt2,spt3;
    View2World(&spt1,&pt1);

    if(event->delta()>0)
        scale = scale+0.1;
    else
        scale += -0.1;
    View2World(&spt2,&pt2);
    change_x += (spt2.x-spt1.x)*scale;
    change_y += (spt2.y-spt1.y)*scale;
    translate = true;
    View2World(&spt3,&pt3);
    StatsXY(&spt3,&pt1);
    ModelTrans();
    update();
}

void GLwidget::animate(SfsRender *render)
{
    this->render = render;
    first = true;//ÿ�ζ����µĵ�ͼ�����޸ĵ�ͼ�ķ�Χ��
    update();
//    map2Vao();//ÿ���¶������ݶԵ�ǰ�����ݽ��е���
}

void GLwidget::map2Vao()
{
    if(render!=nullptr&&!move)
    {
        glClearColor(0.4, 0.2, 0.9, 0.5);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        getOriginBox(first);
        //��ȡ��ͼ������ݣ�Ȼ������ݽ��л��棬���䱣�浽VBO�ڣ���ÿ�λ��Ƶ�ʱ�򶼵��ü���
        QVector<GLfloat> data;//�洢����
        SfsLayer *layer = render->map->layers->value(0);
        for(int j=0;j<layer->geometries->size();j++)
           {
            //ͼ���ڵ�����ѭ�����õ�ÿһ��Ҫ�صļ������ݣ�Ȼ����Щ���ݻ����VBO��
           if(layer->geometries->value(j)->GeometryType()!=Sfs_Polygon)
               continue;
           SfsPolygon *polygon =(SfsPolygon *)layer->geometries->value(j);
           for(int i=0;i<polygon->boundaries->value(0)->pts->size();i++)
           {
               GLfloat x= polygon->boundaries->value(0)->pts->value(i)->x;
               GLfloat y = polygon->boundaries->value(0)->pts->value(i)->y;
               data.append(x);
               data.append(y);
           }
           m_shaderProgram->bind();
           QOpenGLBuffer *VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
           VBOs->append(VBO);
           QOpenGLVertexArrayObject *VAO = new QOpenGLVertexArrayObject(this);
           VAOs->append(VAO);
           VAO->create();
           VAO->bind();
           VAO->setProperty("data_num",data.size());

           VBO->create();
           VBO->bind();
           VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
           VBO->allocate(data.data(),data.size()*sizeof (float));

           attrPos =  m_shaderProgram->attributeLocation("attrPos");
           m_shaderProgram->setAttributeBuffer(attrPos,GL_FLOAT,0,2,2*sizeof (float));
           m_shaderProgram->enableAttributeArray(attrPos);

           //�˴�VBO����Ҫrelease ����release�ᵼ�»��ƴ���
           VAO->release();//�����
           VBO->release();
           m_shaderProgram->release();
           data.clear();
        }
        first = false;
    }
    update();

}

void GLwidget::transform()
{
   ModelTrans();
   ProjectTrans();
}

void GLwidget::ModelTrans()
{
    //����ģ������ת��
    ModelView.setToIdentity();
    m_shaderProgram->bind();
    if(translate)//�������ƽ����
        ModelView.translate(change_x,change_y);
    if(move)
        ModelView.translate(change_x+tem_x,change_y+tem_y);

    ModelView.scale(scale,scale);
    m_shaderProgram->setUniformValue("modelview",ModelView);
    m_shaderProgram->release();
    translate = false;

}

void GLwidget::ProjectTrans()
{
    //����ͶӰ����ת��
    Project.ortho(rx,lx,by,ty,0,1);
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("projection",Project);
    m_shaderProgram->release();

}

void GLwidget::getOriginBox(bool MapBox)
{
    //�õ���ͼ����߽磬ֻ�е�һ��ʱ����Ҫ����������ʱ������Ҫ
    if(MapBox)
    {
        lx = render->map->bbox->getLeftX();
        rx = render->map->bbox->getRightX();
        ty = render->map->bbox->getTopY();
        by = render->map->bbox->getBottomY();
        change_x = 0;//��Ļ����x��ƫ��������ʵ�ʵ�ƫ�ƣ�
        change_y = 0;//y��ƫ����������ʵ��ƫ��
    }
}

void GLwidget::View2World(SfsPoint *s_pt, QPoint *q_pt)
{
    //������Ļ������ʵ������ת��
    GLint viewport[4];
      GLdouble modelview[16];//�н�ģ�;���
      GLdouble projection[16];//�н�ͶӰ����
      GLfloat winX, winY, winZ;
      GLdouble object_z;
      int mouse_x = q_pt->x();
      int mouse_y = q_pt->y();




      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(change_x,change_y,0);
      glScalef(scale,scale,1);
      glGetDoublev(GL_MODELVIEW_MATRIX, modelview);




      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(rx,lx,by,ty,1,0);    
      glGetDoublev(GL_PROJECTION_MATRIX, projection);//�������ʣ���������Լ��趨֮�����projection���󣬷���û�У����ǻ���ͼ���Ѿ��о���



      glViewport(0,0,width,height);
      glGetIntegerv(GL_VIEWPORT, viewport);

      winX = (float)mouse_x;
      winY = (float)viewport[3] - (float)mouse_y - 1.0f;
      glReadBuffer(GL_BACK);
      glReadPixels(mouse_x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
      gluUnProject((GLdouble)winX, (GLdouble)winY, (GLdouble)winZ, modelview, projection, viewport, &s_pt->x, &s_pt->y, &object_z);//��ͶӰ������
}

void GLwidget::World2View(QPoint *q_pt, SfsPoint *s_pt)
{
    //������Ļ������ʵ������ת��
    GLint viewport[4];
      GLdouble modelview[16];//�н�ģ�;���
      GLdouble projection[16];//�н�ͶӰ����
      GLfloat winX, winY, winZ;
      GLdouble object_z;
      int mouse_x = q_pt->x();
      int mouse_y = q_pt->y();




      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(change_x,change_y,0);
      glScalef(scale,scale,1);
      glGetDoublev(GL_MODELVIEW_MATRIX, modelview);




      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(rx,lx,by,ty,1,0);
      glGetDoublev(GL_PROJECTION_MATRIX, projection);//�������ʣ���������Լ��趨֮�����projection���󣬷���û�У����ǻ���ͼ���Ѿ��о���



      glViewport(0,0,width,height);
      glGetIntegerv(GL_VIEWPORT, viewport);

      winX = (float)mouse_x;
      winY = (float)viewport[3] - (float)mouse_y - 1.0f;
      glReadBuffer(GL_BACK);
      glReadPixels(mouse_x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
      gluUnProject((GLdouble)winX, (GLdouble)winY, (GLdouble)winZ, modelview, projection, viewport, &s_pt->x, &s_pt->y, &object_z);//��ͶӰ������

}
