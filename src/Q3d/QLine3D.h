#pragma once

#include "QBitFlags.h"
#include "QPrimitive3D.h"
#include "QRay3D.h"

class QLine3D : public QPrimitive3D
{
public:
    QLine3D ();
    QLine3D (const QVector3D &p1, const QVector3D &p2);
    QLine3D (const QRay3D &ray, float length);
    virtual ~QLine3D();
public:
    QVector3D p1            ()        const;
    QVector3D p2            ()        const;
    QVector3D direction     ()        const;
    float     length        ()        const;
    float     lengthSquared ()        const;
    QVector3D point         (float t) const;
    QRay3D    toRay3D       ()        const;
    void      setP1         (const QVector3D &p1);
    void      setP2         (const QVector3D &p2);
    void      redefine      (const QVector3D &p1, const QVector3D &p2);
    bool      operator ==   (const QLine3D &line);
    bool      operator !=   (const QLine3D &line);
	float projection(const QVector3D& p) const
	{
		return QVector3D::dotProduct(p - _p1, _direction);
	}
	QVector3D closest(const QVector3D& p) const
	{
		return point(projection(p));
	}

    void init()
    {
        m_buff.create();
        m_buff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_buff.bind();

        QVector3D data[2];
        data[0] = p1();
        data[1] = p2();
        m_buff.allocate(data, 2 * sizeof(QVector3D));
        m_buff.release();

    }
    void draw()
    {
        initializeOpenGLFunctions();
        glLineWidth(3);
        glDrawArrays(GL_LINE_LOOP, 0, 2);
    }

protected:
    enum dirtyFlag{dirtyDirection, dirtyLength, dirtyLengthSquared};
    QVector3D                   _p1;
    QVector3D                   _p2;
    mutable QVector3D           _direction;
    mutable float               _length;
    mutable float               _lengthSquared;
    mutable QBitFlags<quint8>   _dirty;
};

inline QLine3D::QLine3D()
    : QPrimitive3D(QPrimitive3D::Line3D)
{
}

inline QLine3D::QLine3D(const QVector3D &p1, const QVector3D &p2)
    : QPrimitive3D(QPrimitive3D::Line3D), _p1(p1), _p2(p2)
{
}

inline QLine3D::QLine3D(const QRay3D &ray, float length)
    : QPrimitive3D(QPrimitive3D::Line3D), _p1(ray.origin()), _p2(ray.point(length))
{
}

inline QLine3D::~QLine3D()
{
}

inline QVector3D QLine3D::p1() const 
{
    return _p1;
}

inline QVector3D QLine3D::p2() const 
{
    return _p2;
}

inline QVector3D QLine3D::direction() const
{
    if(_dirty(dirtyDirection))
    {
        _direction = (_p2 - _p1).normalized();
        _dirty -= dirtyDirection;
    }
    return _direction;
}

inline float QLine3D::length() const
{
    if(_dirty(dirtyLength))
    {
        _length = (_p2 - _p1).length();
        _dirty -= dirtyLength;
    }
    return _length;
}

inline float QLine3D::lengthSquared() const
{
    if(_dirty(dirtyLengthSquared))
    {
        _lengthSquared = (_p2 - _p1).lengthSquared();
        _dirty -= dirtyLengthSquared;
    }
    return _lengthSquared;
}

inline QVector3D QLine3D::point(float t) const 
{ 
    return _p1 + direction() * t;
}

inline QRay3D QLine3D::toRay3D() const
{
    return QRay3D(_p1, direction());
}

inline void QLine3D::setP1(const QVector3D &p1)
{
    if(!qFuzzyCompare(_p1, p1))
    {
        _p1 = p1;
        _dirty++;
    }
}

inline void QLine3D::setP2(const QVector3D &p2)
{
    if(!qFuzzyCompare(_p2, p2))
    {
        _p2 = p2;
        _dirty++;
    }
}

inline void QLine3D::redefine(const QVector3D &p1, const QVector3D &p2)
{
    setP1(p1);
    setP2(p2);
}

inline bool QLine3D::operator == (const QLine3D &line)
{
    return qFuzzyCompare(_p1, line._p1) && qFuzzyCompare(_p2, line._p2);
}

inline bool QLine3D::operator != (const QLine3D &line)
{
    return !qFuzzyCompare(_p1, line._p1) || !qFuzzyCompare(_p2, line._p2);
}

inline bool qFuzzyCompare(const QLine3D &line1, const QLine3D &line2)
{
    return qFuzzyCompare(line1.p1(), line2.p1()) && qFuzzyCompare(line1.p2(), line2.p2());
}

#ifndef QT_NO_DEBUG_STREAM

inline QDebug operator << (QDebug dbg, const QLine3D &line)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "QLine3D((" << line.p1().x() << ", " << line.p1().y() << ", " << line.p1().z() << "), (" << line.p2().x() << ", " << line.p2().y() << ", " << line.p2().z() << "))";
    return dbg;
}

#endif
