#pragma  once

#include <QtGui/QMatrix4x4>
#include <QtGui/qvector2d.h>
#include <QtGui/qquaternion.h>
#include <QtCore/qdebug.h>




class VPmat 
{
	QMatrix4x4 m_proj;
	QMatrix4x4 m_view;
	int m_w;
	int m_h;


	

public:
	VPmat();

    QMatrix4x4 getVPmatrix() const;
    void setWinSize(int w, int h);

    
	QVector3D xy2screen(const QPointF& xy) const;
	const std::pair<QVector3D,QVector3D> xy2ray(const QPointF& xy) const;
	
	const QMatrix4x4& getViewMatrix() const ;
	
    void setViewMatrix(const QMatrix4x4& m);

    static QMatrix4x4 rotate(float a, const QVector3D& v);
    static QMatrix4x4 scale(float       v);
    static QMatrix4x4 translate(const QVector3D& v);
};

