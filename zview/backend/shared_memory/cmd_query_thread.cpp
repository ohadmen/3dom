#include "cmd_query_thread.h"
#include "zview/interface/zview_inf_impl.h"

#include <QDebug>


void CmdQueryThread::run()
{   
    
    while(true)
    {
        m_lock.acquire();
        
        if(m_stop)
            break;
        emit dataReady();
    }
    m_done = true;
}

CmdQueryThread::CmdQueryThread(QObject *parent) : QThread(parent),m_lock(ZviewInfImpl::INTERFACE_LOCK_KEY, 0, QSystemSemaphore::Create),m_stop(false),m_done(false)
{
    
}

void CmdQueryThread::stop()
{
    m_stop=true;
    m_lock.release();
    while(!m_done)
    {
        //qDebug()<<"waiting to be done...";
    }
    //qDebug() << "done";


}
