#pragma once

#include <QWindow>
#include <QOpenGLContext>
#include <QMatrix4x4>

#include <QWheelEvent>

#include "QGLCamera3D.h"
#include "QCamera3D.h"

#include "QGeometry3D.h"
#include "QRay3D.h"
#include "QPlane3D.h"
#include "QLine3D.h"
#include "QSphere3D.h"

#include "QPrimitive3D.h"

#include "QPainter3D.h"

#include "QBitFlags.h"


class CALDLMainWindow : public QWindow
{
Q_OBJECT
public:
    CALDLMainWindow (QScreen *targetScreen = 0);
    ~CALDLMainWindow();
protected:
    void resizeEvent    (QResizeEvent *value);
    void exposeEvent    (QExposeEvent *value);
    void keyPressEvent  (QKeyEvent    *value);

    void mousePressEvent(QMouseEvent  *value);
    void mouseMoveEvent (QMouseEvent  *value);
    void wheelEvent     (QWheelEvent  *value);

protected:
    void paint(void);
private:
    QOpenGLContext *_context;
    QMatrix4x4      _mProj;
    QPoint           _mouseLastPos;
    QCamera3D        _camera[2];

    int             _mode;

    QVector3D        _v[10];
    QRay3D           _r[10];   
    QPlane3D         _p[10];   
    QLine3D          _l[10];   

    QVector<QVector3D> _sp;

    QPaintedSphere3D _sph;
    QPaintedPlane3D  _pp;

    QVector<QPaintedPrimitive3D*> _list;

};


