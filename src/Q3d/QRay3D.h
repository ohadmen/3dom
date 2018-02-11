#pragma once

#include "QPrimitive3D.h"

class QRay3D : public QPrimitive3D
{
public:
    QRay3D();
    QRay3D(const QVector3D &origin, const QVector3D &direction = QVector3D(0.0f, 0.0f, 1.0f));
    virtual ~QRay3D();
public:
    QVector3D origin   ()        const;
    QVector3D direction()        const;
    QVector3D point    (float t) const;
    void setOrigin     (const QVector3D &value);
    void setDirection  (const QVector3D &value);
    bool operator ==   (const QRay3D &value);
    bool operator !=   (const QRay3D &value);
protected:
    QVector3D _origin;
    QVector3D _direction;
};

inline QRay3D::QRay3D() 
    : QPrimitive3D(QPrimitive3D::Ray3D), _direction(0.0f, 0.0f, 1.0f)
{
}

inline QRay3D::QRay3D(const QVector3D &origin, const QVector3D &direction)
    : QPrimitive3D(QPrimitive3D::Ray3D), _origin(origin), _direction(direction)
{
}

inline QRay3D::~QRay3D()
{
}

inline QVector3D QRay3D::origin() const 
{
    return _origin;
}

inline QVector3D QRay3D::direction() const 
{
    return _direction;
}

inline QVector3D QRay3D::point(float t) const 
{ 
    return _origin + _direction * t;
}

inline void QRay3D::setOrigin(const QVector3D &value)
{
    _origin = value;
}

inline void QRay3D::setDirection(const QVector3D &value)
{
    _direction = value.normalized();//TODO: normalize or not?
}

inline bool QRay3D::operator == (const QRay3D &value)
{
    return qFuzzyCompare(_origin, value._origin) && qFuzzyCompare(_direction, value._direction);
}

inline bool QRay3D::operator != (const QRay3D &value)
{
    return !qFuzzyCompare(_origin, value._origin) || !qFuzzyCompare(_direction, value._direction);
}

inline bool qFuzzyCompare(const QRay3D &value1, const QRay3D &value2)
{
    return qFuzzyCompare(value1.origin(), value2.origin()) && qFuzzyCompare(value1.direction(), value2.direction());
}

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator << (QDebug dbg, const QRay3D &ray)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QRay3D(origin("
        << ray.origin().x() << ", " << ray.origin().y() << ", "
        << ray.origin().z() << ") - direction("
        << ray.direction().x() << ", " << ray.direction().y() << ", "
        << ray.direction().z() << "))";
    return dbg;
}
#endif
