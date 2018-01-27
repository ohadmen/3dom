#include "QGLMainWindow.h"

void f1(QPlane3D *value)
{
}

CALDLMainWindow::CALDLMainWindow(QScreen *targetScreen)
    : QWindow(targetScreen)
{
    setSurfaceType(QWindow::OpenGLSurface);
    QSurfaceFormat format;
    format.setSamples     (32);
    format.setSwapInterval(0);
    setFormat(format);
    create();
    _context = new QOpenGLContext(this);
    _context->setFormat(format);
    _context->create();

    setGeometry(40, 40, 1280, 900);

    _mode = 0;

    _camera[0].setPlanes(1.0f, 100.0f);
    _camera[0].setFieldOfView(30);
    _camera[0].setSize  (width(), height());
    _camera[0].setPos   (QVector3D(2.0, 5.0, 15.0));

    _camera[1].setPlanes(1.0f, 5.0f);
    _camera[1].setSize(width(), height());
    _camera[1].setPos(QVector3D(1.0, 1.0, 3.0));

//-------------
    _p[0].redefine(QVector3D(0.0f, 0.0f, 1.0f).normalized(), 1.0f);


//    _sp = f2(2.0, 12, 12);
    _sph = QPaintedSphere3D(QVector3D(0.0f, 0.0f, 0.0), 1, 16, 16);
//    _sph.createVertices(12, 12);

//    _pp = QPaintedPlane3D(QVector3D(0.0f, 0.0f, 1.0f).normalized(), 0.5f);
//    _pp.createVertices(4, 4, QSize(20, 20));



//    QPaintedPlane3D *p1 = new QPaintedPlane3D(QVector3D(0.0f, 0.0f, 1.0f).normalized(), 0.5f);
//    _list << p1;
}

CALDLMainWindow::~CALDLMainWindow()
{
    for(auto i : _list)delete i;
}

void CALDLMainWindow::resizeEvent(QResizeEvent *value)
{
    Q_UNUSED(value)
    _context->makeCurrent(this);
    glViewport(0.0f, 0.0f, static_cast<float>(width()), static_cast<float>(height()));

    _camera[0].setSize(width(), height());
    _camera[1].setSize(width(), height());
    
    paint();
}

void CALDLMainWindow::exposeEvent(QExposeEvent *value)
{
    Q_UNUSED(value)
    if(isExposed())paint();
}

void CALDLMainWindow::keyPressEvent(QKeyEvent *value)
{
    switch(value->key())
    {
        case Qt::Key_F1 :
        {
            _mode --;
            if(_mode < 0)_mode = 0;
            break;
        }
        case Qt::Key_F2 :
        {
            _mode ++;
            if(_mode > 1)_mode = 1;
            break;
        }
    }
    paint();
}

void CALDLMainWindow::mousePressEvent(QMouseEvent *value)
{
//    Q_UNUSED(value)
    _mouseLastPos = value->pos();
    if(value->buttons() == Qt::LeftButton)
    {
        _camera[_mode].rayFromScene(QVector2D(value->pos().x(), value->pos().y()) , _v[0], _v[1]);
        _r[0].setOrigin   (_v[0]);
        _r[0].setDirection((_v[1] - _v[0]).normalized());


//        _v[5] = f3(QVector3D(), 2, _r[0]);
//        _p[0].redefine(_v[5].normalized(), _p[0].distance());


//        QGeometry3D::normal(_r[0], _p[0].normal() * 2, _v[5], _v[6]);

        _v[5] = QGeometry3D::normal(_r[0], _p[0].normal() * 2);
        _v[6] = _p[0].normal() * 2;
        _p[0].redefine(_v[5].normalized(), _p[0].distance());

    }else
    if(value->buttons() == Qt::RightButton)
    {
        _camera[_mode].rayFromScene(QVector2D(value->pos().x(), value->pos().y()) , _v[2], _v[3]);

//        _r[1].setOrigin   (_v[2]);
//        _r[1].setDirection((_v[3] - _v[2]).normalized());
//        _r[2].setOrigin   (QVector3D(0.0f, 0.0f, 0.0f));
//        _r[2].setDirection(QVector3D(1.0f, 0.0f, 0.0f).normalized());
//        QGeometry3D::normal(_r[1], _r[2], _v[4], _v[5]);
//        _p[0].redefine((_v[5] - _v[4]).normalized(), _v[4]);
//        _p[1].redefine(QVector3D(1.0f, 1.0f, 0.0f).normalized(), 0.2f);
//        _l[1] = QLine3D(_r[1].point(10), _r[1].point(20));

    }
    paint();
}

