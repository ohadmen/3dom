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
			m_eye = m_center + QVector3D(0, -m_meshDataP->zmax()*2, 0);
			m_upvec = QVector3D(0, 0, 1);
			//m_scale = 1;

			// Reset other camera parameters


		}

		update();


	}


protected:

	void mouseDoubleClickEvent(QMouseEvent* event)
	{
		QVector3D r = privMousePos2ray(m_mousePos);

		std::array<QVector3D, 2> p = m_meshDataP->closest2ray(m_eye, r);
		m_center = p[0];
		update();


	}
	void keyPressEvent(QKeyEvent * event)
	{
		static const float rad2deg = 90.0 / std::acos(0);
		if (event->key() == Qt::Key_C)
		{

			QVector3D r = privMousePos2ray(m_mousePos);

			std::array<QVector3D, 2> p = m_meshDataP->closest2ray(m_eye, r);


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
			m_mousePos = event->pos();
			setCursor(Qt::ClosedHandCursor);
		}
	}

	void mouseReleaseEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton ||
			event->button() == Qt::RightButton)
		{
			unsetCursor();
		}
	}

	void mouseMoveEvent(QMouseEvent* event)
	{
		auto oldPos = event->pos();
		auto d = oldPos - m_mousePos;
		m_mousePos = oldPos;

		QVector3D rayO = privMousePos2ray(oldPos);
		QVector3D rayO = privMousePos2ray(m_mousePos);


		if (event->buttons() & Qt::LeftButton)
		{
			QVector3D n = m_eye - m_center;
			float yaw = fmod( -d.x(), 360);
			float tilt = fmod( -d.y(), 360);
			QMatrix4x4 m;
			m.rotate(tilt, QVector3D::crossProduct(m_upvec,n));
			m.rotate(yaw, m_upvec);
			n = m*n;
			m_eye = m_center + n;

			update();
		}
		else if (event->buttons() & Qt::MiddleButton)
		{
			QVector3D n = m_eye - m_center;

			QMatrix4x4 m;
			m.translate(-d.x()*QVector3D::crossProduct(m_upvec, n));
			m.translate(-d.y()*m_upvec);
			m_center = m*m_center;
			m_eye = m*m_eye;
			update();
		}



		
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

		//m.ortho(-ratio, ratio, -1.f, 1.f, 0.f, 10.f);
		
		//m.frustum(-ratio*S, ratio*S, -S, S, S, 100.f);
		m.perspective(45, ratio, privZnear(), 10.f);
		


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
	QString m_status;
	std::vector<Circle3d> m_circles;
	std::vector<Line3d> m_lines;






















};

#endif // CANVAS_H
