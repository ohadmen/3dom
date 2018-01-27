#pragma once

#include <QtGui>
//#include <QVector3D>

#include "QRay3D.h"
#include "QPlane3D.h"
#include "QLine3D.h"
#include "QSphere3D.h"

#define PI      3.14159265358
#define PI_M2   6.28318530718
#define RADIAN  57.2957795131
#define DEGRESS 0.01745329251

//#define INTERNAL_COMPARE
#define INTERNAL_COMPARE_EPS    0.00001

class QGeometry3D
{
public:
    QGeometry3D();
   ~QGeometry3D();
public:

    static inline bool compare(float p1, float p2)
    {
#ifdef INTERNAL_COMPARE
        return qAbs(double(p1) - double(p2)) < INTERNAL_COMPARE_EPS;
#else
        return qFuzzyCompare(p1, p2);
#endif
    }

    static inline bool compare(QVector3D p1, QVector3D p2)
    {
#ifdef INTERNAL_COMPARE
    return compare(p1.x(), p2.x()) && compare(p1.y(), p2.y()) && compare(p1.z(), p2.z());
#else
        return qFuzzyCompare(p1, p2);
#endif
    }

//****************************************************************************************************
//  Ray & Point
//****************************************************************************************************
//----------------------------------------------------------------------------------------------------
//Возвращает true если point принадлежит ray
//----------------------------------------------------------------------------------------------------
    static inline bool contains(const QRay3D &ray, const QVector3D &point)
    {
        QVector3D v(point - ray.origin());
        if(v.isNull())return true;
        float dot = QVector3D::dotProduct(v, ray.direction());
        if(qFuzzyIsNull(dot))return false;
        return compare(dot * dot, v.lengthSquared() * ray.direction().lengthSquared());
    }

//----------------------------------------------------------------------------------------------------
//Возвращает расстояние между ray.origin() и проекцией точки point на ray
//----------------------------------------------------------------------------------------------------
    static inline float distanceToProject(const QRay3D &ray, const QVector3D &point)
    {
        return QVector3D::dotProduct(ray.direction(), point - ray.origin()) / ray.direction().lengthSquared();
    }

//----------------------------------------------------------------------------------------------------
//Возвращает точку p принадлежащую ray, такую, что вектор ray.direction() 
//перпендикулярен вектору (point - p)
//----------------------------------------------------------------------------------------------------
    static inline QVector3D normal(const QRay3D &ray, const QVector3D &point)
    {
        return ray.point(distanceToProject(ray, point));
    }

//----------------------------------------------------------------------------------------------------
//Возвращает длину нормали (point - p), где p принадлежит ray, а вектор ray.direction() 
//перпендикулярен вектору (point - p)
//----------------------------------------------------------------------------------------------------
    static inline float distance(const QRay3D &ray, const QVector3D &point)
    {
        return (normal(ray, point) - point).length();
    }

//****************************************************************************************************
//  Ray & Ray
//****************************************************************************************************
//----------------------------------------------------------------------------------------------------
//Возвращает true если ray2 принадлежит ray1
//----------------------------------------------------------------------------------------------------
    static inline bool contains(const QRay3D &ray1, const QRay3D &ray2)
    {
        float dot = QVector3D::dotProduct(ray1.direction(), ray2.direction());
        if(!compare(dot * dot, ray1.direction().lengthSquared() * ray2.direction().lengthSquared()))return false;
        return contains(ray1, ray2.origin());
    }

//----------------------------------------------------------------------------------------------------
//Точки p1 и p2 лежат на ray1 и ray2 соответственно, причем, вектор (p1 - p2) ортогонален ray1.direction() 
//и ray2.direction() и его длина является расстоянием между ray1 и ray2.
//Возвращяет dist, где dist.x() расстояние от ray1.origin() до p1, dist.y() - ray2.origin() до p2
//----------------------------------------------------------------------------------------------------
    static inline void distanceToProject(const QRay3D &ray1, const QRay3D &ray2, QVector2D &dist)
    {
        QVector3D diff = ray1.origin() - ray2.origin();
        float k0 = -QVector3D::dotProduct(ray1.direction(), ray2.direction());
        float k1 =  QVector3D::dotProduct(diff, ray1.direction());
        if(qAbs(k0) < 1.0f)
        {
            float k2 = -QVector3D::dotProduct(diff, ray2.direction());
            dist = QVector2D(k0 * k2 - k1, k0 * k1 - k2);
#if 0   
//Не выходит за ray1.origin()
            if(dist.x() >= 0.0f)dist = dist.y() >= 0.0f ? dist / (1.0f - k0 * k0) : QVector2D(k1 >= 0.0f ? 0.0f : -k1, 0.0f); else
                dist = dist.y() >= 0.0f ? QVector2D(0.0f, k2 >= 0.0f ? 0.0f : -k2) : (k1 < 0.0f ? QVector2D(-k1, 0.0f) : QVector2D(0.0f, k2 >= 0.0f ? 0.0f : -k2));
#else
            if(dist.x() >= 0.0f)dist /=  (1.0f - k0 * k0); else
                dist = dist.y() >= 0.0f ? QVector2D(0.0f, k2 >= 0.0f ? 0.0f : -k2) : (k1 < 0.0f ? QVector2D(-k1, 0.0f) : QVector2D(0.0f, k2 >= 0.0f ? 0.0f : -k2));
#endif
        }else 
        {
            dist = k0 > 0.0f ? QVector2D(k1 >= 0.0f ? 0.0f : -k1, 0.0f) : k1 >= 0.0f ? QVector2D(0.0f, QVector3D::dotProduct(diff, ray2.direction())) : QVector2D(-k1, 0.0f);
        }
    }
    
//----------------------------------------------------------------------------------------------------
//Точки p1 и p2 лежат на ray1 и ray2 соответственно, причем, вектор (p1 - p2) ортогонален ray1.direction() 
//и ray2.direction() и его длина является расстоянием между ray1 и ray2. 
//----------------------------------------------------------------------------------------------------
    static inline void normal(const QRay3D &ray1, const QRay3D &ray2, QVector3D &p1, QVector3D &p2)
    {
        QVector2D dist;
        distanceToProject(ray1, ray2, dist);
        p1 = ray1.origin() + dist.x() * ray1.direction();
        p2 = ray2.origin() + dist.y() * ray2.direction();
    }

//----------------------------------------------------------------------------------------------------
//Возвращяет расстояние между ray1 и ray2. Точки p1 и p2 лежат на ray1 и ray2 соответственно, причем, 
//вектор (p1 - p2) ортогонален ray1.direction() и ray2.direction() и его длина является расстоянием между ray1 и ray2. 
//----------------------------------------------------------------------------------------------------
    static inline float distance(const QRay3D &ray1, const QRay3D &ray2, QVector3D &p1, QVector3D &p2)
    {
        normal(ray1, ray2, p1, p2);
        return (p2 - p1).length();
    }

//----------------------------------------------------------------------------------------------------
//Возвращяет true, если ray1 и ray2 пересекаются. p - точка пересечения
//----------------------------------------------------------------------------------------------------
    static inline bool intersects(const QRay3D &ray1, const QRay3D &ray2, QVector3D &p)
    {
        QVector3D p1, p2;
        normal(ray1, ray2, p1, p2);
        p = (p1 + p2) / 2.0f;
        return compare(p1, p2);
    }

//****************************************************************************************************
//  Plane & Point
//****************************************************************************************************
//----------------------------------------------------------------------------------------------------
//Возвращяет true, если point принадлежит plane
//----------------------------------------------------------------------------------------------------
    static inline bool contains(const QPlane3D &plane, const QVector3D &point)
    {
        return qFuzzyIsNull(distance(plane, point));
    }

//----------------------------------------------------------------------------------------------------
//Возвращяет расстояние между plane и point
//----------------------------------------------------------------------------------------------------
    static inline float distance(const QPlane3D &plane, const QVector3D &point)
    {
        return QVector3D::dotProduct(plane.normal(), point) - plane.distance();
    }

//----------------------------------------------------------------------------------------------------
//Возвращяет ray ортогональный plane с началом в point
//----------------------------------------------------------------------------------------------------
    static inline void normal(const QPlane3D &plane, const QVector3D &point, QRay3D &ray)
    {
        ray.setOrigin(point);
        ray.setDirection(-plane.normal());
    }

//----------------------------------------------------------------------------------------------------
//Возвращяет p принадлежащий plane, причем (point - p) ортогонален plane
//----------------------------------------------------------------------------------------------------
    static inline void normal(const QPlane3D &plane, const QVector3D &point, QVector3D &p)
    {
        p = point - plane.normal() * distance(plane, point);
    }


//****************************************************************************************************
//  Plane & Ray
//****************************************************************************************************
//----------------------------------------------------------------------------------------------------
//Возвращяет dist - расстояние от ray.origin() до точки пересечения ray и plane
//----------------------------------------------------------------------------------------------------
    static inline bool distanceToIntersection(const QRay3D &ray, const QPlane3D &plane, float &dist)
    {
        float denom = QVector3D::dotProduct(plane.normal(), ray.direction());
        if(qFuzzyIsNull(denom))
        {
            dist = qSNaN();
            return false;
        }
        dist = -(QVector3D::dotProduct(plane.normal(), ray.origin()) - plane.distance()) / denom;
        return true;
    }

//----------------------------------------------------------------------------------------------------
//Возвращяет true, если ray и plane пересекаются. p - точка пересечения ray и plane
//----------------------------------------------------------------------------------------------------
    static inline bool intersects(const QRay3D &ray, const QPlane3D &plane, QVector3D &p)
    {
        float t = 0.0f;
        if(!distanceToIntersection(ray, plane, t))return false;
        p = ray.point(t);
        return true;
    }

//****************************************************************************************************
//  Plane & Plane
//****************************************************************************************************
//----------------------------------------------------------------------------------------------------
//Возвращяет true, если plane0 и plane1 пересекаются. ray - лучь лежащий на пересечении plane0 и plane1
//----------------------------------------------------------------------------------------------------
    static inline bool intersects(const QPlane3D &plane0, const QPlane3D &plane1, QRay3D &ray)
    {
        float dot = QVector3D::dotProduct(plane0.normal(), plane1.normal());
        if(qAbs(dot) >= 1.0f)
        {
            float cDiff = dot >= 0.0f ? plane0.distance() - plane1.distance() : cDiff = plane0.distance() + plane1.distance();
            if(qFuzzyIsNull(qAbs(cDiff)))
            {
                return true; //plane0 содержит plane1
            }
            return false;
        }
        float invDet = 1.0f / (1.0f - dot * dot);
        float c0 = (plane0.distance() - dot * plane1.distance()) * invDet;
        float c1 = (plane1.distance() - dot * plane0.distance()) * invDet;
        ray.setOrigin(c0 * plane0.normal() + c1 * plane1.normal());
        ray.setDirection(QVector3D::crossProduct(plane0.normal(), plane1.normal()).normalized());
        return true;
    }

//****************************************************************************************************
//  Line & Point
//****************************************************************************************************
//----------------------------------------------------------------------------------------------------
//Возвращает true если point принадлежит line
//----------------------------------------------------------------------------------------------------
    static inline bool contains(const QLine3D &line, const QVector3D &point)
    {
        QVector3D v1(point - line.p1());
        if(v1.isNull())return true;
        float d1 = QVector3D::dotProduct(v1, line.direction());
        if(qFuzzyIsNull(d1))return false;
        bool res = compare(d1 * d1, v1.lengthSquared());
        if(!res)return false;
        QVector3D v2(point - line.p2());
        float d2 = QVector3D::dotProduct(v2, line.direction());
        return (d1 * d1 < line.lengthSquared()) && (d2 * d2 < line.lengthSquared());
    }

//----------------------------------------------------------------------------------------------------
//Возвращает расстояние между line.p1() и проекцией точки point на line
//----------------------------------------------------------------------------------------------------
    static inline float distanceToProject(const QLine3D &line, const QVector3D &point)
    {
        return distanceToProject(line.toRay3D(), point);
    }

//----------------------------------------------------------------------------------------------------
//Возвращает точку p принадлежащую line, такую, что вектор line.direction() 
//перпендикулярен вектору (point - p)
//----------------------------------------------------------------------------------------------------
    static inline QVector3D normal(const QLine3D &line, const QVector3D &point)
    {
        return line.point(distanceToProject(line, point));
    }

//----------------------------------------------------------------------------------------------------
//Возвращает длину нормали (point - p), где p принадлежит line, а вектор line.direction() 
//перпендикулярен вектору (point - p)
//----------------------------------------------------------------------------------------------------
    static inline float distance(const QLine3D &line, const QVector3D &point)
    {
        return (normal(line, point) - point).length();
    }

//#####################################################################################################################
//#####################################################################################################################
//#####################################################################################################################
//#####################################################################################################################
//#####################################################################################################################
//#####################################################################################################################
//#####################################################################################################################


//****************************************************************************************************
//  Line & Ray
//****************************************************************************************************
//----------------------------------------------------------------------------------------------------
//Возвращает true если line принадлежит ray
//----------------------------------------------------------------------------------------------------
    static inline bool contains(const QLine3D &line, const QRay3D &ray)
    {
        return contains(ray, line.p1()) && contains(ray, line.p2());
    }

//----------------------------------------------------------------------------------------------------
//Точки p1 и p2 лежат на line и ray соответственно, причем, вектор (p1 - p2) ортогонален line.direction() 
//и ray.direction() и его длина является расстоянием между line и ray.
//Возвращяет dist, где dist.x() расстояние от line.p1() до p1, dist.y() - ray.origin() до p2
//----------------------------------------------------------------------------------------------------
    static inline void distanceToProject(const QLine3D &line, const QRay3D &ray, QVector2D &dist)
    {
        distanceToProject(line.toRay3D(), ray, dist);
    }

//----------------------------------------------------------------------------------------------------
//Точки p1 и p2 лежат на line и ray соответственно, причем, вектор (p1 - p2) ортогонален line.direction() 
//и ray.direction() и его длина является расстоянием между line и ray.
//----------------------------------------------------------------------------------------------------
    static inline void normal(const QLine3D &line, const QRay3D &ray, QVector3D &p1, QVector3D &p2)
    {
        normal(line.toRay3D(), ray, p1, p2);
    }

//----------------------------------------------------------------------------------------------------
//Возвращяет расстояние между line и ray. Точки p1 и p2 лежат на line и ray соответственно, причем, 
//вектор (p1 - p2) ортогонален line.direction() и ray.direction() и его длина является расстоянием между line и ray2. 
//----------------------------------------------------------------------------------------------------
    static inline float distance(const QLine3D &line, const QRay3D &ray, QVector3D &p1, QVector3D &p2)
    {
        return distance(line.toRay3D(), ray, p1, p2);
    }

//----------------------------------------------------------------------------------------------------
//Возвращяет true, если line и ray пересекаются. p - точка пересечения
//----------------------------------------------------------------------------------------------------
    static inline bool intersects(const QLine3D &line, const QRay3D &ray, QVector3D &p)
    {
        return intersects(line.toRay3D(), ray, p) && contains(line, p);
    }


//****************************************************************************************************
//  Line & Line
//****************************************************************************************************
//----------------------------------------------------------------------------------------------------
//Точки p1 и p2 лежат на line1 и line2 соответственно, причем, вектор (p1 - p2) ортогонален line1.direction() 
//и line2.direction() и его длина является расстоянием между line1 и line2.
//Возвращяет dist, где dist.x() расстояние от line1.p1() до p1, dist.y() - line2.p1() до p2
//----------------------------------------------------------------------------------------------------

