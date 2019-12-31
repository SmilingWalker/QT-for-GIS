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
    //新建地图,设置范围为最大值
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
    //glEnable(GL_TEXTURE_2D);//开启2D贴图
    PixelData =(GLubyte*) malloc (1024*768*4*4);
    attrPos = -1;
    Selection = false;
    selectChange = false;
    //命名纹理对象
    //glGenTextures(1,&MapTexture)
    VAO_Layer = new  QVector<QVector<QOpenGLVertexArrayObject*>*>();
}

GLwidget::~GLwidget()
{
    //释放地图
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
    m_shaderProgram = new QOpenGLShaderProgram(this);//着色器程序初始化
    m_VertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    m_FragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    //着色器配置，链接，随后只要进行执行即可
    m_VertexShader->compileSourceFile("D:\\QtProject\\GeoJSON\\GLSL\\vertexShaderSource.vert");
    m_FragmentShader->compileSourceFile("D:\\QtProject\\GeoJSON\\GLSL\\fragmentShaderSource.frag");
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
    QTime t;
    t.start();
//    数据读取完成，之后需要交给绘图程序，进行绘制，绘图只需要处理绘图，不需要处理数据
    if(map->layers->size()!=0)
    {
        glClearColor(0.9, 0.9, 0.9, 0.9);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        m_shaderProgram->bind();
        //颜色配置信息
        m_shaderProgram->setUniformValue("color",0.9,0.2,0.5);
        //循环绘画
        for(int j=0;j<map->layers->size();j++){
            SfsLayer *layer = map->layers->value(j);
            if(!layer->getVisible())//如果图层不可见，就跳过
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
               //m_shaderProgram->release();//不需要进行解绑，整个过程使用的是一个着色器程序
            }
        }
        if(Selection){
                int prefix = 0;//前面是否有图层，有图层则附加值，图层要素个数
                m_shaderProgram->setUniformValue("color",0.1,0.1,0.1);
                for (int i=0;i<RetrieveResult.size();i++) {
                    Metadata *data = RetrieveResult.value(i);
                    for(int j=0;j<map->layers->size();j++)
                    {
                        //先匹配图层
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
        //如果开启点选功能则 取消移动功能
        SfsPoint pt;//每次都会建立和析构
        QPoint qpt=event->pos();
        View2World(&pt,&qpt);
        for(int n=0;n<map->layers->size();n++)
        {
            PRtree =map->layers->value(n)->TreeIndex;
            if(PRtree==nullptr)
                break;
            while (true)
            {
                if(!PRtree->isleaf)//如果不是叶子节点
                {
                    //TreePt = PRtree->pt;//根本没有为SfsPoint 定义一个拷贝构造函数
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
                    //找到叶节点 当前的joint是叶节点
                    //找到后需要做两个操作，在界面上表示出来，同时在搜索框呈现信息！自规定，点选启动后，只能选择一个要素，并且每次点选会清除上一次的要素
                    for(int i=0;i<PRtree->Indexes->size();i++)
                    {
//                        RetrieveResult.append(PRtree->Indexes->value(i));//和检索有点区别，这个的元数据是归四叉树索引来管理的不能直接添加需要新建然后再附加
                        Metadata *temp_data = PRtree->Indexes->value(i);
                        Metadata *data = new Metadata();
                        data->layer = temp_data->layer;
                        data->ID = temp_data->ID;
                        data->content = temp_data->content;
                        RetrieveResult.append(data);//检索结果
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
    //一个图层对应一个VAOs，这样便于管理绘制程序
    VAOs = new QVector<QOpenGLVertexArrayObject*>();
    VAO_Layer->append(VAOs);
    LayerBingVAOs[layer] = VAOs;
    //map的范围更新
    map->bbox->setTopY(map->bbox->getTopY()>layer->bbox->getTopY()?map->bbox->getTopY():layer->bbox->getTopY());
    map->bbox->setBottomY(map->bbox->getBottomY()<layer->bbox->getBottomY()?map->bbox->getBottomY():layer->bbox->getBottomY());
    map->bbox->setLeftX(map->bbox->getLeftX()<layer->bbox->getLeftX()?map->bbox->getLeftX():layer->bbox->getLeftX());
    map->bbox->setRightX(map->bbox->getRightX()>layer->bbox->getRightX()?map->bbox->getRightX():layer->bbox->getRightX());
    //first = true;//每次读入新的地图都会修改地图的范围。?是否需要，arcgis没有进行更改，
    map2Vao(layer);//每次新读入数据对当前的数据进行调制
}

void GLwidget::RetrievePaint(QVector<Metadata *> selectNew, QVector<Metadata *> deselect)
{
    //这里如果直接添加，在搜索框被清除之后，就会出现空指针的情况，数据丢失，
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
    //启用选择功能
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
    qDeleteAll(VAOs->begin(),VAOs->end());//清除里面的数据
    VAOs = nullptr;
    LayerBingVAOs.remove(layer);
    map->layers->removeOne(layer);
    delete layer;
    layer = nullptr;
    //修改map的Boundary
    for (int i=0;i<map->layers->size();i++) {
     SfsLayer *layer = map->layers->value(i);
     map->bbox->setTopY(map->bbox->getTopY()>layer->bbox->getTopY()?map->bbox->getTopY():layer->bbox->getTopY());
     map->bbox->setBottomY(map->bbox->getBottomY()<layer->bbox->getBottomY()?map->bbox->getBottomY():layer->bbox->getBottomY());
     map->bbox->setLeftX(map->bbox->getLeftX()<layer->bbox->getLeftX()?map->bbox->getLeftX():layer->bbox->getLeftX());
     map->bbox->setRightX(map->bbox->getRightX()>layer->bbox->getRightX()?map->bbox->getRightX():layer->bbox->getRightX());
    }
    if(map->layers->size()==0)
    {
        //更新为原始值
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
    makeCurrent();//这一句很重要，将当前的OpenGL作为 当前操作的上下文 context，如果不添加这一句就绘制不出来，因为paintGL里是对这个函数有调用的

      //There is no need to call makeCurrent() because this has already been done when this function is called.
     //Before invoking this function, the context and the framebuffer are bound, and the viewport is set up by a call to glViewport(). No other state is set and no clearing or drawing is performed by the framework.
    if(layer!=nullptr&&!move)
    {
        QOpenGLBuffer *VBO;
        glClearColor(0.1, 0.2, 0.9, 0.5);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        getOriginBox(first);
        //读取地图里的数据，然后对数据进行缓存，将其保存到VBO内，在每次绘制的时候都调用即可
        QVector<GLfloat> data;//存储数据,使用vector 初始不判断有多大，实际上vector的数据在内存里也是连续存储的
        GeoType type = layer->getGeometype();
        if(type==Sfs_Polygon)
        {
            //如果是面要素
            for(int j=0;j<layer->geometries->size();j++)
            {
                //图层内的数据循环，得到每一个要素的集合数据，然后将这些数据缓存进VBO内
               //建立索引缓冲对
               QOpenGLBuffer *IBO;
               SfsPolygon *polygon =(SfsPolygon *)layer->geometries->value(j);
                //m_shaderProgram->bind();//实际上在配置VAO和VBO时根本上是不需要着色器绑定的
               VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
               QOpenGLVertexArrayObject *VAO = new QOpenGLVertexArrayObject(this);
               VAOs->append(VAO);
               IBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
               VAO->create();
               VAO->bind();
               //为VAO配置信息，配置点的个数，配置数据类型 vertex_num
               VBO->create();
               VBO->bind();
               IBO->create();
               IBO->bind();
               IBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
               VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
               //获取多边形边数。第一条边为外部边界，此后若有边，则为孔洞边界
                int polygonSize = polygon->boundaries->size();//获取边的条数
                //构建gpc_polygon
                gpc_polygon* gpcPolygon = new gpc_polygon;
                gpcPolygon->num_contours = polygonSize;
                gpcPolygon->contour = new gpc_vertex_list[polygonSize];
                gpcPolygon->hole = new int[polygonSize];

                //为gpcPolygon的hole赋值。外部边界为0， 内部边界为1
                gpcPolygon->hole[0] = 0;//第一个外边界
                for(int i = 1; i < polygonSize; i++){
                    //如果有内边界就赋值
                    gpcPolygon->hole[i] = 1;
                }
                //遍历多边形每一条边，绘制每一条边，并为gpcPolygon赋值
                for(int i = 0; i < polygonSize; i++){
                    //初始化gpcPolygon每一条边
                    SfsLineString* line = polygon->boundaries->at(i);
                    int lineSize = line->pts->size();
                    gpcPolygon->contour[i].num_vertices = lineSize;
                    gpcPolygon->contour[i].vertex = new gpc_vertex[lineSize];
                    //为gpcPolygon的每一条边赋值，为绘制每一条边做准备
                    float* lineVtxs = new float[lineSize * 2];
                    for(int j = 0; j < lineSize; j++){
                        float x = line->pts->at(j)->x;
                        float y = line->pts->at(j)->y;
                        lineVtxs[j * 2] = x;
                        lineVtxs[j * 2 + 1] = y;
                        gpcPolygon->contour[i].vertex[j] = buildVertex(x, y);
                    }
                }
                //三角化gpcPolygon，得到结果数组。
                //初始化gpc_tristrip
                gpc_tristrip* tristrip = new gpc_tristrip;
                tristrip->num_strips = 0;
                tristrip->strip = nullptr;
                gpc_polygon_to_tristrip(gpcPolygon, tristrip);//GL_TRIANGLE_STRIP ，tristrip是一种特殊的三角网，没有记录重复顶点，
                //得到结果为多个tristrip OpenGL函数知道怎么绘制一个tristrip，这里因为每个结果都是一个tristrip 所以必须要灵活的构建，
                //记录 tristrip的个数，每个多边形是由多个tristrip构成，导致每个都要记录
                QVector<int> index;//索引缓冲对象
                int tr_index=0,triangles = 0;

                for (int i=0; i<tristrip->num_strips; i++) {
                    //得到每个tristrip
                    gpc_vertex_list list = tristrip->strip[i];
                    //每一次先获取三个点的位置
                    index.append(tr_index);
                    index.append(tr_index+1);
                    index.append(tr_index+2);
                    data.append(list.vertex[0].x);
                    data.append(list.vertex[0].y);
                    data.append(list.vertex[1].x);
                    data.append(list.vertex[1].y);
                    data.append(list.vertex[2].x);
                    data.append(list.vertex[2].y);
                    tr_index = tr_index+3;//记录的是顶点数组内点的索引值
                    triangles++;
                    for(int j=3;j<list.num_vertices;j++){
                        //每次加入一个点，但是IBO内加入的一个三角形的索引
                        index.append(tr_index-2);
                        index.append(tr_index-1);
                        index.append(tr_index);
                        data.append(list.vertex[j].x);
                        data.append(list.vertex[j].y);
                        tr_index = tr_index+1;//记录的是三角形的数据
                        triangles++;
                    }
                }

                //VAO设置绘制类型和要素个数
                VAO->setProperty("vertex_num",index.size());
                VAO->setProperty("geo_type",QVariant(Sfs_Polygon));
                IBO->allocate(index.data(),index.size()*sizeof (int));//索引缓冲对赋值
                VBO->allocate(data.data(),data.size()*sizeof (float));
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);//这才是配置顶点顶点数据的正确方法，这样配置的属性主要是buffer里的即当前VBO里的数据，同时也需要对于好一个着色器

                //这种配置方法较为灵活，主要是着色器来调动，可以保证万无一失
               // attrPos =  m_shaderProgram->attributeLocation("attrPos");
               // m_shaderProgram->setAttributeBuffer(attrPos,GL_FLOAT,0,2,2*sizeof (float));
               // m_shaderProgram->enableAttributeArray(attrPos);

                VAO->release();//解除绑定
                VBO->release();
                IBO->release();
                delete VBO;
                VBO = nullptr;
                delete IBO;
                IBO = nullptr;
//                VAO->release();//解除绑定
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
               //为VAO配置信息，配置点的个数，配置数据类型 vertex_num
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
               glEnableVertexAttribArray(0);//这才是配置顶点顶点数据的正确方法，这样配置的属性主要是buffer里的即当前VBO里的数据，同时也需要对于好一个着色器
               VAO->release();
               VBO->release();
               delete VBO;
               VBO = nullptr;
               data.clear();
           }
        }
        else if(type==Sfs_Point){
            //只有点数据
            VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
            QOpenGLVertexArrayObject *VAO = new QOpenGLVertexArrayObject(this);
            VAOs->append(VAO);
            VAO->create();
            VAO->bind();
            //为VAO配置信息，配置点的个数，配置数据类型 vertex_num
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
            glEnableVertexAttribArray(0);//这才是配置顶点顶点数据的正确方法，这样配置的属性主要是buffer里的即当前VBO里的数据，同时也需要对于好一个着色器
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
    Project.ortho(lx,rx,by,ty,0,1);
    m_shaderProgram->bind();
    m_shaderProgram->setUniformValue("projection",Project);
    m_shaderProgram->release();
}

void GLwidget::getOriginBox(bool MapBox)
{
    //得到地图的外边界，只有第一次时才需要，其他绘制时不再需要
    if(MapBox)
    {
        Project.setToIdentity();
        ModelView.setToIdentity();
        lx = map->bbox->getLeftX();
        rx = map->bbox->getRightX();
        ty = map->bbox->getTopY();
        by = map->bbox->getBottomY();
        change_x = 0;//屏幕坐标x的偏移量，（实际的偏移）
        change_y = 0;//y的偏移量，都是实际偏移
        scale = 1;
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
      glOrtho(lx,rx,by,ty,1,0);
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
      glOrtho(lx,rx,by,ty,1,0);
      glGetDoublev(GL_PROJECTION_MATRIX, projection);//？？疑问，这里必须自己设定之后才有projection矩阵，否则没有，但是绘制图像已经有矩阵



      glViewport(0,0,width,height);
      glGetIntegerv(GL_VIEWPORT, viewport);

      winX = (float)mouse_x;
      winY = (float)viewport[3] - (float)mouse_y - 1.0f;
      glReadBuffer(GL_BACK);
      glReadPixels(mouse_x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
      gluUnProject((GLdouble)winX, (GLdouble)winY, (GLdouble)winZ, modelview, projection, viewport, &s_pt->x, &s_pt->y, &object_z);//反投影，解算

}
