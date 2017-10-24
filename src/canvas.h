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
		m_scale(1), m_zoom(1), m_tilt(90), m_yaw(0),
		perspective(0.25), anim(this, "perspective"), m_status(" ")
	{
		QFile styleFile("style.qss");
		styleFile.open(QFile::ReadOnly);
		setStyleSheet(styleFile.readAll());
		setFocusPolicy(Qt::StrongFocus);//catching keyboard events
		setMouseTracking(true);
		anim.setDuration(100);

		m_circles.resize(3);
		m_lines.resize(3);
	}

	~Canvas()
	{
		

	}




	void initializeGL()
	{

		initializeGLFunctions();
		m_mesh.init();
		m_backdrop.init();


	}
	void paintEvent(QPaintEvent *event)
	{
		Q_UNUSED(event);

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		m_backdrop.draw();
		draw_mesh();




		

		

	


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
		
		m_meshDataP  = MeshArray::i().getToken(token);
		
		m_mesh.set(*m_meshDataP);

		if (!is_reload)
		{
			QVector3D lower(m_meshDataP->xmin(), m_meshDataP->ymin(), m_meshDataP->zmin());
			QVector3D upper(m_meshDataP->xmax(), m_meshDataP->ymax(), m_meshDataP->zmax());
			m_center = (lower + upper) / 2;
			
			m_scale = 2 / (upper - lower).length();

			// Reset other camera parameters
			m_zoom = 1;
			m_yaw = 0;
			m_tilt = 90;
		}

		update();

		
	}


protected:

//	void mouseDoubleClickEvent(QMouseEvent* event)
//	{
//		
//		QVector3D r = privMousePos2ray(m_mousePos);
//		r.normalize();
//		QVector3D p = m_meshDataP->closest2ray(r);
//
//	}
	void keyPressEvent(QKeyEvent * event)
	{
		static const float rad2deg = 90.0 / std::acos(0);
		if (event->key() == Qt::Key_C)
		{

			std::array<QVector3D,2> r = privMousePos2ray();
			r[1].normalize();
			QVector3D p= m_meshDataP->closest2ray(r[0],r[1]);
			QMatrix4x4 t;

	
			//m_mousePos.x() / (0.5*width()), m_mousePos.y() / (0.5*height())
			t.translate(p);
			//t.rotate(std::acos(r[1][2])*rad2deg, QVector3D(r[1][1], -r[1][0], 0.0f));
			t.scale(0.1f);
			m_circles[0].set(t);
			//m_lines[0].set(p0, p1);

			set_status(QString::number(p.x()) + "," + QString::number(p.y()) + "," + QString::number(p.z()));
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
		auto p = event->pos();
		auto d = p - m_mousePos;


		if (event->buttons() & Qt::LeftButton)
		{
			m_yaw = fmod(m_yaw - d.x(), 360);
			m_tilt = fmod(m_tilt - d.y(), 360);
			update();
		}
		else if (event->buttons() & Qt::MiddleButton)
		{
			m_center = transform_matrix().inverted() *
				view_matrix().inverted() *
				QVector3D(-d.x() / (0.5*width()),
					d.y() / (0.5*height()), 0);
			update();
		}



		m_mousePos = p;
	}

	void wheelEvent(QWheelEvent *event)
	{
		//QVector3D d = transform_matrix().inverted() * view_matrix().inverted()*QVector3D(0, 0, 1);//cam center
		//
		//d.normalize();
		//if (event->delta() < 0)
		//{

		//	m_center += (m_camRotCenter + d)*.1;
		//	//	d.norm
		//}
		//else
		//{
		//	m_center -= d;
		//}


		auto p = event->pos();
		QVector3D v(1 - p.x() / (0.5*width()),
			p.y() / (0.5*height()) - 1, 0);
		QVector3D a = transform_matrix().inverted() *
			view_matrix().inverted() * v;
		
		if (event->delta() < 0)
		{
			for (int i = 0; i > event->delta(); --i)
				m_zoom *= 1.001f;
		}
		else if (event->delta() > 0)
		{
			for (int i = 0; i < event->delta(); ++i)
				m_zoom /= 1.001f;
		}
		
		// Then find the cursor's GL position post-zoom and adjust m_center.
		QVector3D b = transform_matrix().inverted() *
			view_matrix().inverted() * v;
		m_center += b - a;
		update();
	}

	void resizeGL(int width, int height)
	{
		glViewport(0, 0, width, height);
	}
	void set_perspective(float p)
	{
		perspective = p;
		update();
	}

	void view_anim(float v)
	{
		anim.setStartValue(perspective);
		anim.setEndValue(v);
		anim.start();
	}





private:



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


	std::array<QVector3D,2> privMousePos2ray() const
	{
		//QVector3D p0 = transform_matrix().inverted() * view_matrix().inverted()*QVector3D(0, 0, -1);//cam center
		//QVector3D p1 = transform_matrix().inverted() * view_matrix().inverted()*QVector3D(m_mousePos.x() * 2 / float(width()) - 1.0f, -(m_mousePos.y() * 2 / float(height()) - 1.0f), 0);//look point

		std::array<QVector3D, 2> res =
		{
			transform_matrix().inverted() * view_matrix().inverted()*QVector3D(0, 0, -1),
			transform_matrix().inverted() * view_matrix().inverted()*QVector3D(m_mousePos.x() * 2 / float(width()) - 1.0f, -(m_mousePos.y() * 2 / float(height()) - 1.0f), 1)
		};

		return res;
	}

	 template < class ValueType>
	 static	inline ValueType QTLogicalToDevice(QWidget *qw, const ValueType &value)
	{
		return value*qw->devicePixelRatio();
	}

	void draw_mesh()
	{
		//m_meshShader.bind();
	
		// Load the transform and view matrices into the shader


		// Compensate for z-flattening when zooming
		//glUniform1f(m_meshShader.uniformLocation("zoom"), 1 / m_zoom);

		// Find and enable the attribute location for vertex position
		
		// Then draw the m_mesh with that vertex position
		m_mesh.draw(transform_matrix(),view_matrix());


		

		//m_circles[0].draw();
		//m_lines[0].draw();

		


		

	}


	QMatrix4x4 transform_matrix() const
	{
		QMatrix4x4 m;
		m.rotate(m_tilt, QVector3D(1, 0, 0));
		m.rotate(m_yaw, QVector3D(0, 0, 1));
		m.scale(-m_scale, m_scale, -m_scale);
		m.translate(-m_center);
		return m;
	}

	QMatrix4x4 view_matrix() const
	{
		QMatrix4x4 m;
		if (width() > height())
		{
			m.scale(-height() / float(width()), 1, 0.5);
		}
		else
		{
			m.scale(-1, width() / float(height()), 0.5);
		}
		m.scale(m_zoom, m_zoom, 1);
		m(3, 2) = perspective;
		return m;
	}

	QGLShaderProgram m_meshShader;
	//QGLShaderProgram m_quadShader;

	const Mesh* m_meshDataP;
	GLMesh m_mesh;
	Backdrop m_backdrop;

	QVector3D m_center;

	float m_scale;
	float m_zoom;
	float m_tilt;
	float m_yaw;

	float perspective;



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
