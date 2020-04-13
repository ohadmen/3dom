#pragma once
#include <QtGui/qevent.h>
#include "zview/backend/vp_mat.h"


class TrackStateAbs;
//----------------------state machine----------------------//
class TrackStateMachine : public QObject {
    Q_OBJECT
        TrackStateAbs* m_state;
        VPmat m_vpmat;
    
    
public:
    QVector3D pickClosestObject(const QPointF &xy) const;
    void setState(TrackStateAbs* newState);
    TrackStateMachine();

	void canvasUpdate() ;
    void setStatus(const std::string& str) ;
    void setTexture(int k);
    QMatrix4x4 getVPmatrix() const;
	QMatrix4x4 getViewMatrix() const;
	void 	   setViewMatrix(const QMatrix4x4& m);
    void 	   setWinSize(int w, int h);
	QVector3D  xy2screen(const QPointF& xy);
	


public slots:
    void input(QInputEvent* e);

 signals:
     void       signal_canvasUpdate();
     void       signal_setStatus(const QString& str);
     void       signal_setTexture(int txt);






};


