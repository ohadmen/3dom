#pragma once

#include <QtGui/QMouseEvent>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector2D>
#include <QtCore/QBasicTimer>
#include <QtGui/qevent.h>
#include "zview/backend/vp_mat.h"
#include "zview/backend/state_machine/state_machine.h"
#include "zview/gui/drawables/backdrop.h"
#include "zview/gui/drawables/drawables_buffer.h"

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit Canvas(QWidget *parent);
	void addShape(const Types::Shape &obj);
	void resetView();
	
	void input(QInputEvent *e);
	
public slots:
	void slot_setStatus(const QString& str);
	void slot_forceUpdate();
	void slot_setTexture(int txt);

signals:
	void signal_setStatus(const QString& str);
	
protected:
	void initializeGL();

	void paintGL();
	void resizeGL(int w, int h);

    void mouseReleaseEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);



private:
	int m_textureType;
	TrackStateMachine m_stateMachine;
	Backdrop m_backdrop;
	
};
