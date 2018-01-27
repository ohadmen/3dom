#pragma once

#include<QDebug>
#include<QVector3D>

class QPrimitive3D
{
public:
    const enum Type{Vector3D, Ray3D, Line3D, Plane3D, Sphere3D};
    QPrimitive3D() {};
    QPrimitive3D(Type type) : _type(type) {};
public:
    Type type() const {return _type;}
protected:
    Type _type;
};

