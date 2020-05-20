#include "shared_memory_manager.h"
#include "cmd_query_thread.h"
#include <QThread>
#include <QDebug>

SharedMemoryManager::SharedMemoryManager(QObject *parent)  : QObject(parent)
{


    m_th = new CmdQueryThread(this);
    
    m_th->start();
}

SharedMemoryManager::~SharedMemoryManager()
{
    m_th->stop();
    delete m_th;
}