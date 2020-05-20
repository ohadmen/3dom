#pragma once
#include <QThread>
#include <QSystemSemaphore>
#include <QtCore/QSharedMemory>
#include "zview/interface/zview_inf_impl.h"


class CmdQueryThread : public QThread
{
    Q_OBJECT
    QSystemSemaphore m_lock;
    bool m_stop;
    bool m_done;
    QSharedMemory m_data;
    QSharedMemory m_ack;

    void run() override;
    char* sharedMemData();
    void privReadData();
    std::string privReadName(size_t* offsetP);
    size_t privReadShape(ZviewInfImpl::Command cmd);

public:
    CmdQueryThread(QObject *parent);
void stop()    ;
    

};