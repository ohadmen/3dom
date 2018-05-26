#pragma once
#include <Qt>
#include<QDebug>
#include<QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
class QPrimitive3D : protected QOpenGLFunctions
{
  

public:
    const enum Type{Vector3D, Ray3D, Line3D, Plane3D, Sphere3D,Text3D};
    QPrimitive3D():m_color(1, 1, 1, 1) {};
    QPrimitive3D(Type type) : _type(type), m_color(1, 1, 1, 1) {};
    QVector4D getColor()const { return m_color; }
    void bind()
    {
        m_buff.bind();
    }
    void release()
    {
        m_buff.release();
    }
    virtual void init() {};
    virtual void draw() {};
public:
    Type type() const {return _type;}
protected:
    Type _type;
    
    QOpenGLBuffer m_buff;
    QVector4D m_color;
};

