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
        //glEnable(GL_TEXTURE_2D);//开启2D贴图
        PixelData =(GLubyte*) malloc (1024*768*4*4);
        attrPos = -1;
        //命名纹理对象
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
    m_shaderProgram = new QOpenGLShaderProgram(this);//着色器程序初始化
    m_VertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_FragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    //着色器配置，链接，随后只要进行执行即可
    m_VertexShader->compileSourceFile("D:\\QtProject\\GeoJSON\\vertexShaderSource.vert");
    m_FragmentShader->compileSourceFile("D:\\QtProject\\GeoJSON\\fragmentShaderSource.frag");
    m_shaderProgram->addShader(m_VertexShader);
    m_shaderProgram->addShader(m_FragmentShader);
    m_shaderProgram->link();


    glClearColor(0.0, 0.2, 0.3, 1.0);//设置背景颜色
    glShadeModel(GL_SMOOTH);//平滑
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);//允许深度测试
    glDepthFunc(GL_LEQUAL);//设置深度测试类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正
    qDebug()<<"GLWidget::initializeGL";
}

void GLwidget::resizeGL(int w, int h)
{
    qDebug()<<"GLWidget::resizeGL";
    glViewport(0,0,(GLint)w,(GLint)h);//设置视口变换
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
            //读取地图里的数据，然后对数据进行缓存，将其保存到VBO内，在每次绘制的时候都调用即可
            QVector<GLfloat> data;//存储数据
            SfsLayer *layer = render->map->layers->value(0);
            for(int j=0;j<layer->geometries->size();j++)
            {
                //图层内的数据循环，得到每一个要素的集合数据，然后将这些数据缓存进VBO内
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
               VAO->release();//解除绑定
               m_shaderProgram->release();
               data.clear();
               ModelView.setToIdentity();
               Project.setToIdentity();
            }
            first = false;
            transform();
        }
//    数据读取完成，之后需要交给绘图程序，进行绘制，
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
           //m_shaderProgram->release();//不需要进行解绑，整个过程使用的是一个着色器程序
        }
        m_shaderProgram->release();
    }

}

void GLwidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        //移动
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
    //可以用来判断键盘的活动 if(event->modifiers()==Qt::ControlModifier)
    //思路，指定鼠标放大，实际上就是放大前后鼠标点对应的点就是同一个实际点，所以可以先放缩在平移，记录放大前的坐标，然后记录放大后的坐标，进行坐标平移
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
    first = true;//每次读入新的地图都会修改地图的范围。
    update();
//    map2Vao();//每次新读入数据对当前的数据进行调制
}

void GLwidget::map2Vao()
{
    if(render!=nullptr&&!move)
    {
        glClearColor(0.4, 0.2, 0.9, 0.5);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        getOriginBox(first);
        //读取地图里的数据，然后对数据进行缓存，将其保存到VBO内，在每次绘制的时候都调用即可
        QVector<GLfloat> data;//存储数据
        SfsLayer *layer = render->map->layers->value(0);
        for(int j=0;j<layer->geometries->size();j++)
           {
            //图层内的数据循环，得到每一个要素的集合数据，然后将这些数据缓存进VBO内
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

           //此处VBO不需要release 并且release会导致绘制错误
           VAO->release();//解除绑定
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
    //进行模视坐标转换
    ModelView.setToIdentity();
    m_shaderProgram->bind();
    if(translate)//如果进行平移了
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
    //进行投影坐标转换
    Project.ortho(rx,lx,by,ty,0,1);
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("projection",Project);
    m_shaderProgram->release();

}

void GLwidget::getOriginBox(bool MapBox)
{
    //得到地图的外边界，只有第一次时才需要，其他绘制时不再需要
    if(MapBox)
    {
        lx = render->map->bbox->getLeftX();
        rx = render->map->bbox->getRightX();
        ty = render->map->bbox->getTopY();
        by = render->map->bbox->getBottomY();
        change_x = 0;//屏幕坐标x的偏移量，（实际的偏移）
        change_y = 0;//y的偏移量，都是实际偏移
    }
}

void GLwidget::View2World(SfsPoint *s_pt, QPoint *q_pt)
{
    //用于屏幕坐标向实际坐标转换
    GLint viewport[4];
      GLdouble modelview[16];//承接模型矩阵
      GLdouble projection[16];//承接投影矩阵
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
      glGetDoublev(GL_PROJECTION_MATRIX, projection);//？？疑问，这里必须自己设定之后才有projection矩阵，否则没有，但是绘制图像已经有矩阵



      glViewport(0,0,width,height);
      glGetIntegerv(GL_VIEWPORT, viewport);

      winX = (float)mouse_x;
      winY = (float)viewport[3] - (float)mouse_y - 1.0f;
      glReadBuffer(GL_BACK);
      glReadPixels(mouse_x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
      gluUnProject((GLdouble)winX, (GLdouble)winY, (GLdouble)winZ, modelview, projection, viewport, &s_pt->x, &s_pt->y, &object_z);//反投影，解算
}

void GLwidget::World2View(QPoint *q_pt, SfsPoint *s_pt)
{
    //用于屏幕坐标向实际坐标转换
    GLint viewport[4];
      GLdouble modelview[16];//承接模型矩阵
      GLdouble projection[16];//承接投影矩阵
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
      glGetDoublev(GL_PROJECTION_MATRIX, projection);//？？疑问，这里必须自己设定之后才有projection矩阵，否则没有，但是绘制图像已经有矩阵



      glViewport(0,0,width,height);
      glGetIntegerv(GL_VIEWPORT, viewport);

      winX = (float)mouse_x;
      winY = (float)viewport[3] - (float)mouse_y - 1.0f;
      glReadBuffer(GL_BACK);
      glReadPixels(mouse_x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
      gluUnProject((GLdouble)winX, (GLdouble)winY, (GLdouble)winZ, modelview, projection, viewport, &s_pt->x, &s_pt->y, &object_z);//反投影，解算

}
