#pragma once

#include <QtGui>
#include "QGeometry3D.h"
#include "QPrimitive3D.h"
#include "QPlane3D.h"
#include "QLine3D.h"
#include "QSphere3D.h"

class QPainter3D
{
public:
    QPainter3D();
   ~QPainter3D();

public:
    QColor color    () const {return _color;}
    float  pointSize() const {return _pointSize;}
    float  lineWidth() const {return _lineWidth;}

    void setColor    (const QColor &value){_color = value;}
    void setPointSize(float value)        {_pointSize = value;}
    void setLineWidth(float value)        {_lineWidth = value;}
public:
    void paint(const QVector3D &v, bool complite = true);
    void paint(const QVector3D &v1, const QVector3D &v2, bool complite = true);
    void paint(const QPlane3D &value, float size, float cellSize);


    void paint(const QLine3D &value, bool complite = true);

protected:
    QColor  _color;
    float   _pointSize;
    float   _lineWidth;
};

//*************************************************************************************************

class QPaintedSphere3D
{
public:
    QPaintedSphere3D();
    QPaintedSphere3D(const QVector3D &center, float radius);
    QPaintedSphere3D(const QVector3D &center, float radius, int rings, int slices);
    virtual ~QPaintedSphere3D(){}
public:
    QVector3D center   () const {return _d->center();}
    float     radius   () const {return _d->radius();}
    void      setCenter(const QVector3D &value);
    void      setRadius(float value);

    QColor   facesColor      () const {return _facesColor;}
    QColor   verticesColor   () const {return _verticesColor;}
    void     setFacesColor   (const QColor &value) {_facesColor = value;}
    void     setVerticesColor(const QColor &value) {_verticesColor = value;}

public:
    void paint();

public:
    inline QSphere3D  operator  *() const {return *_d;}
    inline QSphere3D* operator ->() const {return _d.data();}

protected:
    void updateMatrix  ();
    void createVertices(int rings, int slices);
protected:
    QSharedPointer<QSphere3D> _d;
    QVector<QVector3D>        _vertices;
    QVector<quint32>          _indexes;
    QMatrix4x4                _m; 
    int                       _rings;
    int                       _slices;

    QColor                    _facesColor;
    QColor                    _verticesColor;
};

//*************************************************************************************************

class QPaintedPrimitive3D
{
public:
    QPaintedPrimitive3D()
        : _facesColor(255, 255, 255, 255), _verticesColor(255, 255, 255, 255){}

    virtual ~QPaintedPrimitive3D(){}
    virtual void paint(){};
public:
    QColor   facesColor      () const {return _facesColor;}
    QColor   verticesColor   () const {return _verticesColor;}
    void     setFacesColor   (const QColor &value) {_facesColor = value;}
    void     setVerticesColor(const QColor &value) {_verticesColor = value;}
protected:
    QVector<QVector3D>  _vertices;
    QVector<quint32>    _indexes;
    QMatrix4x4          _m; 
    QColor              _facesColor;
    QColor              _verticesColor;
};

//*************************************************************************************************

class QPaintedPlane3D : public QPlane3D, public QPaintedPrimitive3D 
{
public:
    QPaintedPlane3D() 
        : QPlane3D(), QPaintedPrimitive3D(),
          _width(1), _height(1), _resolution(10, 10)
    {
    };

    QPaintedPlane3D(const QVector3D &normal, float distance)
        : QPlane3D(normal, distance), QPaintedPrimitive3D(),
          _width(4), _height(4), _resolution(40, 40)
    {
        createVertices();
        updateMatrix();
    }

    QPaintedPlane3D(const QVector3D &p0, const QVector3D &p1, const QVector3D &p2)
        : QPlane3D(p0, p1, p2), QPaintedPrimitive3D(),
          _width(4), _height(4), _resolution(40, 40)
    {
        createVertices();
        updateMatrix();
    }

    virtual ~QPaintedPlane3D(){}
public:


