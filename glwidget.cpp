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
        Selection = false;
        selectChange = false;
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

//    ���ݶ�ȡ��ɣ�֮����Ҫ������ͼ���򣬽��л��ƣ���ͼֻ��Ҫ�����ͼ������Ҫ��������
    if(render!=nullptr)
    {
        glClearColor(0.9, 0.9, 0.9, 0.9);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        m_shaderProgram->bind();
        //��ɫ������Ϣ
        m_shaderProgram->setUniformValue("color",0.5,0.5,0.8);
        //ѭ���滭
        for(int i=0;i<VAOs->size();i++){
            QOpenGLVertexArrayObject *VAO = VAOs->value(i);
            VAO->bind();
           glDrawArrays(GL_LINE_LOOP,0,VAO->property("data_num").toInt()/2);
           VAO->release();
           //m_shaderProgram->release();//����Ҫ���н����������ʹ�õ���һ����ɫ������
        }
        if(Selection){
                int prefix = 0;
                m_shaderProgram->setUniformValue("color",0.1,0.1,0.1);
                for (int i=0;i<RetrieveResult.size();i++) {
                    Metadata *data = RetrieveResult.value(i);
                    for(int j=0;j<render->map->layers->size();j++)
                    {
                        //��ƥ��ͼ��
                        if(data->layer==render->map->layers->value(j))
                        {
                            prefix += 0;
                            break;
                        }
                        else
                            prefix += render->map->layers->value(j)->geometries->size();
                    }
                   QOpenGLVertexArrayObject *VAO = VAOs->value(prefix+data->ID);
                   VAO->bind();
                   glDrawArrays(GL_LINE_LOOP,0,VAO->property("data_num").toInt()/2);
                   VAO->release();
                }
            }
        m_shaderProgram->release();

    }
}

void GLwidget::mousePressEvent(QMouseEvent *event)
{
    if(selectChange)
    {
        clearSelect();
        Selection = true;
        //���������ѡ������ ȡ���ƶ�����
        SfsPoint pt;//ÿ�ζ��Ὠ��������
        QPoint qpt=event->pos();
        View2World(&pt,&qpt);
        for(int n=0;n<render->map->layers->size();n++)
        {
            PRtree = render->map->layers->value(n)->TreeIndex;
            if(PRtree==nullptr)
                break;
            while (true)
            {
                if(!PRtree->isleaf)//�������Ҷ�ӽڵ�
                {
                    //TreePt = PRtree->pt;//����û��ΪSfsPoint ����һ���������캯��
                    if(pt.x>PRtree->pt.x&&pt.y>PRtree->pt.y)
                        PRtree = PRtree->NW;
                    else if(pt.x<PRtree->pt.x&&pt.y>PRtree->pt.y)
                        PRtree = PRtree->NE;
                    else if(pt.x>PRtree->pt.x&&pt.y<PRtree->pt.y)
                        PRtree = PRtree->SW;
                    else if(pt.x<PRtree->pt.x&&pt.y<PRtree->pt.y)
                        PRtree = PRtree->SE;
                }
                else{
                    //�ҵ�Ҷ�ڵ� ��ǰ��joint��Ҷ�ڵ�
                    //�ҵ�����Ҫ�������������ڽ����ϱ�ʾ������ͬʱ�������������Ϣ���Թ涨����ѡ������ֻ��ѡ��һ��Ҫ�أ�����ÿ�ε�ѡ�������һ�ε�Ҫ��
                    for(int i=0;i<PRtree->Indexes->size();i++)
                    {
//                        RetrieveResult.append(PRtree->Indexes->value(i));//�ͼ����е����������Ԫ�����ǹ��Ĳ�������������Ĳ���ֱ�������Ҫ�½�Ȼ���ٸ���
                        Metadata *temp_data = PRtree->Indexes->value(i);
                        Metadata *data = new Metadata();
                        data->layer = temp_data->layer;
                        data->ID = temp_data->ID;
                        data->content = temp_data->content;
                        RetrieveResult.append(data);
                    }
                    break;
                }
            }
        }
        update();
        return;
    }
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
    map2Vao();//ÿ���¶������ݶԵ�ǰ�����ݽ��е���
}

