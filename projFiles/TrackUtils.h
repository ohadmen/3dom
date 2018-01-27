#pragma once
#include <QMatrix4x4>
#include <QtOpenGL/QGLFunctions>
#include <Q3d\QLine3D.h>
#include <Q3d\QRay3D.h>
#include <Q3d\QPlane3D.h>
#include <Q3d\QSphere3D.h>

#include <QtOpenGL>

#include "Trackball.h"
class TrackUtils {
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
	static void drawPlaneHandle()
	{
		float r = 1.0;
		float dr = r / 10.0f;

		glBegin(GL_LINE_STRIP);
		glVertex3f(+r + dr, +r, 0.0);
		glVertex3f(+r, +r + dr, 0.0);
		glVertex3f(+r - dr, +r, 0.0);
		glVertex3f(+r, +r - dr, 0.0);
		glVertex3f(+r + dr, +r, 0.0);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex3f(-r + dr, -r, 0.0);
		glVertex3f(-r, -r + dr, 0.0);
		glVertex3f(-r - dr, -r, 0.0);
		glVertex3f(-r, -r - dr, 0.0);
		glVertex3f(-r + dr, -r, 0.0);
		glEnd();
	}

	static void drawCircle(bool planehandle = true)
	{
		int nside = DrawingHint::circleStep();
		static const double pi2 = std::acos(0) * 4;
		glBegin(GL_LINE_LOOP);
		for (double i = 0; i < nside; i++) {
			glNormal3d(cos(i * pi2 / nside), sin(i * pi2 / nside), 0.0);
			glVertex3d(cos(i * pi2 / nside), sin(i * pi2 / nside), 0.0);
		}
		glEnd();
		if (planehandle)
			drawPlaneHandle();
	}

	static void DrawSphereIcon(const Trackball& tb, bool active, bool planeshandle = false)
	{
		glPushAttrib(GL_TRANSFORM_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glDepthMask(GL_FALSE);

		auto track = tb.getTrack();

		QVector3D center = tb.center() + track.inverted()*QVector3D(0, 0, 0);

		glTranslatef(center.x(), center.y(), center.z());
		glScalef(tb.radius() / track.m11, tb.radius() / track.m22, tb.radius() / track.m33);


		float amb[4] = { .35f, .35f, .35f, 1.0f };
		float col[4] = { .5f, .5f, .8f, 1.0f };
		glEnable(GL_LINE_SMOOTH);
		if (active)
			glLineWidth(DrawingHint::lineWidthMoving());
		else
			glLineWidth(DrawingHint::lineWidthStill());
		glDisable(GL_COLOR_MATERIAL); // has to be disabled, it is used by wrapper to draw meshes, and prevent direct material setting, used here

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor3us(DrawingHint::color().red(), DrawingHint::color().green(), DrawingHint::color().blue());
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, amb);

		col[0] = .40f; col[1] = .40f; col[2] = .85f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
		drawCircle(planeshandle);

		glRotatef(90, 1, 0, 0);
		col[0] = .40f; col[1] = .85f; col[2] = .40f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
		drawCircle(planeshandle);

		glRotatef(90, 0, 1, 0);
		col[0] = .85f; col[1] = .40f; col[2] = .40f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
		drawCircle(planeshandle);

		glPopMatrix();
		glPopAttrib();
	}

