#include "zview/gui/canvas.h"

// #include "drawables/drawables_buffer.h"
// #include "drawables/drawable_basic_shapes.h"


Canvas::Canvas(QWidget* parent = 0) :QOpenGLWidget(parent), m_textureType(0)
{
	setMouseTracking(true);
}

void Canvas::setTexture(int txt) { m_textureType = txt; }

void Canvas::forceUpdate()
{
    update();
}
void Canvas::resetView()
{
	// camera is always at (0,0,0), looking tawards negative z.
	// rotation center is always (0,0,-1). for init, set object to (0,0,-1), and rescale it to fit in image.
	// static const float deg2rad = std::acosf(0.0) / 90;
	// Types::Roi3d objsbbox = drawablesBuffer.get3dbbox();
	
	// float a = std::tan(deg2rad * Params::camFOV() / 2);
	// float s = 2 * a / (objsbbox.rangey() + objsbbox.rangez() * a);
    
    // QVector3D t = -objsbbox.mid();
	// QMatrix4x4 vm = VPmat::translate({0,0,-1})*VPmat::scale(s) * VPmat::translate(t);
	
	// vpmat.setViewMatrix(vm);
	
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
	// QMatrix4x4 vp = vpmat();
	// for (auto& d : drawablesBuffer)
	// {
	// 	d.get()->paintGL(vp, m_textureType);
	// }
	// drawableBasicShapes.paintGL(vp);


	
	
}
void Canvas::resizeGL(int w, int h)
{
	//m_bg->resizeGL(w, h);
	// vpmat.setWinSize(w, h);
	

}

