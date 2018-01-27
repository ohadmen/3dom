#pragma once
#include <QMatrix4x4>


class QGLCamera3D : public QObject
{
Q_OBJECT
public:
    enum Type{Orthographic, Perspective};
public:
    QGLCamera3D ();
    QGLCamera3D (const QRectF &rect, float nearPlane, float farPlane);
    QGLCamera3D (const QRectF &rect, float fov, float nearPlane, float farPlane);
    ~QGLCamera3D();

public:
    QMatrix4x4 mProj    ();
    QMatrix4x4 mView    ();
    QMatrix4x4 mViewProj();
    QRectF     rect     () const {return _rect;}
    float      fov      () const {return _fov;}
    float      nearPlane() const {return _nearPlane;}
    float      farPlane () const {return _farPlane;}
    float      aspect   () const {return _aspect;}
    QVector3D  position () const {return _position;}
    QVector3D  center   () const {return _center;}
    QVector3D  up       () const {return _up;}
    QVector3D  view     () const {return _view;}


    void setRect     (const QRectF &value);
    void setFov      (float value);
    void setNearPlane(float value);
    void setFarPlane (float value);
    void setPosition (const QVector3D &value);
    void setCenter   (const QVector3D &value);
    void setUp       (const QVector3D &value);


    void translate          (float value);
    void rotateAroundCenter (float value);
    void rotateAroundUp     (float value);
    void rotateAroundView   (float value);

    void rotateAroundVector (const QVector3D &vector, float angle);
protected:

private:
    Type       _type; 
    QMatrix4x4 _mProj;
    QMatrix4x4 _mView;
    QMatrix4x4 _mViewProj;
    bool       _projIsActual;
    bool       _viewIsActual;

    QRectF     _rect;
    float      _fov;
    float      _nearPlane;
    float      _farPlane;
    float      _aspect;

    QVector3D  _position;
    QVector3D  _center;
    QVector3D  _up;
    QVector3D  _view;
};

