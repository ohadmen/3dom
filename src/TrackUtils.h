#pragma once
#include <vector>
#include <QOpenGLFunctions>
#include "Shader.h"
#include "Params.h"
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
		static const float radius = 1;
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

	
	void drawSphereIcon(const QMatrix4x4& mvp, bool active, bool planeshandle = false)
	{
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


		privDrawCircle(mvp     ,colR,lw);
		privDrawCircle(mvp*r90x,colG,lw);
		privDrawCircle(mvp*r90y,colB,lw);

	}


};

