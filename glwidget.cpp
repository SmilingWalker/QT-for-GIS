#include "glwidget.h"
#include<SFS/sfspolygon.h>
#include<GPC/gpc.h>
#include<GPC/gpc.c>
#include<QTime>
gpc_vertex buildVertex(float x, float y)
{
    gpc_vertex vertex;
    vertex.x = x;
    vertex.y = y;
    return vertex;
}
GLwidget::GLwidget(QWidget *parent):QOpenGLWidget(parent)
{
    //�½���ͼ,���÷�ΧΪ���ֵ
    map = new SfsMap();
    map->bbox->setBoundary(DBL_MIN,DBL_MAX,DBL_MAX,DBL_MIN);

    setAutoFillBackground(false);
    tem_x =0;
    tem_y =0;

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
    VAO_Layer = new  QVector<QVector<QOpenGLVertexArrayObject*>*>();
}

GLwidget::~GLwidget()
{
    //�ͷŵ�ͼ
    if(map!=nullptr)
        delete map;
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
    m_VertexShader->compileSourceFile("D:\\QtProject\\GeoJSON\\GLSL\\vertexShaderSource.vert");
    m_FragmentShader->compileSourceFile("D:\\QtProject\\GeoJSON\\GLSL\\fragmentShaderSource.frag");
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
    QTime t;
    t.start();
//    ���ݶ�ȡ��ɣ�֮����Ҫ������ͼ���򣬽��л��ƣ���ͼֻ��Ҫ�����ͼ������Ҫ��������
    if(map->layers->size()!=0)
    {
        glClearColor(0.9, 0.9, 0.9, 0.9);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        m_shaderProgram->bind();
        //��ɫ������Ϣ
        m_shaderProgram->setUniformValue("color",0.9,0.2,0.5);
        //ѭ���滭
        for(int j=0;j<map->layers->size();j++){
            SfsLayer *layer = map->layers->value(j);
            if(!layer->getVisible())//���ͼ�㲻�ɼ���������
                continue;
            VAOs = LayerBingVAOs.find(layer).value();
            for(int i=0;i<VAOs->size();i++){
                QOpenGLVertexArrayObject *VAO = VAOs->value(i);
                VAO->bind();
                if(VAO->property("geo_type").toInt()==Sfs_Polygon)
                {
                    QColor  color = layer->render->getSld()->getFill();
                    m_shaderProgram->setUniformValue("color",color.redF(),color.greenF(),color.blueF());
                    glDrawElements(GL_TRIANGLES,VAO->property("vertex_num").toInt(),GL_UNSIGNED_INT,(void*)0);
                }
                else if(VAO->property("geo_type").toInt()==Sfs_LineString){
                    glLineWidth(layer->render->getSld()->getStroke_width());
                    QColor  color = layer->render->getSld()->getStroke();
                    m_shaderProgram->setUniformValue("color",color.redF(),color.greenF(),color.blueF());
                    glDrawArrays(GL_LINES,0,VAO->property("vertex_num").toInt());
                }
                else if(VAO->property("geo_type").toInt()==Sfs_Point)
                {
                    glPointSize(layer->render->getSld()->getSize());
                    glDrawArrays(GL_POINTS,0,VAO->property("vertex_num").toInt());
                }
                VAO->release();
               //m_shaderProgram->release();//����Ҫ���н����������ʹ�õ���һ����ɫ������
            }
        }
        if(Selection){
                int prefix = 0;//ǰ���Ƿ���ͼ�㣬��ͼ���򸽼�ֵ��ͼ��Ҫ�ظ���
                m_shaderProgram->setUniformValue("color",0.1,0.1,0.1);
                for (int i=0;i<RetrieveResult.size();i++) {
                    Metadata *data = RetrieveResult.value(i);
                    for(int j=0;j<map->layers->size();j++)
                    {
                        //��ƥ��ͼ��
                        if(data->layer==map->layers->value(j))
                        {
                            prefix += 0;
                            break;
                        }
                        else
                        {
                            if(map->layers->value(j)->getGeometype()!=Sfs_Point)
                            prefix += map->layers->value(j)->geometries->size();
                            else
                                prefix += 1;
                        }
                    }
                   QOpenGLVertexArrayObject *VAO = VAOs->value(prefix+data->ID);
                   VAO->bind();
                   if(VAO->property("geo_type").toInt()==Sfs_Polygon)
                   {
                       QColor  color = data->layer->render->getSld()->getFill();
                       m_shaderProgram->setUniformValue("color",152.0/255.0,169.0/255.0,238.0/255.0);
                       glDrawElements(GL_TRIANGLES,VAO->property("vertex_num").toInt(),GL_UNSIGNED_INT,(void*)0);
                   }
                   else if(VAO->property("geo_type").toInt()==Sfs_LineString){
                       glLineWidth(data->layer->render->getSld()->getStroke_width());
                       QColor  color = data->layer->render->getSld()->getStroke();
                       m_shaderProgram->setUniformValue("color",152.0/255.0,169.0/255.0,238.0/255.0);
                       glDrawArrays(GL_LINES,0,VAO->property("vertex_num").toInt());
                   }
                   else if(VAO->property("geo_type").toInt()==Sfs_Point)
                   {
                       glPointSize(data->layer->render->getSld()->getSize());
                        m_shaderProgram->setUniformValue("color",152.0/255.0,169.0/255.0,238.0/255.0);
                       glDrawArrays(GL_POINTS,0,VAO->property("vertex_num").toInt());
                   }
                   VAO->release();
               }

            }
        m_shaderProgram->release();

    }
        qDebug("Rendering elapsed: %d ms", t.elapsed());
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
        for(int n=0;n<map->layers->size();n++)
        {
            PRtree =map->layers->value(n)->TreeIndex;
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
                        RetrieveResult.append(data);//�������
                        if(i!=0)
                        {
                            SetClick(data);
                            break;
                        }
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
        scale = scale*1.1;
    else
        scale =scale*0.9;
    View2World(&spt2,&pt2);
    change_x += (spt2.x-spt1.x)*scale;
    change_y += (spt2.y-spt1.y)*scale;
    translate = true;
    View2World(&spt3,&pt3);
    StatsXY(&spt3,&pt1);
    ModelTrans();
    update();
}

void GLwidget::animate(SfsLayer *layer)
{
    if(map->layers->size()==0)
        first = true;
    map->layers->append(layer);
    //һ��ͼ���Ӧһ��VAOs���������ڹ�����Ƴ���
    VAOs = new QVector<QOpenGLVertexArrayObject*>();
    VAO_Layer->append(VAOs);
    LayerBingVAOs[layer] = VAOs;
    //map�ķ�Χ����
    map->bbox->setTopY(map->bbox->getTopY()>layer->bbox->getTopY()?map->bbox->getTopY():layer->bbox->getTopY());
    map->bbox->setBottomY(map->bbox->getBottomY()<layer->bbox->getBottomY()?map->bbox->getBottomY():layer->bbox->getBottomY());
    map->bbox->setLeftX(map->bbox->getLeftX()<layer->bbox->getLeftX()?map->bbox->getLeftX():layer->bbox->getLeftX());
    map->bbox->setRightX(map->bbox->getRightX()>layer->bbox->getRightX()?map->bbox->getRightX():layer->bbox->getRightX());
    //first = true;//ÿ�ζ����µĵ�ͼ�����޸ĵ�ͼ�ķ�Χ��?�Ƿ���Ҫ��arcgisû�н��и��ģ�
    map2Vao(layer);//ÿ���¶������ݶԵ�ǰ�����ݽ��е���
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

void GLwidget::updateMap()
{
    update();
}

void GLwidget::ZoomToLayer(SfsLayer *layer)
{
    Project.setToIdentity();
    ModelView.setToIdentity();
    change_x = 0;
    change_y = 0;
    scale = 1;
    lx = layer->bbox->getLeftX();
    rx = layer->bbox->getRightX();
    by = layer->bbox->getBottomY();
    ty = layer->bbox->getTopY();
    transform();
    update();
}

void GLwidget::RemoveLayer(SfsLayer *layer)
{
    VAOs = LayerBingVAOs.find(layer).value();
    qDeleteAll(VAOs->begin(),VAOs->end());//������������
    VAOs = nullptr;
    LayerBingVAOs.remove(layer);
    map->layers->removeOne(layer);
    delete layer;
    layer = nullptr;
    //�޸�map��Boundary
    for (int i=0;i<map->layers->size();i++) {
     SfsLayer *layer = map->layers->value(i);
     map->bbox->setTopY(map->bbox->getTopY()>layer->bbox->getTopY()?map->bbox->getTopY():layer->bbox->getTopY());
     map->bbox->setBottomY(map->bbox->getBottomY()<layer->bbox->getBottomY()?map->bbox->getBottomY():layer->bbox->getBottomY());
     map->bbox->setLeftX(map->bbox->getLeftX()<layer->bbox->getLeftX()?map->bbox->getLeftX():layer->bbox->getLeftX());
     map->bbox->setRightX(map->bbox->getRightX()>layer->bbox->getRightX()?map->bbox->getRightX():layer->bbox->getRightX());
    }
    if(map->layers->size()==0)
    {
        //����Ϊԭʼֵ
        map->bbox->setBoundary(DBL_MIN,DBL_MAX,DBL_MAX,DBL_MIN);
        ModelView.setToIdentity();
        Project.setToIdentity();
        change_x = 0;
        change_y = 0;
        scale = 1;
    }
    update();
}

SfsMap *GLwidget::getMap() const
{
    return map;
}

void GLwidget::setMap(SfsMap *value)
{
    map = value;
}

void GLwidget::map2Vao(SfsLayer *layer)
{
    QTime t;
    t.start();
    makeCurrent();//��һ�����Ҫ������ǰ��OpenGL��Ϊ ��ǰ������������ context������������һ��ͻ��Ʋ���������ΪpaintGL���Ƕ���������е��õ�

      //There is no need to call makeCurrent() because this has already been done when this function is called.
     //Before invoking this function, the context and the framebuffer are bound, and the viewport is set up by a call to glViewport(). No other state is set and no clearing or drawing is performed by the framework.
    if(layer!=nullptr&&!move)
    {
        QOpenGLBuffer *VBO;
        glClearColor(0.1, 0.2, 0.9, 0.5);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        getOriginBox(first);
        //��ȡ��ͼ������ݣ�Ȼ������ݽ��л��棬���䱣�浽VBO�ڣ���ÿ�λ��Ƶ�ʱ�򶼵��ü���
        QVector<GLfloat> data;//�洢����,ʹ��vector ��ʼ���ж��ж��ʵ����vector���������ڴ���Ҳ�������洢��
        GeoType type = layer->getGeometype();
        if(type==Sfs_Polygon)
        {
            //�������Ҫ��
            for(int j=0;j<layer->geometries->size();j++)
            {
                //ͼ���ڵ�����ѭ�����õ�ÿһ��Ҫ�صļ������ݣ�Ȼ����Щ���ݻ����VBO��
               //�������������
               QOpenGLBuffer *IBO;
               SfsPolygon *polygon =(SfsPolygon *)layer->geometries->value(j);
                //m_shaderProgram->bind();//ʵ����������VAO��VBOʱ�������ǲ���Ҫ��ɫ���󶨵�
               VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
               QOpenGLVertexArrayObject *VAO = new QOpenGLVertexArrayObject(this);
               VAOs->append(VAO);
               IBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
               VAO->create();
               VAO->bind();
               //ΪVAO������Ϣ�����õ�ĸ����������������� vertex_num
               VBO->create();
               VBO->bind();
               IBO->create();
               IBO->bind();
               IBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
               VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
               //��ȡ����α�������һ����Ϊ�ⲿ�߽磬�˺����бߣ���Ϊ�׶��߽�
                int polygonSize = polygon->boundaries->size();//��ȡ�ߵ�����
                //����gpc_polygon
                gpc_polygon* gpcPolygon = new gpc_polygon;
                gpcPolygon->num_contours = polygonSize;
                gpcPolygon->contour = new gpc_vertex_list[polygonSize];
                gpcPolygon->hole = new int[polygonSize];

                //ΪgpcPolygon��hole��ֵ���ⲿ�߽�Ϊ0�� �ڲ��߽�Ϊ1
                gpcPolygon->hole[0] = 0;//��һ����߽�
                for(int i = 1; i < polygonSize; i++){
                    //������ڱ߽�͸�ֵ
                    gpcPolygon->hole[i] = 1;
                }
                //���������ÿһ���ߣ�����ÿһ���ߣ���ΪgpcPolygon��ֵ
                for(int i = 0; i < polygonSize; i++){
                    //��ʼ��gpcPolygonÿһ����
                    SfsLineString* line = polygon->boundaries->at(i);
                    int lineSize = line->pts->size();
                    gpcPolygon->contour[i].num_vertices = lineSize;
                    gpcPolygon->contour[i].vertex = new gpc_vertex[lineSize];
                    //ΪgpcPolygon��ÿһ���߸�ֵ��Ϊ����ÿһ������׼��
                    float* lineVtxs = new float[lineSize * 2];
                    for(int j = 0; j < lineSize; j++){
                        float x = line->pts->at(j)->x;
                        float y = line->pts->at(j)->y;
                        lineVtxs[j * 2] = x;
                        lineVtxs[j * 2 + 1] = y;
                        gpcPolygon->contour[i].vertex[j] = buildVertex(x, y);
                    }
                }
                //���ǻ�gpcPolygon���õ�������顣
                //��ʼ��gpc_tristrip
                gpc_tristrip* tristrip = new gpc_tristrip;
                tristrip->num_strips = 0;
                tristrip->strip = nullptr;
                gpc_polygon_to_tristrip(gpcPolygon, tristrip);//GL_TRIANGLE_STRIP ��tristrip��һ���������������û�м�¼�ظ����㣬
                //�õ����Ϊ���tristrip OpenGL����֪����ô����һ��tristrip��������Ϊÿ���������һ��tristrip ���Ա���Ҫ���Ĺ�����
                //��¼ tristrip�ĸ�����ÿ����������ɶ��tristrip���ɣ�����ÿ����Ҫ��¼
                QVector<int> index;//�����������
                int tr_index=0,triangles = 0;

                for (int i=0; i<tristrip->num_strips; i++) {
                    //�õ�ÿ��tristrip
                    gpc_vertex_list list = tristrip->strip[i];
                    //ÿһ���Ȼ�ȡ�������λ��
                    index.append(tr_index);
                    index.append(tr_index+1);
                    index.append(tr_index+2);
                    data.append(list.vertex[0].x);
                    data.append(list.vertex[0].y);
                    data.append(list.vertex[1].x);
                    data.append(list.vertex[1].y);
                    data.append(list.vertex[2].x);
                    data.append(list.vertex[2].y);
                    tr_index = tr_index+3;//��¼���Ƕ��������ڵ������ֵ
                    triangles++;
                    for(int j=3;j<list.num_vertices;j++){
                        //ÿ�μ���һ���㣬����IBO�ڼ����һ�������ε�����
                        index.append(tr_index-2);
                        index.append(tr_index-1);
                        index.append(tr_index);
                        data.append(list.vertex[j].x);
                        data.append(list.vertex[j].y);
                        tr_index = tr_index+1;//��¼���������ε�����
                        triangles++;
                    }
                }

                //VAO���û������ͺ�Ҫ�ظ���
                VAO->setProperty("vertex_num",index.size());
                VAO->setProperty("geo_type",QVariant(Sfs_Polygon));
                IBO->allocate(index.data(),index.size()*sizeof (int));//��������Ը�ֵ
                VBO->allocate(data.data(),data.size()*sizeof (float));
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);//��������ö��㶥�����ݵ���ȷ�������������õ�������Ҫ��buffer��ļ���ǰVBO������ݣ�ͬʱҲ��Ҫ���ں�һ����ɫ��

                //�������÷�����Ϊ����Ҫ����ɫ�������������Ա�֤����һʧ
               // attrPos =  m_shaderProgram->attributeLocation("attrPos");
               // m_shaderProgram->setAttributeBuffer(attrPos,GL_FLOAT,0,2,2*sizeof (float));
               // m_shaderProgram->enableAttributeArray(attrPos);

                VAO->release();//�����
                VBO->release();
                IBO->release();
                delete VBO;
                VBO = nullptr;
                delete IBO;
                IBO = nullptr;
//                VAO->release();//�����
                //m_shaderProgram->release();
                data.clear();
                ModelView.setToIdentity();
                Project.setToIdentity();
                gpc_free_tristrip(tristrip);
                gpc_free_polygon(gpcPolygon);
            }
        }
        else if(type==Sfs_LineString)
        {
            for(int j=0;j<layer->geometries->size();j++)
            {
               VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
               QOpenGLVertexArrayObject *VAO = new QOpenGLVertexArrayObject(this);
               VAOs->append(VAO);
               VAO->create();
               VAO->bind();
               //ΪVAO������Ϣ�����õ�ĸ����������������� vertex_num
               VBO->create();
               VBO->bind();
               VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
               SfsLineString *linestring = (SfsLineString*)layer->geometries->value(j);
               for(int i=0;i<linestring->pts->size();i++){
                   data.append(linestring->PointN(i)->x);
                   data.append(linestring->PointN(i)->y);
               }
               VBO->allocate(data.data(),data.size()*sizeof(float));
               VAO->setProperty("vertex_num",data.size()/2);
               VAO->setProperty("geo_type",QVariant(Sfs_LineString));
               glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
               glEnableVertexAttribArray(0);//��������ö��㶥�����ݵ���ȷ�������������õ�������Ҫ��buffer��ļ���ǰVBO������ݣ�ͬʱҲ��Ҫ���ں�һ����ɫ��
               VAO->release();
               VBO->release();
               delete VBO;
               VBO = nullptr;
               data.clear();
           }
        }
        else if(type==Sfs_Point){
            //ֻ�е�����
            VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
            QOpenGLVertexArrayObject *VAO = new QOpenGLVertexArrayObject(this);
            VAOs->append(VAO);
            VAO->create();
            VAO->bind();
            //ΪVAO������Ϣ�����õ�ĸ����������������� vertex_num
            VBO->create();
            VBO->bind();
            VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
            for(int j=0;j<layer->geometries->size();j++){
                SfsPoint *pt =(SfsPoint*)layer->geometries->value(j);
                data.append(pt->x);
                data.append(pt->y);
            }
            VBO->allocate(data.data(),data.size()*sizeof(float));
            VAO->setProperty("vertex_num",data.size()/2);
            VAO->setProperty("geo_type",QVariant(Sfs_Point));
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);//��������ö��㶥�����ݵ���ȷ�������������õ�������Ҫ��buffer��ļ���ǰVBO������ݣ�ͬʱҲ��Ҫ���ں�һ����ɫ��
            VAO->release();
            VBO->release();
            delete VBO;
            VBO = nullptr;
            data.clear();
        }
        ModelView.setToIdentity();
        Project.setToIdentity();
        first = false;
        transform();
    }
    qDebug("Clipping elapsed: %d ms", t.elapsed());
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
        Project.setToIdentity();
        ModelView.setToIdentity();
        lx = map->bbox->getLeftX();
        rx = map->bbox->getRightX();
        ty = map->bbox->getTopY();
        by = map->bbox->getBottomY();
        change_x = 0;//��Ļ����x��ƫ��������ʵ�ʵ�ƫ�ƣ�
        change_y = 0;//y��ƫ����������ʵ��ƫ��
        scale = 1;
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
