#pragma once
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <string>
#include <fstream>

#include "zview/common/common_types.h"
class DrawableBase: public QOpenGLFunctions
{
public:
	DrawableBase(const std::string& name);
	virtual void initializeGL() = 0;
	virtual void paintGL(const QMatrix4x4& mvp, int txt) = 0;
	virtual QVector3D picking(const QVector3D& p, const QVector3D& n) const = 0;

	bool isActive() const;
	void setActive(bool a);
	const std::string& getName() const;

protected:
    std::string m_name;
    bool m_active;
	QOpenGLShaderProgram m_meshShader;

	void privInitShader(const QString &shaderName);

};


