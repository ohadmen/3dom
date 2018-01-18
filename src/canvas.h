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
#include "circle3d.h"
#include "Line3d.h"


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

		m_circles.resize(2);
		m_lines.resize(1);
	}

	~Canvas()
	{


	}




	void initializeGL()
	{

		m_mesh.glinit();
		//m_backdrop.glinit();

		QMatrix4x4 s1; s1.scale(1);
		QMatrix4x4 s2; s1.scale(.5);

		m_circles[0].setColor(1, 0, 0);
		m_circles[0].setLineWidth(1);
		m_circles[0].set(s1);

		m_circles[1].setColor(0, 1, 0);
		m_circles[1].setLineWidth(1);
		m_circles[1].set(s2);

		m_lines[0].setColor(0, 1, 0, 1);
		m_lines[0].setLineWidth(1);

		privSetTrackBallCircles();


	}
	void paintEvent(QPaintEvent *event)
	{
		Q_UNUSED(event);

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		//m_backdrop.draw();


		m_mesh.draw(privTmat(), privVmat());
		for (int i = 0; i != m_circles.size(); ++i)
			m_circles[i].draw(privTmat(), privVmat());
		for (int i = 0; i != m_lines.size(); ++i)
			m_lines[i].draw(privTmat(), privVmat());


		m_trackBall[0].draw(privTmat(), privVmat());
		m_trackBall[1].draw(privTmat(), privVmat());
		m_trackBall[2].draw(privTmat(), privVmat());




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

	void privSetTrackBallCircles()
	{
		
		m_trackBall[0].setColor(.92, .14, .14);
		m_trackBall[1].setColor(.14, .92, .14);
		m_trackBall[2].setColor(.14, .14, .92);
		m_trackBall[0].setLineWidth(1);
		m_trackBall[1].setLineWidth(1);
		m_trackBall[2].setLineWidth(1);

		QVector3D cntrlRay = privMousePos2ray(QPoint(width() / 2, height() / 2));
		QVector3D outrlRay = privMousePos2ray(QPoint(width() / 2+privTBradius(), height() / 2));
		float r = (outrlRay - cntrlRay).length()*(m_center - m_eye).length();
		QMatrix4x4 t;
		
		t.translate(m_center);
		t.scale(r);
		
		m_trackBall[0].set(t);
		t.rotate(90, 0, 1, 0);
		m_trackBall[1].set(t);
		t.rotate(90, 1, 0, 0);
		m_trackBall[2].set(t);

	}


	void mouseDoubleClickEvent(QMouseEvent* event)
	{
	

		std::array<QVector3D, 2> p = m_meshDataP->closest2ray(m_eye, m_mouseRay);
		m_center = p[0];
		update();


	}
	void keyPressEvent(QKeyEvent * event)
	{
		static const float rad2deg = 90.0 / std::acos(0);
		if (event->key() == Qt::Key_C)
		{

			

			std::array<QVector3D, 2> p = m_meshDataP->closest2ray(m_eye, m_mouseRay);


			QMatrix4x4 t;


			//m_mousePos.x() / (0.5*width()), m_mousePos.y() / (0.5*height())
			t.translate(p[0]);
			t.rotate(-std::acos(p[1].z())*rad2deg, QVector3D(p[1].y(), -p[1].x(), 0.0f));
			t.scale(0.05f);
			m_circles[0].set(t);
			m_lines[0].set(p[0], p[0]+p[1]);
			//m_lines[0].set(r[0], r[0] + r[1]);

			set_status(QString::number(p[0].x()) + "," + QString::number(p[0].y()) + "," + QString::number(p[0].z()));
		}

	}

	void mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton ||
			event->button() == Qt::RightButton)
		{
			
			setCursor(Qt::ClosedHandCursor);
			
		}
		m_mousePressPos = event->pos();
		m_mousePressEye = m_eye;
		m_mousePressUpvec = m_upvec;
		
		
	


	}

	void mouseReleaseEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton ||
			event->button() == Qt::RightButton)
		{
			unsetCursor();
		}
	}

	static float minmax1(float v)
	{
		return std::min(1.0f, std::max(-1.0f, v));
	}
	static QMatrix4x4 vvrotate( QVector3D v0,  QVector3D v1)
	{
		v0.normalize();
		v1.normalize();
		static const float rad2deg = 90.0 / std::acos(0);
		QVector3D ra = -QVector3D::crossProduct(v0, v1);
		float rd = std::acos(QVector3D::dotProduct(v0, v1))*rad2deg;
		QMatrix4x4 m;
		m.rotate(rd, ra);
		return m;
	}
	void mouseMoveEvent(QMouseEvent* event)
	{
		static const float rad2deg = 90.0 / std::acos(0);
		QPoint newPos = event->pos();
		QPoint d = newPos - m_mousePos;


		
		

		QVector3D ray0 = privMousePos2ray(newPos);
		QVector3D ray1 = m_mouseRay;
		QVector3D n = m_mousePressEye - m_center;
		
		
		float tbRadius = privTBradius();
		float tbRadius2 = tbRadius2*tbRadius2;
		QPoint c(width()/2, height()/2);
		if (event->buttons() & Qt::LeftButton)
		{
			
			QVector3D n0(minmax1((m_mousePressPos.x() - width() / 2) / tbRadius), minmax1((m_mousePressPos.y() - height() / 2) / tbRadius), 0);
			QVector3D n1(minmax1((m_mousePos.x()      - width() / 2) / tbRadius), minmax1((m_mousePos.y()      - height() / 2) / tbRadius), 0);
			n0.setZ(-std::sqrt(std::max(0.0f, 1 - n0.x()*n0.x() - n0.y()*n0.y())));
			n1.setZ(-std::sqrt(std::max(0.0f, 1 - n1.x()*n1.x() - n1.y()*n1.y())));
			QMatrix4x4 m2 = vvrotate(n, QVector3D(0, 0, -1));

			n0 = m2*n0;
			n1 = m2*n1;

			QMatrix4x4 m = vvrotate(n0,n1);
			
		
			QVector3D n = m_mousePressEye - m_center;

			m_eye = m*(n)+ m_center;
			m_upvec = m*m_mousePressUpvec;
			
			QVector3D nn = n;
			nn.normalize();
			std::cout << "nn:" << nn.x() << "," << nn.y() << "," << nn.z() ;
			std::cout << "n0:" << n0.x() << "," << n0.y() << "," << n0.z();
			std::cout << std::endl;

			//QPoint od = (c - m_mousePressPos);
			//float d2 = od.x()*od.x() + od.y()*od.y();
			//if (d2 < tbRadius2)
			//{
			//	QMatrix4x4 m;
			//	m.rotate(QVector3D::dotProduct(ray0, ray1), -QVector3D::crossProduct(ray0, ray1));
			//	n = m*n;
			//	m_eye = m_center + n;
			//	m_upvec = m*m_upvec;
			//}
			//else
			//{
			//	static const float rad2deg = 90.0/std::acos(0) ;
			//	QVector2D va(newPos.x() - c.x(), newPos.y() - c.y());
			//	QVector2D vb(m_mousePos.x() - c.x(), m_mousePos.y() - c.y());
			//	va.normalize();
			//	vb.normalize();

			//	//float phi=std::acos(QVector2D::dotProduct(va, vb) );
			//	float phi = (std::atan2f(va.y(), va.x()) - std::atan2f(vb.y(), vb.x()))*rad2deg;
			//	QMatrix4x4 m;
			//	m.rotate(phi, n);
			//	m_upvec = m*m_upvec;
			//	//std::cout << phi<<"   "<< m_upvec.x() << "," << m_upvec.y() << "," << m_upvec.z() << std::endl;
			//}
			////std::cout << "   " << od.x() << "," << od.y() <<  std::endl;
			

			update();
		}
		else if (event->buttons() & Qt::MiddleButton)
		{
			QVector3D n = m_eye - m_center;

			QMatrix4x4 m;
			QVector3D dv = (ray1 - ray0)*n.length();
			m.translate(dv);
	
			m_center = m*m_center;
			privSetTrackBallCircles();
			m_eye = m*m_eye;
			update();
			//std::cout << dv.x()<<","<<dv.y()<<","<<dv.z() << std::endl;
		}

		m_mousePos = newPos;
		m_mouseRay =  privMousePos2ray(m_mousePos);
	}

	void wheelEvent(QWheelEvent *event)
	{

		QPoint numDegrees = event->angleDelta() / 8;
		
		auto n = m_eye- m_center;
		n *= numDegrees.y() > 0 ? 9.0/10 : 10.0/9;
		m_eye = m_center + n;
		privSetTrackBallCircles();
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
	std::vector<Circle3d> m_circles;
	std::vector<Line3d> m_lines;

	Circle3d m_trackBall[3];





















};

#endif // CANVAS_H
