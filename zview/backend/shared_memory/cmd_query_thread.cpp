#include "cmd_query_thread.h"
#include <QDebug>

void CmdQueryThread::run()
{
    while(true)
    {
        m_lock.acquire();
        if(m_stop)
            break;
        m_cmd.lock();
        qDebug() << "read command...";
        m_cmd.unlock();


    }
    qDebug() << "stopped";
    QString result;
    /* ... here is the expensive or blocking operation ... */
    emit cmdReady(result);
    m_done = true;
}

CmdQueryThread::CmdQueryThread(QObject *parent) : QThread(parent),m_lock("zview_lock", 0, QSystemSemaphore::Create),m_stop(false),m_done(false)
{
    m_cmd.setNativeKey("zview_cmd");
    if (m_cmd.isAttached())
        m_cmd.detach();
    if (!m_cmd.create(cmdSize))
        throw std::runtime_error("could not attach to command shared memory");

}

void CmdQueryThread::stop()
{
    m_stop=true;
    m_lock.release();
    while(!m_done)
    {
        qDebug()<<"waiting to be done...";
    }


}
