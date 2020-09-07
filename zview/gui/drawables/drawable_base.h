#pragma once
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <string>
#include <fstream>
#include "zview/common/params.h"
#include "zview/common/common_types.h"
class DrawableBase: public QOpenGLFunctions
{
public:
	DrawableBase();
	virtual void initializeGL() = 0;
	virtual void paintGL(const QMatrix4x4& mvp) = 0;
	virtual Types::VertData picking(const QVector3D& p, const QVector3D& n) const = 0;
	virtual Types::Roi3d get3dbbox()const=0;
	virtual Types::Shape getShape() const =0;
	virtual ~DrawableBase();

	bool isActive() const;
	void setActive(bool a);
	virtual const std::string& getName() const=0;


	virtual bool updateVertexBuffer(const Types::VertData* data,size_t n);

protected:
	
    
    bool m_active;
	QOpenGLShaderProgram m_meshShader;
	QOpenGLBuffer m_vBuff; //vertices 3xn
	
	void privInitShader(const QString &shaderName);

};


