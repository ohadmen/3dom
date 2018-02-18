#pragma once
#include <vector>
#include <QOpenGLFunctions>
#include "Shader.h"
#include "Params.h"
#include "Q3d/QSphere3D.h"
#include "Qmvp.h"
class TrackUtils: protected QOpenGLFunctions
{
	Shader m_ballShader;
	QOpenGLBuffer m_circBuff;

	struct Pt
	{
		QVector3D xyz;
		QVector3D rgb;
		Pt() :xyz(0, 0, 0), rgb(0, 0, 0) {}
		Pt(float x, float y, float z, float r, float g, float b) :xyz(x, y, z), rgb(r, g, b) {}
	};

	void privDrawCircle(QMatrix4x4 mvp, const QVector4D& col, int lw)
	{
		m_ballShader.bind();
		m_circBuff.bind();
		mvp.scale(1);


		QOpenGLShaderProgram* shader = m_ballShader.get();

		m_ballShader.setMVP(mvp);

		int vp = shader->attributeLocation("a_xyz");
		shader->enableAttributeArray(vp);
		shader->setAttributeBuffer(vp, GL_FLOAT, 0, 3, sizeof(Pt));

		shader->setUniformValue("u_col", col);
		
		

		glLineWidth(lw);
		glDrawArrays(GL_LINE_LOOP, 0, m_circBuff.size() / sizeof(Pt));

		m_circBuff.release();

		// Clean up state machine
		shader->disableAttributeArray(vp);



	}
	void privDrawCircleBlur(const QMatrix4x4& mvp, QVector4D col,int lw)
	{
		static const int nBlurSteps = 5;
		col[3] = 1 / float(nBlurSteps);
		for (int i = 0; i != nBlurSteps; ++i)
		{
			privDrawCircle(mvp, col, lw + i);
		}
	}
public:
	TrackUtils()
	{
		
	}
	bool init()
	{
	
	
		if (!m_ballShader.init("linev", "linef"))
			return false;

		
		static const int nPoints = Params::trackBallCircleStep();
		static const float twopi = std::acos(0) * 4;
		static const float radius = Params::trackBallRadius();
		std::vector<Pt> circPoints(nPoints);
		for (int i = 0; i != nPoints; ++i)
		{
			circPoints[i] = Pt(radius*std::cos(i * twopi / nPoints), radius*std::sin(i * twopi / nPoints), 0.0, 1, 0, 0);
		}
		m_circBuff.create();
				m_circBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_circBuff.bind();
		m_circBuff.allocate(circPoints.data(), int(circPoints.size() * sizeof(Pt)));
		m_circBuff.release();


		return true;
	}

	
	void drawSphereIcon(const Qmvp& mvp, bool active, bool planeshandle = false)
	{
		static const float deg2rad = std::acos(0.0f) / 90.0f;
		float tanfovH = std::tan(Params::camFOV() / 2 * deg2rad);
		
		QMatrix4x4 fixedView;
		//fixedView.scale( 1.0, mvp.getAspectRatio(), 1.0);
		//fixedView.ortho(1, -1, 1, -1, 0, 2);
		fixedView.translate(0, 0, -1);
		fixedView.scale(tanfovH);
		

		QMatrix4x4 sphereMVP = mvp.getP()*fixedView*mvp.getR();
		initializeOpenGLFunctions();
		static const QMatrix4x4 r90x
		(1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1);
		static const QMatrix4x4 r90y
		(0, 0, 1, 0,
			0, 1, 0, 0,
			1, 0, 0, 0,
			0, 0, 0, 1);


		static const QVector4D colR(.40f, .40f, .85f, 1.0f);
		static const QVector4D colG(.40f, .85f, .40f, 1.0f);
		static const QVector4D colB(.85f, .40f, .40f, 1.0f);

		int lw = active ? Params::trackBallLineWidthMoving() : Params::trackBallLineWidthStill();


		privDrawCircle(sphereMVP     ,colR,lw);
		privDrawCircle(sphereMVP*r90x,colG,lw);
		privDrawCircle(sphereMVP*r90y,colB,lw);

	}
	QVector3D hitSphere( Qmvp mvp,  QVector2D p)
	{
		return QVector3D();
		/*
		QLine3D ll = mvp.viewLineFromWindow(p);

		QLine3D ln = mvp.viewLineFromWindow(p);
		QPlane3D vp = mvp.getViewPlane();
		QVector3D hitPlane(0, 0, 0), //intersection view plane with point touched
			hitSphere(0, 0, 0),
			hitSphere1(0, 0, 0),
			hitSphere2(0, 0, 0),
			hitHyper(0, 0, 0);

		QSphere3D sphere(QVector3D(), Params::trackBallRadius());//trackball sphere
		bool resSp = IntersectionLineSphere < float >(sphere, ln, hitSphere1, hitSphere2);

		Point3f viewpoint = tb->camera.ViewPoint();
		if (resSp == true) {
			if (Distance(viewpoint, hitSphere1) < Distance(viewpoint, hitSphere2))
				hitSphere = hitSphere1;
			else
				hitSphere = hitSphere2;
		}

		/ *float dl= * / Distance(ln, center);
		bool resHp;
		IntersectionPlaneLine < float >(vp, ln, hitPlane);
		if (tb->camera.isOrtho)
			resHp = HitHyperOrtho(center, tb->radius, viewpoint, vp, hitPlane, hitHyper);
		else
			resHp = HitHyper(center, tb->radius, viewpoint, vp, hitPlane, hitHyper);

		// four cases

		// 1) Degenerate line tangent to both sphere and hyperboloid!
		if ((!resSp && !resHp)) {
			Point3f hit = ClosestPoint(ln, center);
			//printf("closest point to line %f\n",Distance(hit,tb->center));
			return hit;
		}
		if ((resSp && !resHp))
			return hitSphere;           // 2) line cross only the sphere
		if ((!resSp && resHp))
			return hitHyper;            // 3) line cross only the hyperboloid

										// 4) line cross both sphere and hyperboloid: choose according angle.
		float angleDeg = math::ToDeg(Angle((viewpoint - center), (hitSphere - center)));

		//  qDebug("Angle %f (%5.2f %5.2f %5.2f) (%5.2f %5.2f %5.2f)",angleDeg,hitSphere[0],hitSphere[1],hitSphere[2],hitHyper[0],hitHyper[1],hitHyper[2]);
		if (angleDeg < 45)
			return hitSphere;
		else
			return hitHyper;
*/

	
	}


};

