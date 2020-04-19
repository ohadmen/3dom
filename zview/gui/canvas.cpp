#include "zview/gui/canvas.h"
#include "zview/common/params.h"
#include <math.h>
// #include "drawables/drawables_buffer.h"
// #include "drawables/drawable_basic_shapes.h"


Canvas::Canvas(QWidget* parent = 0) :QOpenGLWidget(parent), m_textureType(0),m_stateMachine()
{
	
	
	setMouseTracking(true);
    //QObject::connect(m_stateMachine, &TrackStateMachine::setStatus, this, &CurieMainWin::setStatus);
	QObject::connect(&m_stateMachine, &TrackStateMachine::signal_setStatus, this, &Canvas::slot_setStatus);
    QObject::connect(&m_stateMachine, &TrackStateMachine::signal_canvasUpdate, this, &Canvas::slot_forceUpdate);
    QObject::connect(&m_stateMachine, &TrackStateMachine::signal_setTexture, this, &Canvas::slot_setTexture);
 
}

void Canvas::slot_setStatus(const QString& str)
{
	emit signal_setStatus(str);
}



void Canvas::slot_setTexture(int txt) { m_textureType = txt; qDebug() << "texture set to " << m_textureType;}

void Canvas::slot_forceUpdate(){    update();}
void Canvas::resetView()
{
	constexpr float deg2rad =  M_PI/180.0;
	// camera is always at (0,0,0), looking tawards negative z.
	// rotation center is always (0,0,-1). for init, set object to (0,0,-1), and rescale it to fit in image.
	// static const float deg2rad = std::acosf(0.0) / 90;
	Types::Roi3d objsbbox;
	for (const auto& o : drawablesBuffer)
	{
		Types::Roi3d bbox = o.second.get()->get3dbbox();
		objsbbox |= bbox;
	}
	
	float a = std::tan(deg2rad * Params::camFOV() / 2);
	float s = 2 * a / (objsbbox.rangey() + objsbbox.rangez() * a);
    
    QVector3D t = -objsbbox.mid();
	QMatrix4x4 vm = VPmat::translate({0,0,-1})*VPmat::scale(s) * VPmat::translate(t);
	
	m_stateMachine.setViewMatrix(vm);
	
}

void  Canvas::initializeGL()
{
	initializeOpenGLFunctions();
    
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//setFocusPolicy(Qt::StrongFocus);
	//glEnable(GL_BLEND);
	//glEnable(GL_LINE_SMOOTH);
	//glClearColor(0, 0, 0, 1);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_backdrop.initializeGL();

	// drawableBasicShapes.initializeGL();
	
	
	
	
}

void Canvas::paintGL()
{
	glEnable(GL_DEPTH_TEST);
	m_backdrop.paintGL(QMatrix4x4(),0);
	QMatrix4x4 vp = m_stateMachine.getVPmatrix();
	for (auto& d : drawablesBuffer)
	{
		d.second.get()->paintGL(vp, m_textureType);
	}
	//drawableBasicShapes.paintGL(vp);
	


	
	
}
void Canvas::resizeGL(int w, int h)
{
	m_backdrop.resizeGL(w, h);
	m_stateMachine.setWinSize(w, h);
	

}

void Canvas::addShape(const Types::Shape& obj,const std::string& name)
{
	DrawablesBuffer::i().addShape(obj,name);
	resetView();
}


void Canvas::input(QInputEvent *e)
{
	m_stateMachine.input(e);
}


void Canvas::mouseReleaseEvent(QMouseEvent* e){m_stateMachine.input(e);}
void Canvas::mousePressEvent(QMouseEvent* e){m_stateMachine.input(e);}
void Canvas::mouseDoubleClickEvent(QMouseEvent* e){m_stateMachine.input(e);}
void Canvas::wheelEvent(QWheelEvent* e){m_stateMachine.input(e);}
void Canvas::mouseMoveEvent(QMouseEvent* e){    m_stateMachine.input(e);repaint();}

