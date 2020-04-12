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
#include "zview/gui/drawables/backdrop.h"
#include "zview/gui/drawables/drawables_buffer.h"

// #include "libcurie/common/common_types.h"
// #include "libcurie/drawables/backdrop.h"
// #include "libcurie/drawables/drawable_base.h"
// #include "libcurie/drawables/drawable_mesh.h"
// #include "vp_mat.h"

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit Canvas(QWidget* parent);
	void addShape(const Types::Shape& obj,const std::string& name);
	void resetView();

public slots:
    void forceUpdate();
    void setTexture(int txt);
protected:
	void initializeGL();

	
	void paintGL();
	void resizeGL(int w, int h);
	




    
	
private:

	
	DrawablesBuffer m_buffer;
	Backdrop m_backdrop;
    int m_textureType;
	VPmat m_vpmat;
	
	


};



