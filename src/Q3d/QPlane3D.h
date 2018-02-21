#pragma once

#include "QBitFlags.h"
#include "QPrimitive3D.h"
#include "QLine3D.h"

class QPlane3D : public QPrimitive3D
{
public:
    QPlane3D();
    QPlane3D(const QVector3D &normal, float distance);
    QPlane3D(const QVector3D &normal, const QVector3D &point);
    QPlane3D(const QVector3D &p0, const QVector3D &p1, const QVector3D &p2);
    virtual ~QPlane3D();
public:
    QVector3D normal      () const;
    float     distance    () const;
    QVector3D direction   () const;
    QVector3D origin      () const;

    void      setNormal   (const QVector3D &value);
    void      setDistance (float value);
    void      redefine    (const QVector3D &normal, float distance)
	{
		_normal = normal.normalized();
		_distance = distance;
		_dirty++;
		update();
	}


    void      redefine    (const QVector3D &normal, const QVector3D &point)
	{
		redefine(normal, QVector3D::dotProduct(normal, point));
	}
    void      redefine    (const QVector3D &p0, const QVector3D &p1, const QVector3D &p2)
	{
		redefine(QVector3D::crossProduct(p1 - p0, p2 - p0).normalized(), p0);
	}
    bool      operator == (const QPlane3D &plane);
    bool      operator != (const QPlane3D &plane);
	bool	  intersection(const QLine3D& li,QVector3D* pP) const
	{
		const float epsilon = (1e-8f);

		float k = QVector3D::dotProduct(normal(),li.direction());						// Compute 'k' factor
		if (std::abs(k) < epsilon)
			return false;
		float r = (distance()-QVector3D::dotProduct(normal(),li.p1())) / k;	// Compute ray distance
		*pP = li.p1() + li.direction()*r;
		return true;
	}
protected:
    virtual void update(){}
protected:
    const enum dirtyFlag{dirtyDirection, dirtyOrigin};
    QVector3D                 _normal;
    float                     _distance;
    mutable QBitFlags<quint8> _dirty;
    mutable QVector3D         _direction;
    mutable QVector3D         _origin;
};

inline QPlane3D::QPlane3D() 
    : QPrimitive3D(QPrimitive3D::Plane3D), _distance(0)
{
}

inline QPlane3D::QPlane3D(const QVector3D &normal, float distance) 
    : QPrimitive3D(QPrimitive3D::Plane3D), _normal(normal), _distance(distance)
{
}

inline QPlane3D::QPlane3D(const QVector3D &normal, const QVector3D &point)
{
    redefine(normal, point);
}

inline QPlane3D::QPlane3D(const QVector3D &p0, const QVector3D &p1, const QVector3D &p2)
{
    redefine(p0, p1, p2);
}

inline QPlane3D::~QPlane3D(){}

inline QVector3D QPlane3D::normal() const
{
    return _normal;
}

inline float QPlane3D::distance() const 
{
    return _distance;
}

inline QVector3D QPlane3D::direction() const
{
    if(_dirty(dirtyDirection))
    {
        _direction = QVector3D::crossProduct(_normal, QVector3D(0.0f, 1.0f, 0.0f)).normalized();
        _dirty -= dirtyDirection;
    }
    return _direction;
}

inline QVector3D QPlane3D::origin() const
{
    if(_dirty(dirtyOrigin))
    {
        _origin = _normal * _distance;
        _dirty -= dirtyOrigin;
    }
    return _origin;
}

inline void QPlane3D::setNormal(const QVector3D &value)
{
    if(!qFuzzyCompare(_normal, value))
    {
        _normal = value.normalized();
        _dirty++;
        update();
    }
}

inline void QPlane3D::setDistance(float value)
{
    if(!qFuzzyCompare(_distance, value))
    {
        _distance = value;
        _dirty++;
        update();
    }
}




inline bool QPlane3D::operator == (const QPlane3D &plane)
{
    return qFuzzyCompare(_normal, plane._normal) && qFuzzyCompare(_distance, plane._distance);
}

inline bool QPlane3D::operator != (const QPlane3D &plane)
{
    return !qFuzzyCompare(_normal, plane._normal) || !qFuzzyCompare(_distance, plane._distance);
}

inline bool qFuzzyCompare(const QPlane3D &plane1, const QPlane3D &plane2)
{
    return qFuzzyCompare(plane1.normal(), plane2.normal()) && qFuzzyCompare(plane1.distance(), plane2.distance());
}

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator << (QDebug dbg, const QPlane3D &plane)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QPlane3D(normal(" << plane.normal().x() << ", " << plane.normal().y() << ", " << plane.normal().z() << "), distance = " << plane.distance() << ")";
    return dbg;
}
#endif 