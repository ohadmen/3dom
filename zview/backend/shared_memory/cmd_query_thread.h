#pragma once
#include <QThread>
#include <QSystemSemaphore>
#include <QtCore/QSharedMemory>



class CmdQueryThread : public QThread
{
    Q_OBJECT
    
    static constexpr int cmdSize = 8;
    QSystemSemaphore m_lock;
    bool m_stop;
    bool m_done;
    QSharedMemory m_cmd;

    void run() override;
public:
    CmdQueryThread(QObject *parent);
    
    
signals:
    void cmdReady(const QString &s);
public slots:
    void stop();
};