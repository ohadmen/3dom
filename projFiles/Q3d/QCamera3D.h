#pragma once

#include <QtGui>
//#include <QVector2D>
//#include <QVector3D>
#include <QMatrix4x4>
#include "QPlane3D.h"

class QCamera3D : public QObject
{
Q_OBJECT
public:
    enum Type{Orthographic, Perspective, Frustum};
public:
    QCamera3D();
   ~QCamera3D();
public:
    QMatrix4x4 mProj       ();
    QMatrix4x4 mView       ();
    QMatrix4x4 mViewProj   ();
    QMatrix4x4 mViewProjInv();

    Type       type          () const {return _type;}
    float      nearPlane     () const {return _nearPlane;}
    float      farPlane      () const {return _farPlane;}
    float      fieldOfView   () const {return _fieldOfView;}
    float      aspectRatio   () const {return _aspectRatio;}
    float      left          () const {return _left;}
    float      right         () const {return _right;}
    float      bottom        () const {return _bottom;}
    float      top           () const {return _top;}

    QVector3D  pos           () const {return _pos;}
    QVector3D  center        () const {return _center;}
    QVector3D  up            () const {return _up;}
    QVector3D  view          () const {return _view;}


    void setType          (Type  value);
    void setNearPlane     (float value);
    void setFarPlane      (float value);
    void setPlanes        (float nearPlane, float farPlane);
    void setFieldOfView   (float value);
    void setLeft          (float value);
    void setTop           (float value);
    void setRight         (float value);
    void setBottom        (float value);
    void setRect          (float left, float top, float right, float bottom);
    void setSize          (float width, float height);


    void setPos           (const QVector3D &value);
    void setCenter        (const QVector3D &value);
    void setUp            (const QVector3D &value);


//********
    void rayFromScene     (const QVector2D &p, QVector3D &near, QVector3D &far);
    void rotateAroundVector(const QVector3D &vector, float angle);

    QVector3D _v1, _v2, _v3, _v4, _v5, _v6;
    void calcFrustumMetrics();

//********

protected:
    void doDirtyProj(); 
    void doDirtyView(); 



    QVector4D calcFrustumPlane(int i1, int i2, bool sign);
    void calcFrustumCorners();
    void calcFrustumPlanes();


private:
    Type       _type; 
    QMatrix4x4 _mProj;
    QMatrix4x4 _mView;
    QMatrix4x4 _mViewProj;
    QMatrix4x4 _mViewProjInv;

    bool       _mProjDirty;
    bool       _mViewDirty;
    bool       _mViewProjDirty;
    bool       _mViewProjInvDirty;


    float      _nearPlane;
    float      _farPlane;
    float      _fieldOfView;
    float      _aspectRatio;
    float      _left;
    float      _top;
    float      _right;
    float      _bottom;
    float      _width;
    float      _height;


    QVector3D  _pos;
    QVector3D  _center;
    QVector3D  _up;
    QVector3D  _view;

public:
    QPlane3D _p[10];
    QVector3D _v[8];


};

