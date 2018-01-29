#pragma once
#include <QMatrix4x4>
#include <QtOpenGL/QGLFunctions>
class Similarity {
	QQuaternion rot;
	QVector3D tra;
	float sca;

	static QQuaternion privM2Q(const QMatrix4x4& m) {

		QQuaternion q;
		if (m(0, 0) + m(1, 1) + m(2, 2) > 0.0f) {
			float t = m(0, 0) + m(1, 1) + m(2, 2) + 1.0f;
			float s = 0.5f / std::sqrt(t);
			q.setScalar(s * t);
			q.setZ((m(1, 0) - m(0, 1)) * s);
			q.setY((m(0, 2) - m(2, 0)) * s);
			q.setX((m(2, 1) - m(1, 2)) * s);
		}
		else if (m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2)) {
			float t = m(0, 0) - m(1, 1) - m(2, 2) + 1.0f;
			float s = 0.5f / std::sqrt(t);
			q.setX(s * t);
			q.setY((m(1, 0) + m(0, 1)) * s);
			q.setZ((m(0, 2) + m(2, 0)) * s);
			q.setScalar((m(2, 1) - m(1, 2)) * s);
		}
		else if (m(1, 1) > m(2, 2)) {
			float t = -m(0, 0) + m(1, 1) - m(2, 2) + 1.0f;
			float s = 0.5f / std::sqrt(t);
			q.setY(s * t);
			q.setX((m(1, 0) + m(0, 1)) * s);
			q.setScalar((m(0, 2) - m(2, 0)) * s);
			q.setZ((m(2, 1) + m(1, 2)) * s);
		}
		else {
			float t = -m(0, 0) - m(1, 1) + m(2, 2) + 1.0f;
			float s = 0.5f / std::sqrt(t);
			q.setZ(s * t);
			q.setScalar((m(1, 0) - m(0, 1)) * s);
			q.setX((m(0, 2) + m(2, 0)) * s);
			q.setY((m(2, 1) + m(1, 2)) * s);
		}
		return q;
	}
	

public:

	const float&       scale()     const { return sca; }
	const QVector3D&   translate() const { return tra; }
	const QQuaternion& rotation()  const { return rot; }

	 float&       scale()     { return sca; }
	 QVector3D&   translate() { return tra; }
	 QQuaternion& rotation()  { return rot; }

	Similarity() {}
	Similarity(const QQuaternion &q) { setRotate(q); }
	Similarity(const QVector3D &p) { SetTranslate(p); }
	Similarity(float s) { setScale(s); }
	Similarity(float alpha, float beta, float gamma)
	{
		rot.fromEulerAngles(alpha, beta, gamma);
		tra = QVector3D(0, 0, 0);
		sca = 1;
	}

	Similarity operator*(const Similarity &a) const
	{
		//Similarity r(*this);
		//r *= a;
		Similarity r;
		r.rot = rot * a.rot;
		r.sca = sca * a.sca;
		r.tra = (rot.rotatedVector(a.tra)) * sca + tra;
		return r;
	}
	Similarity &operator*=(const Similarity &a)
	{
		rot = rot * a.rot;
		sca = sca * a.sca;
		tra = (rot.rotatedVector(a.tra)) * sca + tra;
		return *this;
	}
	//QVector3D operator*(const QVector3D &p) const;


	Similarity& setIdentity()
	{
		rot = QQuaternion(1, 0, 0, 0);
		tra = QVector3D(0, 0, 0);
		sca = 1;
		return *this;
	}
	Similarity &setScale(const float s)
	{
		setIdentity();
		sca = s;
		return *this;
	}
	Similarity &SetTranslate(const QVector3D &t)
	{
		setIdentity();
		tra = t;
		return *this;
	}
	///use radiants for angle.
	Similarity &setRotate(float angle, const QVector3D & axis)
	{
		setIdentity();
		rot.fromAxisAndAngle(axis, angle);
		return *this;
	}
	Similarity& setRotate(const QQuaternion &q)
	{
		setIdentity();
		rot = q;
		return *this;
	}

	QMatrix4x4 matrix() const
	{
		QMatrix4x4 r;r.rotate(rot);
		QMatrix4x4 s;s.scale(sca);
		QMatrix4x4 t;t.translate(tra);
		return QMatrix4x4(s*r*t);  // trans * scale * rot;
	}
	QMatrix4x4 inverted() const
	{
		return matrix().inverted();
	}
	void fromMatrix(const QMatrix4x4 &m)
	{
		//Computes a t*s*r decomposition
		float det = m.determinant();
		//assert(det > 0);
		sca = pow(det, (1.f / 3.0f));
		QMatrix4x4 tt;tt.scale(1 / sca, 1 / sca, 1 / sca);
		QMatrix4x4 t = m*tt;
		tra[0] = t(0, 3); t(0,3) = 0.0;
		tra[1] = t(1, 3); t(1,3) = 0.0;
		tra[2] = t(2, 3); t(2,3) = 0.0;

		rot = privM2Q(t);
	
		tra = t.inverted() * tra;
		tra /= sca;
	}
};