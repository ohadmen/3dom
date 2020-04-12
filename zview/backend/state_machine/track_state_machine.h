#pragma once
#include <QtGui/qevent.h>

class TrackStateAbs;
//----------------------state machine----------------------//
class TrackStateMachine : public QObject {
    Q_OBJECT
        TrackStateAbs* m_state;
public:
    void setState(TrackStateAbs* newState);
    TrackStateMachine();
public slots:
    void input(QInputEvent* e);

signals:
    void canvasUpdate();
    void setStatus(const QString& str);
    void setTexture(int txt);

};