    static inline void distanceToProject(const QLine3D &line1, const QLine3D &line2, QVector2D &dist)
    {
        distanceToProject(line1.toRay3D(), line2.toRay3D(), dist);
    }
//----------------------------------------------------------------------------------------------------
//Возвращяет true, если line1 и line2 пересекаются. p - точка пересечения
//----------------------------------------------------------------------------------------------------
    static inline bool intersects(const QLine3D &line1, const QLine3D &line2, QVector3D &p)
    {
        return intersects(line1.toRay3D(), line2.toRay3D(), p) && contains(line1, p) && contains(line2, p);
    }


//########################################################################################################

//----------------------------------------------------------------------------------------------------
//Возвращяет true, если sphere и ray пересекаются. p - точка пересечения
//----------------------------------------------------------------------------------------------------

    static inline bool intersects(const QSphere3D &sphere, const QRay3D &ray, QVector3D &p)
    {
#if 0
        const QVector3D v1 = ray.origin       ();
        const QVector3D v2 = ray.direction    ();
        const QVector3D v3 = v1 - sphere.center();
        const float c = QVector3D::dotProduct(v3, v3) - sphere.radius() * sphere.radius();
//    if (q == nullptr && c <= 0.0f)return true;
        const float b = QVector3D::dotProduct(v3, v2);
        if (c > 0.0f && b > 0.0f)return false;
        const float discr = b * b - c;
        if(discr < 0.0f)return false;
//    if (q == nullptr)return true;
        float t = -b - sqrt(discr);
        if(t < 0.0f)t = 0.0f;
        p = v1 + t * v2;
        return true;
#else
        const QVector3D v = ray.origin() - sphere.center();
        const float k1 = QVector3D::dotProduct(v, v) - sphere.radius() * sphere.radius();
        const float k2 = QVector3D::dotProduct(v, ray.direction());
        if (k1 > 0.0f && k2 > 0.0f)return false;
        const float k3 = k2 * k2 - k1;
        if(k3 < 0.0f)return false;
        float k4 = -k2 - sqrt(k3);
        if(k4 < 0.0f)k4 = 0.0f;
        p = ray.origin() + k4 * ray.direction();
        return true;
#endif
    }

};