    void paint()
    {
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity();

        glPointSize(10);
        glBegin(GL_POINTS);
//            glVertex3f(0.0f, 0.0f, 0.0f); 

            glVertex3f(direction().x(), direction().y(), direction().z()); 
            glVertex3f(origin().x(), origin().y(), origin().z()); 

        glEnd();


        glBegin(GL_LINES);
            glVertex3f(0.0f, 0.0f, 0.0f); 
            glVertex3f(direction().x(), direction().y(), direction().z()); 
        glEnd();


        glLineWidth(3);

        glBegin(GL_LINES);
            glVertex3f(0.0f, 0.0f, 0.0f); 
            glVertex3f(_normal.x(), _normal.y(), _normal.z()); 
        glEnd();



        glLoadMatrixf(_m.constData());

        glPointSize(10);
//        glBegin(GL_POINTS);
//            glVertex3f(0.0f, 0.0f, 0.0f); 
//        glEnd();


        glLineWidth(1);
        glPointSize(3);

        glBegin(GL_TRIANGLES);
            glColor4f(_facesColor.redF(), _facesColor.greenF(), _facesColor.blueF(), _facesColor.alphaF());
            for(auto i : _indexes)glVertex3f(_vertices[i].x(), _vertices[i].y(), _vertices[i].z()); 
        glEnd();

        glEnd();
            glBegin(GL_POINTS);
            glColor4f(_verticesColor.redF(), _verticesColor.greenF(), _verticesColor.blueF(), _verticesColor.alphaF());
            for(auto i : _indexes)glVertex3f(_vertices[i].x(), _vertices[i].y(), _vertices[i].z()); 
        glEnd();

        glLoadIdentity();
    }
protected:
    void createVertices()
    {
        const int nVerts = _resolution.width() * _resolution.height();
        const float x0 = -_width / 2.0f;
        const float z0 = -_height / 2.0f;
        const float dx = _width / (_resolution.width() - 1);
        const float dz = _height / (_resolution.height() - 1);
        const float du = 1.0 / (_resolution.width() - 1);
        const float dv = 1.0 / (_resolution.height() - 1);
        for(int j = 0; j < _resolution.height(); ++j) 
        {
            const float z = z0 + static_cast<float>(j) * dz;
            const float v = static_cast<float>(j) * dv;

            for (int i = 0; i < _resolution.width(); ++i) 
            {
                const float x = x0 + static_cast<float>(i) * dx;
                const float u = static_cast<float>(i) * du;
                _vertices << QVector3D(x, 0.0f, z);// position

#if 0            
                float vtu = u;// texture coordinates
                float vtv = v;
                float vnx = 0.0f; //normals
                float vny = 1.0f;
                float vnz = 0.0f;
                float tn1 = 1.0f;// tangent
                float tn2  0.0f;
                float tn3  0.0f;
                float tn4  1.0f;
#endif
            }
        }

        const int faces   = 2 * (_resolution.width() - 1) * (_resolution.height() - 1);
        const int indices = 3 * faces;
        for(int j = 0; j < _resolution.height() - 1; ++j)
        {
            const int rowStartIndex = j * _resolution.width();
            const int nextRowStartIndex = (j + 1) * _resolution.width();
            for (int i = 0; i < _resolution.width() - 1; ++i) 
            {
                _indexes << rowStartIndex + i;
                _indexes << nextRowStartIndex + i;
                _indexes << rowStartIndex + i + 1;
                _indexes << nextRowStartIndex + i;
                _indexes << nextRowStartIndex + i + 1;
                _indexes << rowStartIndex + i + 1;
            }
        }
    }

    void updateMatrix()
    {
        _m.setToIdentity();
        _m.translate(origin());
        _m.rotate(QQuaternion::fromDirection(direction(), _normal));
    }

    void update()
    {
        updateMatrix();
    }

protected:
    float _width;
    float _height;
    QSize _resolution;
};