void CALDLMainWindow::mouseMoveEvent(QMouseEvent *value)
{
    
    int dx = value->x() - _mouseLastPos.x();
    int dy = value->y() - _mouseLastPos.y();
    if(value->buttons() == Qt::LeftButton)
    {
        _camera[_mode].rayFromScene(QVector2D(value->pos().x(), value->pos().y()) , _v[0], _v[1]);
        _r[0].setOrigin   (_v[0]);
        _r[0].setDirection((_v[1] - _v[0]).normalized());


        _v[5] = QGeometry3D::normal(_r[0], _p[0].normal() * 2);
        _v[6] = _p[0].normal() * 2;
        _p[0].redefine(_v[5].normalized(), _p[0].distance());


//        _pp.redefine(_v[5].normalized(), _pp.distance());
//        _pp.updateMatrix();

//        if(!_list.isEmpty())((QPaintedPlane3D*)_list[0])->redefine(_v[5].normalized(), ((QPaintedPlane3D*)_list[0])->distance());
        if(!_list.isEmpty())((QPaintedPlane3D*)_list[0])->setNormal(_v[5].normalized());


    }else
    if(value->buttons() == Qt::RightButton)
    {
    }else
    if(value->buttons() == Qt::MiddleButton)
    {
        QVector3D pos = _camera[_mode].pos();
        float x = -dx * 0.25f;
        float y = -dy * 0.5f;
        QVector3D v1(0.0f, 1.0f, 0.0f);
        _camera[_mode].rotateAroundVector(v1, x);
        QVector3D v2(1.0f, 0.0f, 0.0f);
        _camera[_mode].rotateAroundVector(v2, y);
    }
    _mouseLastPos = value->pos();
    paint();
}

void CALDLMainWindow::wheelEvent(QWheelEvent *value)
{
    (void)(value);
//    float d = value->delta() > 0 ? 0.5f : -0.5f;
    paint();
}