	static QPlane3D getViewPlane(const QVector3D& viewPoint, const QVector3D& center)
	{
		
	
		QVector3D plnorm = viewPoint - center;
		plnorm.normalize();
		QPlane3D pl(plnorm, QVector3D::dotProduct(plnorm,center));
		
		return pl;
	}
	static bool hitHyper(const QVector3D& center, float radius, const QVector3D& viewpoint, const QPlane3D& viewplane, const QVector3D& hitOnViewplane, QVector3D* hitP)
	{
		float hitplaney = (center- hitOnViewplane).length();
		float viewpointx = (center- viewpoint).length();

		float a = hitplaney / viewpointx;
		float b = -hitplaney;
		float c = radius * radius / 2.0f;
		float delta = b * b - 4 * a * c;
		float x1, x2, xval, yval;

		if (delta > 0) {
			x1 = (-b - sqrt(delta)) / (2.0f * a);
			x2 = (-b + sqrt(delta)) / (2.0f * a);

			xval = x1;                  // always take the minimum value solution
			yval = c / xval;            //  alternatively it also could be the other part of the equation yval=-(hitplaney/viewpointx)*xval+hitplaney;
		}
		else {
			return false;
		}
		// Computing the result in 3d space;
		QVector3D dirRadial = hitOnViewplane - center;
		dirRadial.normalize();
		QVector3D dirView = viewplane.direction();
		dirView.normalize();
		*hitP = center + dirRadial * yval + dirView * xval;
		return true;
	}
	/**
	* @brief HitHyperOrtho
	* @param center
	* @param radius
	* @param viewpoint
	* @param viewplane
	* @param hitOnViewplane
	* @param hit
	* @return true if the hit is valid
	*
	* Specialized version of the HitHyper();
	* in the simple ortho case, the hit point is just the value of
	* y = 1/x * (r^2 /2 ) on the hitOnViewPlane
	*/
	static bool hitHyperOrtho(const QVector3D& center, float radius, const QVector3D& viewpoint, const QPlane3D& viewplane, const QVector3D& hitOnViewplane, QVector3D* hitP)
	{
		float xval = (center- hitOnViewplane).length();

		float yval = (1.0 / xval) *  radius * radius / 2.0f;

		// Computing the result in 3d space;
		QVector3D dirRadial = hitOnViewplane - center;
		dirRadial.normalize();
		QVector3D dirView = viewplane.direction();
		dirView.normalize();
		*hitP = center + dirRadial * xval + dirView * yval;

		return true;
	}
	static float vvAngle(const QVector3D& v1, const QVector3D& v2)
	{
		static const float rad2deg = 90.0f / std::acos(0);
		float w = v1.length()*v2.length();
		if (w == 0) return -1;
		float t = QVector3D::dotProduct(v1,v2) / w;
		if (t>1) t = 1;
		else if (t<-1) t = -1;
		return std::acos(t)*rad2deg;
	}
	static QVector3D hitSphere(const Trackball& tb, const QVector3D & p)
	{
		QVector3D center = tb.center();
		auto camera = tb.getCamera();
		
		QLine3D ln = camera.viewLineFromWindow(QVector3D(p[0], p[1], 0));
		QPlane3D vp = TrackUtils::getViewPlane(camera.viewPoint(), center);
		
			
	

		QSphere3D sphere(center, tb.radius());//trackball sphere
		QVector3D hitSphere1, hitSphere2;
		bool resSp = sphere.intersection(ln,&hitSphere1, &hitSphere2);

		QVector3D viewpoint = camera.viewPoint();
		QVector3D hitSphere;
		if (resSp) {
			if ((viewpoint-hitSphere1).lengthSquared() < (viewpoint, hitSphere2).lengthSquared())
				hitSphere = hitSphere1;
			else
				hitSphere = hitSphere2;
		}

		///*float dl= */ Distance(ln, center);
		bool resHp;
		QVector3D hitPlane;
		vp.intersection(ln, &hitPlane);
		QVector3D hitHyperPt;
		if (camera.isOrtho())
			resHp = hitHyperOrtho(center, tb.radius(), viewpoint, vp, hitPlane, &hitHyperPt);
		else
			resHp = hitHyper(center, tb.radius(), viewpoint, vp, hitPlane, &hitHyperPt);

		// four cases

		// 1) Degenerate line tangent to both sphere and hyperboloid!
		if ((!resSp && !resHp)) {
			QVector3D hit = ln.closest(center);
			//printf("closest point to line %f\n",Distance(hit,tb->center));
			return hit;
		}
		if ((resSp && !resHp))
			return hitSphere;           // 2) line cross only the sphere
		if ((!resSp && resHp))
			return hitHyperPt;            // 3) line cross only the hyperboloid

										// 4) line cross both sphere and hyperboloid: choose according angle.
		
		float angleDeg = vvAngle((viewpoint - center), (hitSphere - center));

		//  qDebug("Angle %f (%5.2f %5.2f %5.2f) (%5.2f %5.2f %5.2f)",angleDeg,hitSphere[0],hitSphere[1],hitSphere[2],hitHyper[0],hitHyper[1],hitHyper[2]);
		if (angleDeg < 45)
			return hitSphere;
		else
			return hitHyperPt;

		// Codice ORIGINALE PONCHIO
		//vp.SetOffset(vp.Offset() + Thr);

		//QVector3D hit;
		//bool res = Intersection<float>(vp, ln, hit);
		//float d = Distance(tb->center - vn.Direction()*Thr, hit);
		//if(d < Thr) {
		//  QVector3D hit2;
		//  Sphere3f sphere(tb->center, tb->radius);
		//  bool res = Intersection<float>(sphere, ln, hit, hit2);

		//  //find closest intersection to sphere
		//  float d = (hit - viewpoint).Norm();
		//  float d2 = (hit2 - viewpoint).Norm();
		//  if(d > d2) hit = hit2;
		//  hit -= tb->center;
		//} else {

	}
}; //end namespace trackutils
