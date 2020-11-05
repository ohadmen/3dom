#include "cmd_query_thread.h"
#include "zview/interface/zview_inf_impl.h"

#include <QDebug>


void CmdQueryThread::run()
{   
    m_done = false;
    while(true)
    {
        m_lock.acquire();
        
        if(m_stop)
            break;
        emit dataReady();
    }
    m_done = true;
}

CmdQueryThread::CmdQueryThread(QObject *parent) : QThread(parent),m_lock(ZviewInfImpl::INTERFACE_LOCK_KEY, 0, QSystemSemaphore::Create),m_stop(false),m_done(true)
{
    
}
CmdQueryThread::~CmdQueryThread()
{
    stop();
}

void CmdQueryThread::stop()
{
    if(m_done)
        return;
    m_stop=true;
    m_lock.release();
    for(int i=0;i!=10000 && !m_done;++i)
    {
        sleep(1);
        // qDebug()<<"waiting to be done...";
    }
     //qDebug() << "done";


}
