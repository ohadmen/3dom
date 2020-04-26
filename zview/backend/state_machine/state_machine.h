#pragma once
#include <QtGui/qevent.h>
#include "zview/backend/vp_mat.h"
#include <map>
#include <memory>
#include <string>
#include "track_state_abs.h"

//----------------------state machine----------------------//
class TrackStateMachine : public QObject
{
    Q_OBJECT

    
    int generate_type_id()
    {

        static int value = 0;
        return value++;
    }

    // template <class T>
    // int type_id()
    // {

    //     static int value = generate_type_id();
    //     return value;
    // }

    template <class T>
    int getKey()
    {
        static int key = generate_type_id();
        auto it = m_statesMap.find(key);
        if (it == m_statesMap.end())
            m_statesMap[key].reset(new T(this));
        return key;
    }

public:
    QVector3D pickClosestObject(const QPointF &xy) const;

    template <class T>
    void setCurrentState()
    {
        m_prevState = m_currentState;
        m_currentState = getKey<T>();
        
    }
    void returnToPerviousState()
    {
        m_currentState = m_prevState;
    }

    template <class T>
    T *getState()
    {
        int key = getKey<T>();
        return static_cast<T *>(m_statesMap[key].get());
    }

    TrackStateMachine();

    void canvasUpdate();
    void setStatus(const std::string &key);
    void setTexture(int k);
    QMatrix4x4 getVPmatrix() const;
    QMatrix4x4 getViewMatrix() const;
    void setViewMatrix(const QMatrix4x4 &m);
    void setWinSize(int w, int h);
    QVector3D xy2screen(const QPointF &xy);
    void updatePmat();

    void input(QInputEvent *e);

signals:
    void signal_canvasUpdate();
    void signal_setStatus(const QString &str);
    void signal_setTexture(int txt);

private:
    std::map<int, std::unique_ptr<TrackStateAbs>> m_statesMap;
    int m_currentState;
    int m_prevState;
    VPmat m_vpmat;
};
