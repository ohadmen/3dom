#pragma  once
#include <QMatrix4x4>
#include <qquaternion.h>
#include "Q3d/QPlane3D.h"
#include "Params.h"

class Qmvp
{
    class RigidT
    {
        QQuaternion m_r;
        float        m_s;
        QVector3D   m_t;
    public:
        static QMatrix4x4 spriv2mat(QQuaternion v) { QMatrix4x4 m; m.rotate(v); return std::move(m); }
        static QMatrix4x4 spriv2mat(float       v) { QMatrix4x4 m; m.scale(v); return std::move(m); }
        static QMatrix4x4 spriv2mat(QVector3D   v) { QMatrix4x4 m; m.translate(v); return std::move(m); }

        QMatrix4x4 mat()const { return spriv2mat(m_t)*spriv2mat(m_s)*spriv2mat(m_r); }
        QMatrix4x4 matI()const { return spriv2mat(m_r.inverted())*spriv2mat(1 / m_s)*spriv2mat(-m_t); }
        void reset()
        {
            m_r = QQuaternion(0, QVector3D(1, 0, 0));
            m_t = QVector3D(0, 0, 0);
            m_s = 1.0f;
        }
        void applyT(const QVector3D& t                )  { m_t += t; }
        void applyR(const QVector3D& axis, float angle)     { m_r = QQuaternion::fromAxisAndAngle(axis, angle) * m_r;    }
        void applyS(float s) { m_s *= s; }

        QQuaternion getR()const { return m_r; }
        float        getS()const { return m_s; }
        QVector3D   getT()const { return m_t; }

        void setR(const QQuaternion& r) { m_r = r; }
        void setR(const QVector3D& axis, float angle) { m_r = QQuaternion::fromAxisAndAngle(axis, angle); }
        void setS(float              s) { m_s = s; }
        void setT(const QVector3D&   t) { m_t = t; }

    };
    //QQuaternion m_modelR;
    //QVector3D   m_modelT;
    //float        m_modelS;

    RigidT m_view;
    RigidT m_model;

    //QQuaternion m_viewR;
    //float        m_viewS;
    //QVector3D   m_viewT;


    QMatrix4x4 m_proj;
    QVector2D m_uv2pix;//[w h]/2



public:
    Qmvp() {
        reset(1, 1);
        
        m_uv2pix=QVector2D(1,1);
        
    }

    void reset(int w, int h)
    {
        m_view.reset();
        m_model.reset();
        setWinSize(w,h);

    }

    void applyT(const QVector3D& t                ,bool view=true)  { view? m_view.applyT(t) : m_model.applyT(t); }
    void applyR(const QVector3D& axis, float angle,bool view=true)    { view? m_view.applyR(axis,angle) : m_model.applyR(axis, angle); }
    void applyS(float s                           ,bool view=true)  { view? m_view.applyS(s) : m_model.applyS(s); }


    QQuaternion  getR(bool view=true)const {return view? m_view.getR():m_model.getR();}
    float        getS(bool view=true)const {return view? m_view.getS():m_model.getS();}
    QVector3D    getT(bool view=true)const {return view? m_view.getT():m_model.getT();}
    QMatrix4x4   getP()const { return m_proj; }
    QVector2D    getU()const { return m_uv2pix; }

    void setT(const QVector3D& t, bool view = true) { view ? m_view.setT(t) : m_model.setT(t); }
    void setR(const QVector3D& axis, float angle, bool view = true) { view ? m_view.setR(axis, angle) : m_model.setR(axis, angle); }
    void setR(const QQuaternion q, bool view = true) { view ? m_view.setR(q) : m_model.setR(q); }
    void setS(float s, bool view = true) { view ? m_view.setS(s) : m_model.setS(s); }
    void setP(const QMatrix4x4& p) { m_proj = p; }
    void setU(const QVector2D& u) { m_uv2pix = u; }
    

