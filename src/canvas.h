#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QDebug>
#include <iostream>
#include <cmath>

#include "backdrop.h"
#include "glmesh.h"
#include "mesh.h"
#include "Trackball.h"


class Canvas : public QGLWidget, protected QGLFunctions
{
	Q_OBJECT

public:
	Canvas(const QGLFormat& format, QWidget *parent = 0)
		: QGLWidget(format, parent), m_mesh(),
		 anim(this, "perspective"), m_status(" ")
	{
		QFile styleFile("style.qss");
		styleFile.open(QFile::ReadOnly);
		setStyleSheet(styleFile.readAll());
		setFocusPolicy(Qt::StrongFocus);//catching keyboard events
		setMouseTracking(true);
		anim.setDuration(100);

	}

	~Canvas()
	{


	}




	void initializeGL()
	{

		m_mesh.glinit();
		//m_backdrop.glinit();



	}
	void paintEvent(QPaintEvent *event)
	{
		Q_UNUSED(event);

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		//m_backdrop.draw();


		m_mesh.draw(privTmat(), privVmat());





		//glPointSize(10);
		//glColor3f(1, 1, .5);
		//glBegin(GL_POINTS);
		//glVertex3f(.5, 0, 0);
		//glEnd();




		//DrawCircle(0.1, 0.5, 0, 0.1);

		if (m_status.isNull())    return;

		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.drawText(10, height() - 10, m_status);
	}



	void view_orthographic()
	{
		view_anim(0);
	}

	void view_perspective()
	{
		view_anim(0.25);
	}


	public slots:
	void set_status(const QString &s)
	{
		m_status = s;
		update();
	}

	void clear_status()
	{
		m_status = "";
		update();
	}
	void load_mesh(int token, bool is_reload)
	{

		m_meshDataP = MeshArray::i().getToken(token);

		m_mesh.set(*m_meshDataP);

		if (!is_reload)
		{
	
			QVector3D lower(m_meshDataP->xmin(), m_meshDataP->ymin(), m_meshDataP->zmin());
			QVector3D upper(m_meshDataP->xmax(), m_meshDataP->ymax(), m_meshDataP->zmax());
			m_center = (lower + upper) / 2;
			//TODO : use privFOV to calculate init distance
			m_eye = m_center + QVector3D(0, 0,m_meshDataP->ymin()*4);
			m_upvec = QVector3D(0, 1, 0);
		


		}

		update();
	

	}


protected:



	void mouseDoubleClickEvent(QMouseEvent* event)
	{
	

		

	}
	void keyPressEvent(QKeyEvent * event)
	{
	

	}

	void mousePressEvent(QMouseEvent *e) {
		m_tb.callback_mouseDown(e->x(), height() - e->y(),  e->button(), e->modifiers());
		//   if(e->button() == Qt::LeftButton)
		//trackball.MouseDown(e->x(), width() - e->y(), Trackball::BUTTON_LEFT);       
		// if(e->button() == Qt::RightButton)
		//   trackball.MouseDown(e->x(), width() - e->y(), Trackball::BUTTON_LEFT | Trackball::KEY_CTRL);       
		updateGL();
	}

	void mouseReleaseEvent(QMouseEvent *e) {
		m_tb.callback_mouseUp(e->x(), height() - e->y(), e->button(),e->modifiers());
		// if(e->button() == Qt::LeftButton)
		//trackball.MouseUp(e->x(), width() - e->y(), Trackball::BUTTON_LEFT);  
		//if(e->button() == Qt::RightButton)
		//  trackball.MouseUp(e->x(), width() - e->y(), Trackball::BUTTON_LEFT | Trackball::KEY_CTRL);                 
	}


	static float minmax1(float v)
	{
		return std::min(1.0f, std::max(-1.0f, v));
	}


	void mouseMoveEvent(QMouseEvent *e) {
		m_tb.MouseMove(e->x(), height() - e->y());
		updateGL();
	}

	void wheelEvent(QWheelEvent *event)
	{

		QPoint numDegrees = event->angleDelta() / 8;
		
		auto n = m_eye- m_center;
		n *= numDegrees.y() > 0 ? 9.0/10 : 10.0/9;
		m_eye = m_center + n;

		update();
	}

	void resizeGL(int width, int height)
	{
		glViewport(0, 0, width, height);
	}
	void set_perspective(float p)
	{
		//perspective = p;
		update();
	}

	void view_anim(float v)
	{
		//anim.setStartValue(perspective);
		anim.setEndValue(v);
		anim.start();
	}






private:

	static float privZnear(float s=0)
	{
		float v = 0.01;
		if (s != 0)
			s = v;
		return v;
	}
	static float privFOV(float s = 0)
	{
		float v = 45;
		if (s != 0)
			s = v;
		return v;
	}
	float privTBradius()
	{
		float v = std::min(width(),height())/3;
		return v;
	}



	//void DrawCircle(float cx, float cy, float cz, float r)
	//{
	//	static const float pi = std::acos(0.0)*2;
	//	static const int num_segments = 32;
	//	static const float theta = 2 * pi / float(num_segments);
	//
	//	glLineWidth(10);
	//	glColor3f(0.0f, 1.0f, 0.0f);
	//	glBegin(GL_LINE_LOOP);
	//	for (int ii = 0; ii < num_segments; ii++)
	//	{

	//		glVertex3f( cx + r*std::cos(theta*ii), cy+r*std::sin(theta*ii),cz);//output vertex 

	//	}
	//	glEnd();
	//	
	//}



	QVector3D privMousePos2ray(const QPoint& mp) const
	{


		QVector4D pB(mp.x() * 2.0 / float(width()) - 1.0f, -(mp.y() * 2.0 / float(height()) - 1.0f), 1, 1);
		pB = privTmat().inverted()*privVmat().inverted()*pB;
		pB /= pB.w();


		QVector3D res = (pB - m_eye).toVector3D();
		res.normalize();
		return res;
	}

	template < class ValueType>
	static	inline ValueType QTLogicalToDevice(QWidget *qw, const ValueType &value)
	{
		return value*qw->devicePixelRatio();
	}

	//transformation matrix
	QMatrix4x4 privTmat() const
	{
		QMatrix4x4 m;
		m.lookAt(m_eye, m_center, m_upvec);
		return m;
	}
	//view Matrix
	QMatrix4x4 privVmat() const
	{
		QMatrix4x4 m;

		float ratio = width() / (float)height();
		//m.ortho(-ratio, ratio, -1.f, 1.f, 0.f, 10000.f);
		//m.frustum(-ratio*S, ratio*S, -S, S, S, 10000.f);
		m.perspective(privFOV(), ratio, privZnear(), 10000.f);
		


		return m;
	}

	QGLShaderProgram m_meshShader;
	//QGLShaderProgram m_quadShader;

	const Mesh* m_meshDataP;
	GLMesh m_mesh;
	Backdrop m_backdrop;

	QVector3D m_eye;
	QVector3D m_center;
	QVector3D m_upvec;


	//float m_zoom;
	//float m_scale;
	//float m_tilt;
	//float m_yaw;
	//
	//float perspective;



	//	vcg::Trackball m_trackball;
	//	vcg::Trackball m_trackball_light;


	Q_PROPERTY(float perspective WRITE set_perspective);
	QPropertyAnimation anim;

	QPoint m_mousePos;
	QVector3D m_mouseRay;
	QPoint m_mousePressPos;
	QVector3D m_mousePressEye;
	QVector3D m_mousePressUpvec;
	QString m_status;
	Trackball m_tb;





















};

#endif // CANVAS_H
