
#ifndef CANVAS_H
#define CANVAS_H


#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <qevent.h>
#include "Qmvp.h"
#include "loader.h"
#include "TrackUtils.h"
#include "TrackBall.h"
#include "GLpainter.h"
#include "Backdrop.h"
#include "Params.h"
#include "Zstream.h"


class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
    /*Q_OBJECT*/ //REMOVE AFTER THE FIRST SLOT IS INSERTED

public:
    explicit Canvas(QWidget *parent=0) :
        QOpenGLWidget(parent),
        m_textureType(0),
        m_currentMeshToken(-1),
        m_tb(this)
    {
        setAcceptDrops(true);
        setMouseTracking(true);
    }
    ~Canvas()
    {

        // and the buffers.
        makeCurrent();
        doneCurrent();
    }
    void setToken(int token) {
        m_currentMeshToken = token;

        //draw normals 

    }

    
    bool resetView()
    {
        static const float deg2rad = std::acos(0.0f) / 90.0f;
        
        //Mesh* p = MeshArray::i().getMesh(m_currentMeshToken);
        //if (p == nullptr)
        //    return false;
        //m_mvp.resetView(width(),height());
    
        //fit object s.t. if camera is at 0,0,-1 all is visible
        float tanfovH = std::tan(Params::camFOV() / 2 * deg2rad);
        
        //float s = tanfovH / (p->getContainmentRadius()*(1+ tanfovH));
        
        float s = tanfovH / (Zstream::i().getContainmentRadius()*(1 + tanfovH));
        

    
        
        m_tb.resetView(width(), height());
        //m_tb.applyT(-p->getCenter(),false);
        //m_tb.applyT(Zstream::i().getCenter(), false);
        m_tb.applyR(QVector3D(1, 0, 0), 180);
        m_tb.applyS(s);
        update();
        return true;
    }
    void setTextureType(int v)
    {
        m_textureType = v;
    }
  
protected:

    void wheelEvent           (QWheelEvent *event){m_tb.wheelEvent       (event); update();}
    void mousePressEvent      (QMouseEvent *event){m_tb.mousePressEvent  (event); update();}
    void mouseReleaseEvent    (QMouseEvent *event){m_tb.mouseReleaseEvent(event); update();}
    void mouseMoveEvent       (QMouseEvent *event)    {        m_tb.mouseMoveEvent   (event); update();}
    void mouseDoubleClickEvent(QMouseEvent *event) {
        m_tb.mouseDoubleClickEvent(event); update();    }
    void keyPressEvent(QKeyEvent *e) { m_tb.keyPressEvent(e); update();}
    void keyReleaseEvent(QKeyEvent *e) { m_tb.keyReleaseEvent(e);update(); }

    
    void initializeGL()
    {

        initializeOpenGLFunctions();
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        setFocusPolicy(Qt::StrongFocus);
        glEnable(GL_BLEND);
        glEnable(GL_LINE_SMOOTH);
        glClearColor(0, 0, 0, 1);

        glEnable(GL_DEPTH_TEST);
        //
        //glEnable(GL_CULL_FACE);

        
        m_tb.init(&m_currentMeshToken);
        GLpainter::i().init(this);
        m_bg.init();
        Zstream::i().setDrawingWidget(this);
        

    }

    void resizeGL(int w, int h)
    {

        //m_mvp.setWinSize(w,h);
        m_tb.setWinSize(w, h);
        update();

    }
    void paintGL()
    {
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clear color and depth buffer
        glEnable(GL_DEPTH_TEST);



        // Blended points, lines, and polygons.
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //
        //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        //
        //glEnable(GL_POINT_SMOOTH);
        //glEnable(GL_LINE_SMOOTH);
        //glEnable(GL_POLYGON_SMOOTH);



        QMatrix4x4 mvp = m_tb.getMVP().getMat();

        //Mesh* p = MeshArray::i().getMesh(m_currentMeshToken);
        //if (p != nullptr)
        //    p->draw(mvp, m_textureType);
        if(Zstream::i().isValid())
            Zstream::i().draw(mvp, m_textureType);
        m_bg.draw();
        

        m_tb.draw();

        //GLpainter::i().draw(m_tb.getMVP());
        GLpainter::i().draw(m_tb.getMVP());
    }
   

   void dragEnterEvent(QDragEnterEvent *event)
   {

       if (event->mimeData()->hasUrls())
       {
           auto urls = event->mimeData()->urls();
           if (urls.size() == 1 && urls.front().path().endsWith(".stl"))
               event->acceptProposedAction();
       }
   }
   void dropEvent(QDropEvent *event)
   {
       QString meshfn = event->mimeData()->urls().front().toLocalFile();
       

   }
  
private:

 

    int m_currentMeshToken;
    
    int m_textureType;
    Trackball m_tb;
    Backdrop m_bg;

    

};

#endif // Canvas_H