    void recalcProjMat()
    {
        float ar = float(m_uv2pix[0]) / float(m_uv2pix[1] ? m_uv2pix[1] : 1);
        m_proj.setToIdentity();
        m_proj.perspective(Params::camFOV() / ar, ar, Params::camZnear(), Params::camZfar());
    }

    void setWinSize(int w, int h)
    {
        
        m_uv2pix = QVector2D(w , h ) / 2.0;
        recalcProjMat();
        
    }
    float getAspectRatio() const 
    {
        return m_proj(1, 1) / m_proj(0, 0);
    }
    QMatrix4x4 getMat() const
    {
            //return m_proj*s*r*t;  // trans * scale * rot;
            return m_proj*m_view.mat()*m_model.mat();  // trans * scale * rot;
    }
    
    QMatrix4x4 getMatInv() const
    {
        //return m_proj*s*r*t;  // trans * scale * rot;
        return m_model.matI()*m_view.matI()*m_proj.inverted();  // trans * scale * rot;
    }


   
    QVector3D getViewPoint() const {         return m_model.matI()*QVector3D(0, 0,  0);    }
    QPlane3D  getViewPlane() const { return QPlane3D(getViewPoint()- m_view.getT(), 1); }

    QPlane3D viewPlaneFromModel(const QVector3D& p)
    {
    
        QVector3D n = m_view.matI() * QVector3D(0, 0, 0);
        return QPlane3D(n, p);
    }

    QRay3D viewRayFromModel(const QVector3D &p)
    {
    
        QVector3D vp = getViewPoint();
        return QRay3D(vp, (p - vp));
    
    }

     
    QRay3D viewRayFromWindow(const QVector2D &p)
    {
        QVector3D vp = getViewPoint();
        QVector3D pp = unProject(p);
        return QRay3D( vp, (pp - vp));
    
    }

    QVector2D project(const QVector3D &xyz) const {
        QVector3D uv = getMat() * xyz; //[-1 1]
        QVector2D uv_(uv[0], uv[1]);
        QVector2D pix = uv2pix(uv_);
        return  pix;
    }

    QVector2D uv2pix( QVector2D uv) const {
        uv[1] *= -1;//inverse Y
        QVector2D pix = uv*m_uv2pix + m_uv2pix;
        return pix;
    }

    QVector2D pix2uv(const QVector2D &pix) const {
        QVector2D uv = (pix - m_uv2pix) / m_uv2pix;
        uv[1] *= -1;//inverse Y
        return uv;
    }

    QVector3D unProject(const QVector2D &pix) const {
        QVector2D uv = pix2uv(pix);


        QVector3D xyz = getP().inverted() * QVector3D(uv, -1);

        //xyz /= -xyz[2];
        return xyz;
    }


    QLine3D viewLineFromModel(const QVector3D &p) const
    {

        QVector3D vp = getViewPoint();
        return QLine3D(vp, p);

    }

    QLine3D cameraRay(const QVector2D& p)const
    {

        QVector3D vp =m_model.matI()*  m_view.matI()*QVector3D(0,0,0);
        QVector3D pp =        m_model.matI()*m_view.matI()* unProject(p);
        
        return QLine3D(vp, vp+(pp-vp)*1000);
    }
    
    QLine3D viewLineFromWindow(const QVector2D& p) const
    {
        QVector3D vp = getViewPoint();
        QVector3D pp = unProject(p);
    
       
        QLine3D line(vp, pp);
        
        return line;
    }

};

//Qmvp operator*(const Qmvp& a, const Qmvp& b)
//{
//    Qmvp c;
//    c.m_viewR  = a.m_viewR  * b.m_viewR    ;
//    c.m_viewT  = a.m_viewT  * b.m_viewT    ;
//    c.m_viewS  = a.m_viewS  * b.m_viewS    ;
//    c.m_proj   = a.m_proj   * b.m_proj    ;
//    c.m_uv2pix = a.m_uv2pix * b.m_uv2pix;
//}

