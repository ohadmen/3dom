#include "QCamera3D.h"

QCamera3D::QCamera3D()
    :
    _type(Perspective),
    _mProjDirty       (true),
    _mViewDirty       (true),
    _mViewProjDirty   (true),
    _mViewProjInvDirty(true),
    _nearPlane  (  0.1f),
    _farPlane   (100.0f),
    _fieldOfView( 45.0f),
    _aspectRatio(  1.0f),
    _left       ( -0.5f),
    _top        (  0.5f),
    _right      (  0.5f),
    _bottom     ( -0.5f),
    _width      (_right - _left),
    _height     (_bottom - _top),
    _pos        (0.0f, 0.0f, 5.0f),
    _center     (0.0f, 0.0f, 0.0f),
    _up         (0.0f, 1.0f, 0.0f),
    _view       (_center - _pos)
{
}

QCamera3D::~QCamera3D()
{
}

//************************************************************************************************

void QCamera3D::doDirtyProj() //Перенести
{
    _mProjDirty        = true; 
    _mViewProjDirty    = true;
    _mViewProjInvDirty = true;

}

void QCamera3D::doDirtyView() 
{
    _mViewDirty        = true;
    _mViewProjDirty    = true;
    _mViewProjInvDirty = true;
}

//************************************************************************************************

QMatrix4x4 QCamera3D::mProj() 
{
    if(_mProjDirty)
    {
        _mProj.setToIdentity();
        switch(_type)
        {
            case Orthographic:
                _mProj.ortho(_left, _right, _bottom, _top, _nearPlane, _farPlane);
                break;
            case Perspective:
                _mProj.perspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane);
                break;
            case Frustum:
                _mProj.frustum(_left, _right, _bottom, _top, _nearPlane, _farPlane);
                break;
            default : break;
        }
        _mProjDirty = false;
    }
    return _mProj;
}

QMatrix4x4 QCamera3D::mView() 
{
    if(_mViewDirty)
    {
        _mView.setToIdentity();
        _mView.lookAt(_pos, _center, _up);
        _mViewDirty = false;
    }
    return _mView;
}

QMatrix4x4 QCamera3D::mViewProj() 
{
    if(_mViewProjDirty)
    {
        _mViewProj = mProj() * mView();
        _mViewProjDirty = false;
    }
    return _mViewProj;
}

QMatrix4x4 QCamera3D::mViewProjInv()
{
    if(_mViewProjInvDirty)
    {
        _mViewProjInv = mViewProj().inverted();
        _mViewProjInvDirty = true;
    }
    return _mViewProjInv;
}

//****************************************************************************************************

