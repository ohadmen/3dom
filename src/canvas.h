
#ifndef CANVAS_H
#define CANVAS_H


#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>

#include "loader.h"

class GeometryEngine;

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit Canvas(QWidget *parent=0) :
		QOpenGLWidget(parent),
		

		angularSpeed(0)
	{

	}
	~Canvas()
	{

		// and the buffers.
		makeCurrent();

		
		doneCurrent();
	}
	void setToken(int token) { m_currentMeshToken = token; }
	void loadMesh(int token)
	{
		MeshArray::i().getMesh(token)->initGL();
	}
	
protected:
	void mousePressEvent(QMouseEvent *e)
	{
		// Save mouse press position
		mousePressPosition = QVector2D(e->localPos());
	}
	void mouseReleaseEvent(QMouseEvent *e)
	{
		// Mouse release position - mouse press position
		QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

		// Rotation axis is perpendicular to the mouse position difference
		// vector
		QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

		// Accelerate angular speed relative to the length of the mouse sweep
		qreal acc = diff.length() / 100.0;

		// Calculate new rotation axis as weighted sum
		rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

		// Increase angular speed
		angularSpeed += acc;
	}
	void timerEvent(QTimerEvent *e) {
		// Decrease angular speed (friction)
		angularSpeed *= 0.99;

		// Stop rotation when speed goes below threshold
		if (angularSpeed < 0.01) {
			angularSpeed = 0.0;
		}
		else {
			// Update rotation
			rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

			// Request an update
			update();
		}
	}

	void initializeGL()
	{
		initializeOpenGLFunctions();

		glClearColor(0, 0, 0, 1);

		initShaders();


		//! [2]
		// Enable depth buffer
		glEnable(GL_DEPTH_TEST);

		// Enable back face culling
		glEnable(GL_CULL_FACE);
		//! [2]

		

		auto t = MeshArray::i().getTokenList();
		for (auto zz : t)
			MeshArray::i().getMesh(zz)->initGL();
		// Use QBasicTimer because its faster than QTimer
		timer.start(12, this);
	}

	void resizeGL(int w, int h)
	{
		// Calculate aspect ratio
		qreal aspect = qreal(w) / qreal(h ? h : 1);

		// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
		const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;	

		// Reset projection
		projection.setToIdentity();

		// Set perspective projection
		projection.perspective(fov, aspect, zNear, zFar);
	}
	void paintGL()
	{
		// Clear color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		//! [6]
		// Calculate model view transformation
		QMatrix4x4 matrix;
		matrix.translate(0.0, 0.0, -5.0);
		matrix.rotate(rotation);

		// Set modelview-projection matrix
		program.setUniformValue("mvp_matrix", projection * matrix);
		//! [6]

			// Draw cube geometry
		
		Mesh* p = MeshArray::i().getMesh(m_currentMeshToken);
		if (p == nullptr)
			return;
		p->draw(&program);
	}

	void initShaders()
	{
		// Compile vertex shader
		if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
			close();

		// Compile fragment shader
		if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
			close();

		// Link shader pipeline
		if (!program.link())
			close();

		// Bind shader pipeline for use
		if (!program.bind())
			close();
	}

private:
	QBasicTimer timer;
	QOpenGLShaderProgram program;
	


	QMatrix4x4 projection;

	QVector2D mousePressPosition;
	QVector3D rotationAxis;
	qreal angularSpeed;
	QQuaternion rotation;
	int m_currentMeshToken;
};

#endif // Canvas_H
