#pragma once
#include <QThread>
#include <QSystemSemaphore>
#include <QtCore/QSharedMemory>



class CmdQueryThread : public QThread
{
    Q_OBJECT
    QSystemSemaphore m_lock;
    bool m_stop;
    bool m_done;

    void run() override;

public:
    CmdQueryThread(QObject *parent);
void stop()    ;
signals:
    void dataReady();
    

};