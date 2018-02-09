#pragma once
#include <vector>
#include <QOpenGLFunctions>
#include "Shader.h"
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
	class DrawingHint
	{
	public:
		static int circleStep(int vin = -1)
		{
			static int vs = 64;
			if (vin != -1)
				vs = vin;
			return vs;
		}
		static int lineWidthStill(float vin = -1)
		{
			static float vs = 0.9f;
			if (vin != -1)
				vs = vin;
			return vs;
		}
		static int lineWidthMoving(float vin = -1)
		{
			static float vs = 1.8f;
			if (vin != -1)
				vs = vin;
			return vs;
		}
		static QColor color(QColor vin = -1)
		{
			static QColor vs = Qt::darkBlue;
			if (vin != -1)
				vs = vin;
			return vs;
		}

	};
public:
	TrackUtils()
	{
		
	}
	bool init()
	{
		if (!m_ballShader.init("vshader", "fshader"))
			return false;

		initializeOpenGLFunctions();
		static const int nPoints = DrawingHint::circleStep();
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

	void drawCircle(QMatrix4x4 mvp)
	{
		m_circBuff.bind();
		mvp.scale(1);
		m_ballShader.setMVP(mvp);
		QOpenGLShaderProgram* shader = m_ballShader.get();

		int vp = shader->attributeLocation("a_xyz");
		shader->enableAttributeArray(vp);
		shader->setAttributeBuffer(vp, GL_FLOAT, 0, 3, sizeof(Pt));


		int vc = shader->attributeLocation("a_rgb");
		shader->enableAttributeArray(vc);
		shader->setAttributeBuffer(vc, GL_FLOAT, sizeof(QVector3D), 3, sizeof(Pt));



		glLineWidth(3);
		glDrawArrays(GL_LINE_LOOP,0, m_circBuff.size() / sizeof(Pt));

		m_circBuff.release();

		// Clean up state machine
		shader->disableAttributeArray(vp);
		shader->disableAttributeArray(vc);

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


		//glPushAttrib(GL_TRANSFORM_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);


		static const float amb[4] = { .35f, .35f, .35f, 1.0f };
		static const float colR[4] = { .40f, .40f, .85f, 1.0f };
		static const float colG[4] = { .40f, .85f, .40f, 1.0f };
		static const float colB[4] = { .85f, .40f, .40f, 1.0f };
		//glEnable(GL_LINE_SMOOTH);
		if (active)
			glLineWidth(DrawingHint::lineWidthMoving());
		else
			glLineWidth(DrawingHint::lineWidthStill());
		//glDisable(GL_COLOR_MATERIAL); // has to be disabled, it is used by wrapper to draw meshes, and prevent direct material setting, used here

		//glEnable(GL_LIGHTING);
		//glEnable(GL_LIGHT0);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		////glColor3us(DrawingHint::color().red(), DrawingHint::color().green(), DrawingHint::color().blue());
		////glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, amb);
		//
		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colR);
		drawCircle(mvp);

		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colG);
		drawCircle(mvp*r90x);

		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colB);
		drawCircle(mvp*r90y);

	}


};