void CALDLMainWindow::paint()
{
    _context->makeCurrent(this);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear    (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable   (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable   (GL_DEPTH_TEST);


//*****************************************************************************

    glMatrixMode (GL_PROJECTION);
    glLoadMatrixf(_camera[_mode].mViewProj().constData());

//*****************************************************************************

    QPainter3D painter;


//*****************************************************************************
    float w = 2.0f;
    glBegin(GL_LINES);

    painter.setColor(QColor(255, 0, 0, 255));
    painter.paint(QVector3D(-w,  0.0f,  0.0f), QVector3D(w, 0.0f, 0.0f), false);
    painter.setColor(QColor(0, 255, 0, 255));
    painter.paint(QVector3D(0.0f, -w,  0.0f), QVector3D(0.0f, w, 0.0f), false);
    painter.setColor(QColor(255, 0, 255, 255));
    painter.paint(QVector3D(0.0f, 0.0f, -w), QVector3D(0.0f, 0.0f, w), false);
    painter.paint(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, w), false);

    glEnd();

//*****************************************************************************
//    _sph.paint();
//    _pp.paint();

    for(auto i : _list)i->paint();

#if 0
    glBegin(GL_TRIANGLES);
    glColor4f(0, 1, 0, 1);
//    glBegin(GL_POINTS);
    for(int i = 0; i<_sp.size()-3; i++)
    {
        glVertex3f(_sp[i].x(), _sp[i].y(), _sp[i].z()); 
        glVertex3f(_sp[i+1].x(), _sp[i+1].y(), _sp[i+1].z()); 
        glVertex3f(_sp[i+2].x(), _sp[i+2].y(), _sp[i+2].z()); 
    }
    glEnd();
#endif


#if 1
    painter.setColor(QColor(255, 255, 0, 255));
    painter.setLineWidth(1.0f);
    painter.paint(_v[0], _v[1]);


//    painter.paint(_v[2], _v[3]);
    painter.setPointSize(5.0f);
//    painter.paint(_v[0]); 
//    painter.paint(_v[1]);
//    painter.paint(_v[2]); 
//    painter.paint(_v[3]);


    painter.setColor(QColor(255, 0, 0, 255));
//    painter.paint(_v[4], _v[5]);
//    painter.paint(_v[4]);
    painter.paint(_v[5]);
    painter.paint(_v[6]);


//    painter.paint(_v[7]);
//    painter.paint(_v[8]);
    painter.paint(_v[5], _v[6]);

    painter.paint(_l[0]);
    painter.paint(_l[1]);
#endif

//*****************************************************************************

#if 0
    glBegin(GL_LINES);
        painter.setColor(QColor(255, 255, 255, 30));
        painter.paint(QVector3D(-1.0f,  1.0f, -1.0f), QVector3D( 1.0f,  1.0f, -1.0f), false);
        painter.paint(QVector3D( 1.0f,  1.0f, -1.0f), QVector3D( 1.0f, -1.0f, -1.0), false);
        painter.paint(QVector3D( 1.0f, -1.0f, -1.0f), QVector3D(-1.0f, -1.0f, -1.0f), false);
        painter.paint(QVector3D(-1.0f, -1.0f, -1.0f), QVector3D(-1.0f,  1.0f, -1.0f), false);
        painter.paint(QVector3D(-1.0f,  1.0f,  1.0f), QVector3D( 1.0f,  1.0f,  1.0f), false);
        painter.paint(QVector3D( 1.0f,  1.0f,  1.0f), QVector3D( 1.0f, -1.0f,  1.0), false);
        painter.paint(QVector3D( 1.0f, -1.0f,  1.0f), QVector3D(-1.0f, -1.0f,  1.0f), false);
        painter.paint(QVector3D(-1.0f, -1.0f,  1.0f), QVector3D(-1.0f,  1.0f,  1.0f), false);
        painter.paint(QVector3D(-1.0f,  1.0f, -1.0f), QVector3D(-1.0f,  1.0f,  1.0f), false);
        painter.paint(QVector3D( 1.0f,  1.0f, -1.0f), QVector3D( 1.0f,  1.0f,  1.0f), false);
        painter.paint(QVector3D( 1.0f, -1.0f, -1.0f), QVector3D( 1.0f, -1.0f,  1.0f), false);
        painter.paint(QVector3D(-1.0f, -1.0f, -1.0f), QVector3D(-1.0f, -1.0f,  1.0f), false);
    glEnd();
#endif

//*****************************************************************************
#if 0
    painter.setColor(QColor(255, 255, 55, 255));
    painter.paint(_p[0], 5.0f, 0.1f);
//    painter.paint(_p[1], 5.0f, 0.1f);

//    painter.paint(_camera[1]._p[0], 5.0f, 0.1f);
//    painter.paint(_camera[1]._p[1], 5.0f, 0.1f);
//    painter.paint(_camera[1]._p[2], 5.0f, 0.1f);
//    painter.paint(_camera[1]._p[3], 5.0f, 0.1f);
#endif
//*****************************************************************************
#if 0
    painter.setColor(QColor(55, 55, 255, 255));
    painter.setLineWidth(2.0f);

    glBegin(GL_LINES);
    painter.paint(_camera[1]._v[0], _camera[1]._v[1], false);
    painter.paint(_camera[1]._v[1], _camera[1]._v[2], false);
    painter.paint(_camera[1]._v[2], _camera[1]._v[3], false);
    painter.paint(_camera[1]._v[3], _camera[1]._v[0], false);
    painter.paint(_camera[1]._v[4], _camera[1]._v[5], false);
    painter.paint(_camera[1]._v[5], _camera[1]._v[6], false);
    painter.paint(_camera[1]._v[6], _camera[1]._v[7], false);
    painter.paint(_camera[1]._v[7], _camera[1]._v[4], false);
    painter.paint(_camera[1]._v[0], _camera[1]._v[4], false);
    painter.paint(_camera[1]._v[1], _camera[1]._v[5], false);
    painter.paint(_camera[1]._v[2], _camera[1]._v[6], false);
    painter.paint(_camera[1]._v[3], _camera[1]._v[7], false);
    glEnd();
#endif

//*****************************************************************************

    _context->swapBuffers(this);
}