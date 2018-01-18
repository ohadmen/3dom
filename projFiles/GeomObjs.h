#pragma once
#include <QtOpenGL/QGLFunctions>
#include <QVector3D>
class Plane
{
	QVector3D m_o;
	QVector3D m_n;
public:
	Plane(const QVector3D& o, const QVector3D& n) :m_o(o), m_n(n) {}
};
class Ray
{
	QVector3D m_o;
	QVector3D m_d;
public:
	Ray(const QVector3D& o, const QVector3D& d) :m_o(o), m_d(d) {}

};
