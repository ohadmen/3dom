#include "shared_memory_manager.h"
#include "cmd_query_thread.h"
#include <QThread>
#include <QDebug>

SharedMemoryManager::SharedMemoryManager(QObject *parent) : QObject(parent)
{


    m_th = new CmdQueryThread(this);
    connect(m_th, &CmdQueryThread::cmdReady, this, &SharedMemoryManager::cmdRecieved);
    connect(m_th, &CmdQueryThread::finished, m_th, &QObject::deleteLater);
    connect(this, &SharedMemoryManager::stop, m_th, &CmdQueryThread::stop);
    m_th->start();
}

void SharedMemoryManager::cmdRecieved(const QString &cmd)
{
    Q_UNUSED(cmd);
}
SharedMemoryManager::~SharedMemoryManager()
{
    emit stop();
}