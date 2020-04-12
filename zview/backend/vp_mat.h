#pragma  once
//#include <QObject>

#include <QtGui/QMatrix4x4>
#include <QtGui/qvector2d.h>
#include <QtGui/qquaternion.h>
#include <QtCore/qdebug.h>

//signelon, view projection matrix
#define vpmat VPmat::i()
class VPmat 
{
	QMatrix4x4 m_proj;
	int m_w;
	int m_h;
	QMatrix4x4 m_view;

public:
	VPmat();

    QMatrix4x4 operator()() const;
    void setWinSize(int w, int h);

    template<class T>
	QVector3D xy2screen(const T& xy)
	{
		QVector4D uv(xy.x() / m_w * 2 - 1, -(xy.y() / m_h * 2 - 1),1,1);
		QVector4D sv = m_proj.inverted() * uv;
		return QVector3D(sv);

	}
	const QMatrix4x4& getProjMatrix() const ;
	const QMatrix4x4& getViewMatrix() const ;
	
    void setViewMatrix(const QMatrix4x4& m);

    static QMatrix4x4 rotate(float a, const QVector3D& v);
    static QMatrix4x4 scale(float       v);
    static QMatrix4x4 translate(const QVector3D& v);
};

