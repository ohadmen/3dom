#include "shared_memory_manager.h"
#include "cmd_query_thread.h"
#include <QThread>
#include <QDebug>

SharedMemoryManager::SharedMemoryManager(QObject *parent) : QObject(parent)
{


    CmdQueryThread *th = new CmdQueryThread(this);
    connect(th, &CmdQueryThread::cmdReady, this, &SharedMemoryManager::cmdRecieved);
    connect(th, &CmdQueryThread::finished, th, &QObject::deleteLater);
    connect(this, &SharedMemoryManager::stop, th, &CmdQueryThread::stop);
    th->start();
}

void SharedMemoryManager::cmdRecieved(const QString &cmd)
{
    
}
SharedMemoryManager::~SharedMemoryManager()
{
    emit stop();
}