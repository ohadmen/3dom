#pragma once
#include <QMatrix4x4>
#include <QtOpenGL/QGLFunctions>
class Similarity {
	QQuaternion rot;
	QVector3D tra;
	float sca;

	Similarity() {}
	Similarity(const QQuaternion &q) { SetRotate(q); }
	Similarity(const QVector3D &p) { SetTranslate(p); }
	Similarity(float s) { SetScale(s); }
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


	Similarity &SetIdentity()
	{
		rot = QQuaternion(1, 0, 0, 0);
		tra = QVector3D(0, 0, 0);
		sca = 1;
		return *this;
	}
	Similarity &SetScale(const float s)
	{
		SetIdentity();
		sca = s;
		return *this;
	}
	Similarity &SetTranslate(const QVector3D &t)
	{
		SetIdentity();
		tra = t;
		return *this;
	}
	///use radiants for angle.
	Similarity &SetRotate(float angle, const QVector3D & axis)
	{
		SetIdentity();
		rot.fromAxisAndAngle(axis, angle);
		return *this;
	}
	Similarity &SetRotate(const QQuaternion &q)
	{
		SetIdentity();
		rot = q;
		return *this;
	}

	QMatrix4x4 Matrix() const
	{
		QMatrix4x4 r;r.rotate(rot);
		QMatrix4x4 s;s.scale(sca);
		QMatrix4x4 t;t.translate(tra);
		return QMatrix4x4(s*r*t);  // trans * scale * rot;
	}
	QMatrix4x4 InverseMatrix() const
	{
		return Matrix().inverted();
	}
	void FromMatrix(const QMatrix4x4 &m)
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

		////////////////////
		QMatrix3x3 rrr;
		?????????????
		rot.fromRotationMatrix(t);

		Invert(t);
		tra = t * tra;
		tra /= sca;
	}
};