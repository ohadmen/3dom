
#ifndef CANVAS_H
#define CANVAS_H


#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include "Qmvp.h"
#include "loader.h"
#include "TrackUtils.h"
#include "TrackBall.h"

#include "Params.h"


class GeometryEngine;

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit Canvas(QWidget *parent=0) :
		QOpenGLWidget(parent),
		m_currentMeshToken(-1)
	{}
	~Canvas()
	{

		// and the buffers.
		makeCurrent();

		
		doneCurrent();
	}
	void setToken(int token) { 
		m_currentMeshToken = token;
		MeshArray::i().getMesh(m_currentMeshToken)->initGL();
	}
	void loadMesh(int token)
	{
		MeshArray::i().getMesh(token)->initGL();
	}
	bool cam2geometry()
	{
		static const float deg2rad = std::acos(0.0f) / 90.0f;
		
		Mesh* p = MeshArray::i().getMesh(m_currentMeshToken);
		if (p == nullptr)
			return false;
		//m_mvp.resetView(width(),height());

		//fit object s.t. if camera is at 0,0,-1 all is visible
		float tanfovH = std::tan(Params::camFOV() / 2 * deg2rad);
		float s = tanfovH / (p->getContainmentRadius()*(1+ tanfovH));


		
		m_tb.resetView(width(), height());
		m_tb.applyT(-p->getCenter()+QVector3D(0,0,-1));
		m_tb.applyS(s);
		return true;
	}
protected:

	void keyPressEvent    (QKeyEvent   *event){m_tb.keyPressEvent    (event); update();}
	void wheelEvent       (QWheelEvent *event){m_tb.wheelEvent       (event); update();}
	void mousePressEvent  (QMouseEvent *event){m_tb.mousePressEvent  (event); update();}
	void mouseReleaseEvent(QMouseEvent *event){m_tb.mouseReleaseEvent(event); update();}
	void mouseMoveEvent   (QMouseEvent *event){m_tb.mouseMoveEvent   (event); update();}

	
	void initializeGL()
	{
		initializeOpenGLFunctions();
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		glClearColor(0, 0, 0, 1);

		glEnable(GL_DEPTH_TEST);
		//
		//glEnable(GL_CULL_FACE);

		
		m_tb.init();
//		auto t = MeshArray::i().getTokenList();
//		for (auto zz : t)
//			MeshArray::i().getMesh(zz)->initGL();

	}

	void resizeGL(int w, int h)
	{

		//m_mvp.setWinSize(w,h);
		m_tb.setWinSize(w, h);
		update();

	}
	void paintGL()
	{
		// Clear color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//QMatrix4x4 matNoScale = m_mvp.getP()*m_mvp.getT()*m_mvp.getR();
		//m_trackUtils.drawSphereIcon(matNoScale, false);
		m_tb.draw();

		Mesh* p = MeshArray::i().getMesh(m_currentMeshToken);
		if (p == nullptr)
			return;
		//p->draw(m_mvp.getMat());
		p->draw(m_tb.getMVP().getMat());




	}


private:


	int m_currentMeshToken;
	
	Trackball m_tb;

};

#endif // Canvas_H
