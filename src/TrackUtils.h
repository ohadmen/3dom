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
		static const int nBlurSteps = 20;
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

	Qmvp  getShpereMVP(const Qmvp& mvp)
	{
		static const float deg2rad = std::acos(0.0f) / 90.0f;
		float tanfovH = std::tan(Params::camFOV() / 2 * deg2rad);


		Qmvp sphereMVP=mvp;
		sphereMVP.setT(QVector3D(0, 0, -1));
		sphereMVP.setS(tanfovH);
	
		return sphereMVP;
	}
	void drawSphereIcon(const Qmvp& mvp, bool active)
	{
		//static const float deg2rad = std::acos(0.0f) / 90.0f;
		//float tanfovH = std::tan(Params::camFOV() / 2 * deg2rad);
		//
		//QMatrix4x4 fixedView;
		////fixedView.scale( 1.0, mvp.getAspectRatio(), 1.0);
		////fixedView.ortho(1, -1, 1, -1, 0, 2);
		//fixedView.translate(0, 0, -1);
		//fixedView.scale(tanfovH);
		//
		//
		//QMatrix4x4 sphereMVP = mvp.getP()*fixedView*mvp.getR();
		QMatrix4x4 sphereMVP = getShpereMVP(mvp).getMat();
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
	QVector3D hitViewPlane(const Qmvp& mvp_, const  QVector2D& p)
	{
		Qmvp mvp = getShpereMVP(mvp_);
		QPlane3D vp = mvp.getViewPlane();
		QLine3D ln = mvp.viewLineFromWindow(p);
		QVector3D hitpoint;
		bool ok = vp.intersection(ln, &hitpoint);

		return ok?hitpoint:QVector3D();
	}
	QVector3D hitSphere(const Qmvp& mvp_,const  QVector2D& p)
	{
		Qmvp mvp = getShpereMVP(mvp_);
		static const float rad2deg = 90.0 / std::acos(0.0);
		QVector3D sphereT = mvp.getT().row(2).toVector3D();
		float shpereR = Params::trackBallRadius()*mvp.getS()(0,0);
		
		QLine3D ln = mvp.viewLineFromWindow(p);
		QVector3D viewpoint = mvp.getViewPoint();

		QPlane3D vp = mvp.getViewPlane();
		
		QVector3D hits1, hits2;
		
		QSphere3D sphere(sphereT, shpereR);//trackball sphere
		bool resSp = sphere.intersection(ln, &hits1, &hits2);
		
		QVector3D hs = (viewpoint - hits1).length() < (viewpoint - hits2).length() ? hits1 : hits2;
		

		QVector3D	hitHyper;
		bool 	resHp = sprivHitHyper(sphereT, shpereR,viewpoint, vp, ln, &hitHyper);

		// four cases

		// 1) Degenerate line tangent to both sphere and hyperboloid!
		QVector3D hit;
		if (!resSp && !resHp)
		 hit = ln.point(ln.projection(QVector3D(0, 0, 0))); 
		else if (resSp && !resHp)
			hit = hs;           // 2) line cross only the sphere
		else if (!resSp && resHp)
			hit = hitHyper;            // 3) line cross only the hyperboloid
		else
		{
			// 4) line cross both sphere and hyperboloid: choose according angle.
			float angleDeg = std::acos(QVector3D::dotProduct(viewpoint.normalized(),hs.normalized()))*rad2deg;
			hit = angleDeg < 45 ? hs : hitHyper;
		}
		return hit;
	
	}
	static bool sprivHitHyper(const QVector3D& center,float r, const QVector3D& viewpoint, const QPlane3D& viewplane, const QLine3D& viewLine, QVector3D* hitP)
	{
		QVector3D hitOnViewplane;  //intersection view plane with point touched
		bool ResPl = viewplane.intersection(viewLine, &hitOnViewplane);
		if (!ResPl)
			return false;
		float hitplaney = (hitOnViewplane- center).length();
		float viewpointx = (viewpoint- center).length();

		float a = hitplaney / viewpointx;
		float b = -hitplaney;
		float c = r*r / 2.0f;
		float delta = b * b - 4 * a * c;
		if (delta < 0)
			return false;

		float	xval = (-b - sqrt(delta)) / (2.0f * a);
		float 	yval = c / xval;            //  alternatively it also could be the other part of the equation yval=-(hitplaney/viewpointx)*xval+hitplaney;
		
		// Computing the result in 3d space;
		QVector3D dirRadial = (hitOnViewplane- center).normalized();

		QVector3D dirView = -viewplane.normal();
		
		*hitP = center+dirRadial * yval + dirView * xval;
		return true;
	}


};

