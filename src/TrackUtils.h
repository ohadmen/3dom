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

    void privDrawCircle(QMatrix4x4 mvp, const QVector4D& col, float lw)
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
    void privDrawCircleBlur(const QMatrix4x4& mvp, QVector4D col,float lw)
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


        Qmvp sphereMVP; 
        sphereMVP.setR(mvp.getR());
        sphereMVP.setP(mvp.getP());
        sphereMVP.setU(mvp.getU());
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

        float lw = active ? Params::trackBallLineWidthMoving() : Params::trackBallLineWidthStill();


        privDrawCircle(sphereMVP     ,colR,lw);
        privDrawCircle(sphereMVP*r90x,colG,lw);
        privDrawCircle(sphereMVP*r90y,colB,lw);

    }
    QVector3D hitViewPlane(const Qmvp& mvp_, const  QVector2D& p,bool view=true)
    {
        Qmvp mvp = getShpereMVP(mvp_);
        QPlane3D vp = mvp.getViewPlane();
        QLine3D ln = mvp.viewLineFromWindow(p);
        QVector3D hitpoint;
        bool ok = vp.intersection(ln, &hitpoint);
        if (!view)
        {
            hitpoint = mvp.getR().inverted()*hitpoint;
            hitpoint /= mvp_.getS();
        }
        return ok?hitpoint:QVector3D();
    }
    QVector3D hitSphere(const Qmvp& mvp_, const  QVector2D& p)
    {
        Qmvp mvp = getShpereMVP(mvp_);
        static const float rad2deg = 90.0 / std::acos(0.0);
        QVector3D sphereT = -mvp.getT();
        float shpereR = Params::trackBallRadius()*mvp.getS();

        QLine3D ln = mvp.viewLineFromWindow(p);
        QVector3D viewpoint = mvp.getViewPoint();

        QPlane3D vp = mvp.getViewPlane();

        QVector3D hits1, hits2;

        QSphere3D sphere(sphereT, shpereR);//trackball sphere
       

    

        QVector3D    hit;
        //try to hit hyperboloid
        if (sprivHitHyper(sphereT, shpereR, viewpoint, vp, ln, &hit))
        { }
        //try to hit sphere
        else if(sphere.intersection(ln, &hits1, &hits2))
            hit = (viewpoint - hits1).length() < (viewpoint - hits2).length() ? hits1 : hits2;
        //?not hitting any, take closest point
        else
            hit = ln.point(ln.projection(QVector3D(0, 0, 0)));
        return hit;

        
        return hit;
    
    }

    static bool sprivHitHyper(const QVector3D& center, float r, const QVector3D& viewpoint, const QPlane3D& viewplane, const QLine3D& viewLine, QVector3D* hitP)
    {

        /*
        x=0:0.001:1;
        r=0.4;
        vp=1;

        viewLineA = -.44;
        viewLineB = -viewLineA*vp;

        hyperFuncA=r^5/(vp^3*sqrt(1-r^2/vp));
        x0=r^2/vp;
        hyperFuncB=r*sqrt(1-r^2/vp^2)-hyperFuncA*vp/r^2;
        y1=sqrt(max(0,r^2-x.^2));
        y2=hyperFuncA./x+hyperFuncB;
        y3 = viewLineA*x+viewLineB;

        d = (viewLineB-hyperFuncB)^2+4*viewLineA*hyperFuncA;

        xx=(-(viewLineB-hyperFuncB)+sqrt(d))/(2*viewLineA);


        plot(x,y1,x,y2,x,y3,xx,viewLineA*xx+viewLineB,'ro');
        line([vp x0],[0 r*sqrt(1-r^2/vp.^2)],'color','r');
        set(gca,'ylim',[0 1]);
        grid minor;
        % axis equal
        */

        QVector3D hitOnViewplane;  //intersection view plane with point touched
        bool ResPl = viewplane.intersection(viewLine, &hitOnViewplane);
        if (!ResPl)
            return false;
        float vp = (viewpoint - center).length();

        float viewLineA = -(hitOnViewplane - center).length() / viewplane.distance();
        float viewLineB = -viewLineA*vp;
        float r2 = r*r;

        float x0 = r2 / vp;
        float hyperFuncA = r2*r2*r / (std::pow(vp,3) * std::sqrt(1 - x0));
        //hyperFuncA *= 2;//make slope less sharp
        float hyperFuncB = r*std::sqrt(1 - x0/vp ) - hyperFuncA/x0;
        float diffb = (viewLineB - hyperFuncB);
        float d = diffb*diffb + 4 * viewLineA*hyperFuncA;
        if (d < 0)
            return false;
        float xval = 0.5*(-diffb + std::sqrt(d)) / viewLineA;
        if (xval < 0)
            return false;
        if (xval > x0)
            return false;
        float yval = viewLineA*xval + viewLineB;
        QVector3D dirRadial = (hitOnViewplane - center).normalized();

        QVector3D dirView = -viewplane.normal();

        *hitP = center + dirRadial * yval + dirView * xval;
        return true;
    }
  

};

