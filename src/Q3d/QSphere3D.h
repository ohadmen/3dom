#pragma once

#include "QPrimitive3D.h"
#include "QLine3D.h"
class QSphere3D : public QPrimitive3D
{
public:
    QSphere3D ();
    QSphere3D (const QVector3D &center, float radius);
    virtual ~QSphere3D();
public:
    QVector3D center   () const;
    float     radius   () const;
    void setCenter     (const QVector3D &value);
    void setRadius     (float value);
    bool operator ==   (const QSphere3D &value);
    bool operator !=   (const QSphere3D &value);
	bool intersection(const QLine3D& li, QVector3D* p0P, QVector3D* p1P)
	{
		QVector3D neworig = li.p1() - center();
		QVector3D n = li.direction();
		// poi si risolve il sistema di secondo grado (con maple...)
		float t1 = n.x()*n.x();
		float t2 = n.y()*n.y();
		float t3 = n.z()*n.z();
		float t6 = neworig.y()*n.y();
		float t7 = neworig.x()*n.x();
		float t8 = neworig.z()*n.z();
		float t15 = radius()*radius();
		float t17 = neworig.z()*neworig.z();
		float t19 = neworig.y()*neworig.y();
		float t21 = neworig.x()*neworig.x();
		float t28 = (2.0*t7*t6 + 2.0*t6*t8 + 2.0*t7*t8 + t1*t15 - t1*t17 - t1*t19 - t2*t21 + t2*t15 - t2*t17 - t3*t21 + t3*t15 - t3*t19);
		if (t28<0) return false;
		float t29 = sqrt(t28);
		float val0 = 1 / (t1 + t2 + t3)*(-t6 - t7 - t8 + t29);
		float val1 = 1 / (t1 + t2 + t3)*(-t6 - t7 - t8 - t29);

		*p0P = li.point(val0);
		*p1P = li.point(val1);
		return true;
	}
protected:
    QVector3D _center;
    float     _radius;
private:
};

inline QSphere3D::QSphere3D()
    : QPrimitive3D(QPrimitive3D::Sphere3D), _radius(1.0f)
{
}

inline QSphere3D::QSphere3D(const QVector3D &center, float radius)
    : QPrimitive3D(QPrimitive3D::Sphere3D), _center(center), _radius(radius)
{
}

inline QSphere3D::~QSphere3D()
{
}

inline QVector3D QSphere3D::center() const 
{
    return _center;
}

inline float QSphere3D::radius() const 
{
    return _radius;
}

inline void QSphere3D::setCenter(const QVector3D &value)
{
    _center = value;
}

inline void QSphere3D::setRadius(float value)
{
    _radius = value;
}

inline bool QSphere3D::operator == (const QSphere3D &value)
{
    return qFuzzyCompare(_center, value._center) && qFuzzyCompare(_radius, value._radius);
}

inline bool QSphere3D::operator != (const QSphere3D &value)
{
    return !qFuzzyCompare(_center, value._center) || !qFuzzyCompare(_radius, value._radius);
}

inline bool qFuzzyCompare(const QSphere3D &value1, const QSphere3D &value2)
{
    return qFuzzyCompare(value1.center(), value2.center()) && qFuzzyCompare(value1.radius(), value2.radius());
}

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator << (QDebug dbg, const QSphere3D &value)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QSphere3D(center("
        << value.center().x() << ", " << value.center().y() << ", "
        << value.center().z() << ") - radius = " << value.radius();
    return dbg;
}
#endif