void GLwidget::RetrievePaint(QVector<Metadata *> selectNew, QVector<Metadata *> deselect)
{
    //�������ֱ����ӣ������������֮�󣬾ͻ���ֿ�ָ�����������ݶ�ʧ��
//    qDebug()<<"signal reseive";
    Selection = true;
    for(int j=0;j<deselect.size();j++){
        Metadata* deleteData = deselect.value(j);
        temp.removeOne(deleteData);
    }
    for(int i=0;i<selectNew.size();i++){
        Metadata* insertData = selectNew.value(i);
        temp.push_back(insertData);
    }

    for (int m = 0;m<RetrieveResult.size();m++) {
        delete  RetrieveResult.value(m);
    }
    if(RetrieveResult.size()!=0)
        RetrieveResult.remove(0,RetrieveResult.size());
    for(int k=0;k<temp.size();k++)
    {
        Metadata *temp_data = temp.value(k);
        Metadata *data = new Metadata();
        data->layer = temp_data->layer;
        data->ID = temp_data->ID;
        data->content = temp_data->content;
        RetrieveResult.append(data);
    }
    update();
}

void GLwidget::clearSelect()
{
    temp.remove(0,temp.size());
    for (int m = 0;m<RetrieveResult.size();m++) {
        delete  RetrieveResult.value(m);
    }
    if(RetrieveResult.size()!=0)
        RetrieveResult.remove(0,RetrieveResult.size());
    update();
}

void GLwidget::ChangeSelect()
{
    //����ѡ����
    if(!selectChange)
        selectChange = true;
    else
        selectChange =false;
}

void GLwidget::map2Vao()
{
    makeCurrent();//��һ�����Ҫ������ǰ��OpenGL��Ϊ ��ǰ������������ context������������һ��ͻ��Ʋ���������ΪpaintGL���Ƕ���������е��õ�

      //There is no need to call makeCurrent() because this has already been done when this function is called.
     //Before invoking this function, the context and the framebuffer are bound, and the viewport is set up by a call to glViewport(). No other state is set and no clearing or drawing is performed by the framework.
    if(render!=nullptr&&!move&&first)
    {
        //��һ�ζ������ݵ�ʱ��������
            QOpenGLBuffer *VBO;
            glClearColor(0.4, 0.2, 0.9, 0.5);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            getOriginBox(first);
            //��ȡ��ͼ������ݣ�Ȼ������ݽ��л��棬���䱣�浽VBO�ڣ���ÿ�λ��Ƶ�ʱ�򶼵��ü���
        QVector<GLfloat> data;//�洢����
        SfsLayer *layer = render->map->layers->value(0);//ֻ�ǵ�ͼ�㣬���ڸ���
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
//               m_shaderProgram->bind();//ʵ����������VAO��VBOʱ�������ǲ���Ҫ��ɫ���󶨵�
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

           glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
           glEnableVertexAttribArray(0);//��������ö��㶥�����ݵ���ȷ�������������õ�������Ҫ��buffer��ļ���ǰVBO������ݣ�ͬʱҲ��Ҫ���ں�һ����ɫ��

           //�������÷�����Ϊ����Ҫ����ɫ�������������Ա�֤����һʧ

//               attrPos =  m_shaderProgram->attributeLocation("attrPos");
//               m_shaderProgram->setAttributeBuffer(attrPos,GL_FLOAT,0,2,2*sizeof (float));
//               m_shaderProgram->enableAttributeArray(attrPos);


           VBO->release();
           VAO->release();//�����
//               m_shaderProgram->release();
           data.clear();
           ModelView.setToIdentity();
           Project.setToIdentity();
        }
        first = false;
        transform();
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
    Project.ortho(lx,rx,by,ty,0,1);
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
      glOrtho(lx,rx,by,ty,1,0);
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
      glOrtho(lx,rx,by,ty,1,0);
      glGetDoublev(GL_PROJECTION_MATRIX, projection);//�������ʣ���������Լ��趨֮�����projection���󣬷���û�У����ǻ���ͼ���Ѿ��о���



      glViewport(0,0,width,height);
      glGetIntegerv(GL_VIEWPORT, viewport);

      winX = (float)mouse_x;
      winY = (float)viewport[3] - (float)mouse_y - 1.0f;
      glReadBuffer(GL_BACK);
      glReadPixels(mouse_x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
      gluUnProject((GLdouble)winX, (GLdouble)winY, (GLdouble)winZ, modelview, projection, viewport, &s_pt->x, &s_pt->y, &object_z);//��ͶӰ������

}