void QCamera3D::setType(Type value)
{
    if(_type != value)
    {
        _type = value;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setNearPlane(float value)
{
    if(!qFuzzyCompare(_nearPlane, value))
    {
        _nearPlane = value;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setFarPlane(float value)
{
    if(!qFuzzyCompare(_farPlane, value))
    {
        _farPlane = value;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setPlanes(float nearPlane, float farPlane)
{
    if(!qFuzzyCompare(_nearPlane, nearPlane) || !qFuzzyCompare(_farPlane, farPlane))
    {
        _nearPlane = nearPlane;
        _farPlane = farPlane;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setFieldOfView(float value)
{
    if(!qFuzzyCompare(_fieldOfView, value))
    {
        _fieldOfView = value;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setLeft(float value)
{
    if(!qFuzzyCompare(_left, value))
    {
        _left = value;
        _width = _right - _left;
        _aspectRatio = _width / _height;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setTop(float value)
{
    if(!qFuzzyCompare(_top, value))
    {
        _top = value;
        _height = _bottom - _top;
        _aspectRatio = _width / _height;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setRight(float value)
{
    if(!qFuzzyCompare(_right, value))
    {
        _right = value;
        _width = _right - _left;
        _aspectRatio = _width / _height;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setBottom(float value)
{
    if(!qFuzzyCompare(_bottom, value))
    {
        _bottom = value;
        _height = _bottom - _top;
        _aspectRatio = _width / _height;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setRect(float left, float top, float right, float bottom)
{
    if(!qFuzzyCompare(_left, left) || !qFuzzyCompare(_top, top) || !qFuzzyCompare(_right, right) || !qFuzzyCompare(_bottom, bottom))
    {
        _left   = left;
        _top    = top;
        _right  = right;
        _bottom = bottom;
        _width  = _right - _left;
        _height = _bottom - _top;
        _aspectRatio = _width / _height;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setSize(float width, float height)
{
    if(!qFuzzyCompare(_width, width) || !qFuzzyCompare(_height, height))
    {
        _width  = width;
        _height = height;
        _right  = _left + _width;
        _bottom = _top  + _height;
        _aspectRatio = _width / _height;
        doDirtyProj();
//        _mProjDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;

        calcFrustumMetrics();//!!!!!!!!!!!!!!

    }
}

void QCamera3D::setPos(const QVector3D &value)
{
    if(!qFuzzyCompare(_pos, value))
    {
        _pos = value;
        _view = _center - _pos;
        doDirtyView();
//        _mViewDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;

        calcFrustumMetrics();//!!!!!!!!!!!!!!

    }
}

void QCamera3D::setCenter(const QVector3D &value)
{
    if(!qFuzzyCompare(_center, value))
    {
        _center = value;
        _view = _center - _pos;
        doDirtyView();
//        _mViewDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

void QCamera3D::setUp(const QVector3D &value)
{
    if(!qFuzzyCompare(_up, value))
    {
        _up = value;
        doDirtyView();
//        _mViewDirty = true;
//        _mViewProjDirty = true;
//        _mViewProjInvDirty = true;
    }
}

//****************************************************************************************************

void QCamera3D::rayFromScene(const QVector2D &p, QVector3D &pNear, QVector3D &pFar)
{
    QVector2D d(p.x() * 2.0f / _width - 1.0f, -p.y() * 2.0f / _height + 1.0f);
    pNear = mViewProjInv() * (QVector3D(d, -1.0f));
    pFar  = mViewProjInv() * (QVector3D(d,  1.0f));
}

//****************************************************************************************************

void QCamera3D::calcFrustumMetrics()
{
#if 0
    float k1 = 2.0f * tanf(_fieldOfView * DEGRESS / 2.0f);
    float hNear = k1 * _nearPlane;
    float hFar  = k1 * _farPlane;
    float wNear = hNear * _aspectRatio;
    float wFar  = hFar  * _aspectRatio;

    QVector3D v1(_view.normalized());
    QVector3D v2(_up.normalized());
    QVector3D v3(QVector3D::crossProduct(v2, v1));

    QVector3D fc = _pos + v1 * _farPlane;
    QVector3D nc = _pos + v1 * _nearPlane;

    QVector3D v4 = v2 * hFar / 2.0f;
    QVector3D v5 = v3 * wFar / 2.0f;
    QVector3D v6 = v2 * hNear / 2.0f;
    QVector3D v7 = v3 * wNear / 2.0f;

    QVector3D ftl = fc + v4 - v5;
    QVector3D ftr = fc + v4 + v5;
	QVector3D fbl = fc - v4 - v5;
	QVector3D fbr = fc - v4 + v5;

    QVector3D ntl = nc + v6 - v7;
    QVector3D ntr = nc + v6 + v7;
    QVector3D nbl = nc - v6 - v7;
    QVector3D nbr = nc - v6 + v7;

//------------------------------------------------------------
//    QVector4D left   = calcFrustumPlane(3, 0, true);
//    QVector4D right  = calcFrustumPlane(3, 0, false);
//    QVector4D top    = calcFrustumPlane(3, 1, true);
//    QVector4D bottom = calcFrustumPlane(3, 1, false);
//    QVector4D near   = calcFrustumPlane(3, 2, true);
//    QVector4D far    = calcFrustumPlane(3, 2, false);
//    QVector4D vr = far;
//    _v1 = vr.toVector3D() * -vr[3];
//    qDebug() << vr;
#endif

    calcFrustumCorners();
    calcFrustumPlanes();
//------------------------------------------------------------

}

QVector4D QCamera3D::calcFrustumPlane(int i1, int i2, bool sign)
{
    QMatrix4x4 m(mViewProj());
    QVector3D v(sign ? QVector3D(m(i1, 0) + m(i2, 0), m(i1, 1) + m(i2, 1), m(i1, 2) + m(i2, 2)) :
                       QVector3D(m(i1, 0) - m(i2, 0), m(i1, 1) - m(i2, 1), m(i1, 2) - m(i2, 2)));
    float d = sign ? m(i1, 3) + m(i2, 3) : m(i1, 3) - m(i2, 3);
    float l = v.length();
    v /= l; 
    d /= l;
    return QVector4D(v, d);
}

void QCamera3D::calcFrustumCorners()
{
    _v[0] = mViewProjInv() * QVector3D(-1.0f, -1.0f, -1.0f);
    _v[1] = mViewProjInv() * QVector3D( 1.0f, -1.0f, -1.0f);
    _v[2] = mViewProjInv() * QVector3D( 1.0f,  1.0f, -1.0f);
    _v[3] = mViewProjInv() * QVector3D(-1.0f,  1.0f, -1.0f);

    _v[4] = mViewProjInv() * QVector3D(-1.0f, -1.0f,  1.0f);
    _v[5] = mViewProjInv() * QVector3D( 1.0f, -1.0f,  1.0f);
    _v[6] = mViewProjInv() * QVector3D( 1.0f,  1.0f,  1.0f);
    _v[7] = mViewProjInv() * QVector3D(-1.0f,  1.0f,  1.0f);
}

void QCamera3D::calcFrustumPlanes()
{
    QMatrix4x4 m(mViewProj());
    float l = 0.0f;

    QVector3D left(m(3, 0) + m(0, 0), m(3, 1) + m(0, 1), m(3, 2) + m(0, 2));
    float dLeft = m(3, 3) + m(0, 3); 
    l = left.length();
    left /= -l; 
    dLeft /= l;
    _p[0].redefine(left, dLeft);

    QVector3D right(m(3, 0) - m(0, 0), m(3, 1) - m(0, 1), m(3, 2) - m(0, 2));
    float dRight = m(3, 3) - m(0, 3);
    l = right.length();
    right /= -l; 
    dRight /= l;
    _p[1].redefine(right, dRight);


    QVector3D top(m(3, 0) - m(1, 0), m(3, 1) - m(1, 1), m(3, 2) - m(1, 2));
    float dTop = m(3, 3) - m(1, 3);
    l = top.length();
    top /= -l; 
    dTop /= l;
    _p[2].redefine(top, dTop);

    QVector3D bottom(m(3, 0) + m(1, 0), m(3, 1) + m(1, 1), m(3, 2) + m(1, 2));
    float dBottom = m(3, 3) + m(1, 3);
    l = bottom.length();
    bottom /= -l; 
    dBottom /= l;
    _p[3].redefine(bottom, dBottom);


    QVector3D nearP(m(3, 0) + m(2, 0), m(3, 1) + m(2, 1), m(3, 2) + m(2, 2));
    float dNear = m(3, 3) + m(2, 3);
    l = nearP.length();
    nearP /= -l; 
    dNear /= l;

    QVector3D farP(m(3, 0) - m(2, 0), m(3, 1) - m(2, 1), m(3, 2) - m(2, 2));
    float dFar = m(3, 3) - m(2, 3);
    l = farP.length();
    farP /= -l; 
    dFar /= l;


    _v1 = left * dLeft;
    _v2 = right * dRight;
    _v3 = top * dTop;
    _v4 = bottom * dBottom;
    _v5 = nearP * dNear;
    _v6 = farP * dFar;

}

void QCamera3D::rotateAroundVector(const QVector3D &vector, float angle)
{
    if(!qFuzzyIsNull(angle))
    {
        setPos(QQuaternion::fromAxisAndAngle(vector, angle) * _pos);
        _up = QQuaternion::fromAxisAndAngle(vector, angle) * _up;
        doDirtyView();
    }
}
