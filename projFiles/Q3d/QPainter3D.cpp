#include "QPainter3D.h"

QPainter3D::QPainter3D()
    :_color(255, 255, 255, 255), _pointSize(1.0f), _lineWidth(1.0f)
{}

QPainter3D::~QPainter3D()
{}

void QPainter3D::paint(const QVector3D &v, bool complite)
{
    glPointSize(_pointSize);
    glColor4f(_color.redF(), _color.greenF(), _color.blueF(), _color.alphaF());
    if(complite)glBegin(GL_POINTS);
    glVertex3f(v.x(), v.y(), v.z()); 
    if(complite)glEnd();
}

void QPainter3D::paint(const QVector3D &v1, const QVector3D &v2, bool complite)
{
    glLineWidth(_lineWidth);
    glColor4f(_color.redF(), _color.greenF(), _color.blueF(), _color.alphaF());
    if(complite)glBegin(GL_LINES);
    glVertex3f(v1.x(), v1.y(), v1.z()); 
    glVertex3f(v2.x(), v2.y(), v2.z()); 
    if(complite)glEnd();
}

void QPainter3D::paint(const QLine3D &value, bool complite)
{
    paint(value.p1(), complite);
    paint(value.p2(), complite);
    paint(value.p1(), value.p2(), complite);
}

void QPainter3D::paint(const QPlane3D &value, float size, float cellSize)
{
    int cnt = size / cellSize + 1;
    float k0 = size / (cnt - 1);
    QVector3D v0 = value.normal() * value.distance();
    QVector3D n0 = QVector3D(0.0f, 1.0f, 0.0f);
    QVector3D n1 = QVector3D::crossProduct(value.normal(), n0).normalized();
    QVector3D n2 = QVector3D::crossProduct(value.normal(), n1).normalized();
    QVector3D v1 = n1 * k0;
    QVector3D v2 = n2 * k0;
    n1 *= size / 2.0f;
    n2 *= size / 2.0f;
    QVector3D v_0_0 = v0 - n1 - n2;
    QVector3D v_0_1 = v0 + n1 - n2;
    QVector3D v_1_0 = v0 - n1 - n2;
    QVector3D v_1_1 = v0 - n1 + n2;

    QVector3D v3 = value.normal() * 2;


    glBegin(GL_LINES);

//    paint(QVector3D(), v0, false);
    paint(QVector3D(), v3, false);

    for(int i = 1; i < cnt + 1; i++)
    {
        paint(v_0_0, v_0_1, false);
        paint(v_1_0, v_1_1, false);
        v_0_0 += v2; v_0_1 += v2;
        v_1_0 += v1; v_1_1 += v1;
    }
    glEnd();


    float pointSize = _pointSize;
    _pointSize = 5;

    _color = QColor(0, 255, 0, 255);

//    paint(v0);
    paint(v3);
    _pointSize = pointSize;
}


//*************************************************************************************************

QPaintedSphere3D::QPaintedSphere3D() 
    : _d(new QSphere3D()), _rings(12), _slices(12), 
      _facesColor(255, 255, 255, 255), _verticesColor(255, 255, 255, 255)
{
    createVertices(_rings, _slices);
}

QPaintedSphere3D::QPaintedSphere3D (const QVector3D &center, float radius) 
    : _d(new QSphere3D(center, radius)), _rings(12), _slices(12),
      _facesColor(255, 255, 255, 255), _verticesColor(255, 255, 255, 255)
{
    createVertices(_rings, _slices);
    updateMatrix();
}

QPaintedSphere3D::QPaintedSphere3D(const QVector3D &center, float radius, int rings, int slices)
    : _d(new QSphere3D(center, radius)), _rings(rings), _slices(slices),
      _facesColor(255, 255, 255, 255), _verticesColor(255, 255, 255, 255)
{
    createVertices(_rings, _slices);
    updateMatrix();
}

void QPaintedSphere3D::setCenter(const QVector3D &value)
{
    if(!qFuzzyCompare(_d->center(), value))
    {
        _d->setCenter(value); 
        updateMatrix();
    }
}

void QPaintedSphere3D::setRadius(float value)
{
    if(!qFuzzyCompare(_d->radius(), value))
    {
        _d->setRadius(value); 
        updateMatrix();
    }
}

void QPaintedSphere3D::updateMatrix()
{
    _m.setToIdentity();
    _m.translate(_d->center());
    _m.scale(_d->radius());
}

void QPaintedSphere3D::createVertices(int rings, int slices)
{
    _indexes.clear();
    _vertices.clear();
    const float dTheta = (M_PI * 2) / static_cast<float>(slices);
    const float dPhi   = M_PI / static_cast<float>(rings);
    const float du     = 1.0f / static_cast<float>(slices);
    const float dv     = 1.0f / static_cast<float>(rings);
    for(int lat = 0; lat < rings + 1; ++lat)
    {
        const float phi = M_PI_2 - static_cast<float>(lat) * dPhi;
        const float cosPhi = qCos(phi);
        const float sinPhi = qSin(phi);
        const float v = 1.0f - static_cast<float>(lat) * dv;

        for(int lon = 0; lon < slices + 1; ++lon)
        {
            const float theta = static_cast<float>(lon) * dTheta;
            const float cosTheta = qCos(theta);
            const float sinTheta = qSin(theta);
            const float u = static_cast<float>(lon) * du;
#if 0
            float vx = _d->radius() * cosTheta * cosPhi;
            float vy = _d->radius() * sinPhi;
            float vz = _d->radius() * sinTheta * cosPhi;
#else

            float vx = cosTheta * cosPhi;
            float vy = sinPhi;
            float vz = sinTheta * cosPhi;
#endif
#if 0
            float vtu = u; //texture coord
            float vtv = v;
            float vnx = cosTheta * cosPhi; //normals
            float vny = sinPhi;
            float vnz = sinTheta * cosPhi;

            float tn1 = sinTheta; //tangents
            float tn2 = 0.0f;
            float tn3 = -cosTheta;
            float tn4 = 1.0f;
#endif
            _vertices << QVector3D(vx, vy, vz);
        }
    }
    const int faces   = (slices << 1) * (rings - 1);
    const int indices = faces * 3;
    int nextRingStartIndex = slices + 1;
    for(int j = 0; j < slices; ++j)
    {
        _indexes << nextRingStartIndex + j;
        _indexes << 0;
        _indexes << nextRingStartIndex + j + 1;
    }
    for(int i = 1; i < (rings - 1); ++i)
    {
        const int ringStartIndex = i * (slices + 1);
        const int nextRingStartIndex = (i + 1) * (slices + 1);
        for(int j = 0; j < slices; ++j)
        {
            _indexes << ringStartIndex + j;
            _indexes << ringStartIndex + j + 1;
            _indexes << nextRingStartIndex + j;
            _indexes << nextRingStartIndex + j;
            _indexes << ringStartIndex + j + 1;
            _indexes << nextRingStartIndex + j + 1;
        }
    }
    const int ringStartIndex = (rings - 1) * (slices + 1);
    nextRingStartIndex = rings * (slices + 1);
    for ( int j = 0; j < slices; ++j )
    {
        _indexes << ringStartIndex + j + 1;
        _indexes << nextRingStartIndex;
        _indexes << ringStartIndex + j;
    }
}

void QPaintedSphere3D::paint()
{
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(_m.constData());

    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(0.0f, 0.0f, 0.0f); 
    glEnd();

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

//*************************************************************************************************
