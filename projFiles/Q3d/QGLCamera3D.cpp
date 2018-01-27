#include "QGLCamera3D.h"

QGLCamera3D::QGLCamera3D()
    : _projIsActual(false), 
      _viewIsActual(false), 
      _type(Perspective),  
      _rect(QRectF(0.0f, 0.0f, 100.0f, 100.0f)), 
      _fov(45.0f),
      _nearPlane(0.1f), 
      _farPlane(100.0f),
      _aspect(1.0f),
      _position(0.0f, 0.0f, 5.0f), 
      _up(0.0f, 1.0f, 0.0f), 
      _view(_center - _position)
{
}

QGLCamera3D::QGLCamera3D(const QRectF &rect, float nearPlane, float farPlane)
    : _projIsActual(false), 
      _viewIsActual(false),
      _type(Orthographic), 
      _rect(rect), 
      _fov(0.0f),
      _nearPlane(nearPlane), 
      _farPlane(farPlane),
      _aspect(_rect.width() / _rect.height()),
      _position(0.0f, 0.0f, 5.0f), 
      _up(0.0f, 1.0f, 0.0f), 
      _view(_center - _position)
{
}

QGLCamera3D::QGLCamera3D(const QRectF &rect, float fov, float nearPlane, float farPlane)
    : _projIsActual(false), 
      _viewIsActual(false),
      _type(Perspective), 
      _rect(rect), 
      _fov(fov),
      _nearPlane(nearPlane), 
      _farPlane(farPlane),
      _aspect(_rect.width() / _rect.height()),
      _position(0.0f, 0.0f, 5.0f), 
      _up(0.0f, 1.0f, 0.0f), 
      _view(_center - _position)
{
}

QGLCamera3D::~QGLCamera3D()
{
}

//**************************************************************************************************************************************

QMatrix4x4 QGLCamera3D::mProj() 
{
    if(!_projIsActual)
    {
        _mProj.setToIdentity();
        switch(_type)
        {
            case Orthographic:
                _mProj.ortho(_rect.left(), _rect.right(), _rect.bottom(), _rect.top(), _nearPlane, _farPlane);
                break;
            case Perspective:
                _mProj.perspective(_fov, _aspect, _nearPlane, _farPlane);
                break;
            default : break;
        }
        _projIsActual = true;
    }
    return _mProj;
}

QMatrix4x4 QGLCamera3D::mView() 
{
    if(!_viewIsActual)
    {
        _mView.setToIdentity();
        _mView.lookAt(_position, _center, _up);
        _viewIsActual = true;
    }
    return _mView;
}

QMatrix4x4 QGLCamera3D::mViewProj() 
{
    if(!_projIsActual || !_viewIsActual)_mViewProj = mProj() * mView();
    return _mViewProj;
}

//**************************************************************************************************************************************

void QGLCamera3D::setRect(const QRectF &value)
{
    if(_rect != value)
    {
        _rect = value;
        _aspect = _rect.width() / _rect.height();
        _projIsActual = false;
    }
}

void QGLCamera3D::setFov(float value)
{
    if(!qFuzzyCompare(_farPlane, value))
    {
        _fov = value;
        _projIsActual = false;
    }
}

void QGLCamera3D::setNearPlane(float value)
{
    if(!qFuzzyCompare(_farPlane, value))
    {
        _nearPlane = value;
        _projIsActual = false;
    }
}

void QGLCamera3D::setFarPlane(float value)
{
    if(!qFuzzyCompare(_farPlane, value))
    {
        _farPlane = value;
        _projIsActual = false;
    }
}

void QGLCamera3D::setPosition(const QVector3D &value)
{
    if(!qFuzzyCompare(_position, value))
    {
        _position = value;
        _view = _center - _position;
        _viewIsActual = false;
    }
}

void QGLCamera3D::setCenter(const QVector3D &value)
{
    if(!qFuzzyCompare(_center, value))
    {
        _center = value;
        _view = _center - _position;
        _viewIsActual = false;
    }
}

void QGLCamera3D::setUp(const QVector3D &value)
{
    if(!qFuzzyCompare(_up, value))
    {
        _up = value;
        _viewIsActual = false;
    }
}

//**************************************************************************************************************************************

void QGLCamera3D::translate(float value)
{
    if(!qFuzzyIsNull(value))
    {
        setPosition(_position + _view.normalized() * value);
    }
}

void QGLCamera3D::rotateAroundCenter(float value)
{
    if(!qFuzzyIsNull(value))
    {
        setPosition(QQuaternion::fromAxisAndAngle(_up, value) * _position);
    }
}

void QGLCamera3D::rotateAroundUp(float value)
{
    if(!qFuzzyIsNull(value))
    {
        setCenter(QQuaternion::fromAxisAndAngle(_up, value) * _view + _position);
    }
}

void QGLCamera3D::rotateAroundView(float value)
{
    if(!qFuzzyIsNull(value))
    {
        setUp(QQuaternion::fromAxisAndAngle(_view, value) * _up);
    }
}

void QGLCamera3D::rotateAroundVector(const QVector3D &vector, float angle)
{
    if(!qFuzzyIsNull(angle))
    {
        setPosition(QQuaternion::fromAxisAndAngle(vector, angle) * _position);

        _up = QQuaternion::fromAxisAndAngle(vector, angle) * _up;

        _viewIsActual = false;

    }
}
