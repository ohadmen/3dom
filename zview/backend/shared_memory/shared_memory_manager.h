#pragma once
#include <QtCore/QSharedMemory>
#include <QObject>
#include "zview/gui/drawables/drawables_buffer.h"
class CmdQueryThread;
class SharedMemoryManager : public QObject
{
    Q_OBJECT
    CmdQueryThread* m_th;
public:
    SharedMemoryManager(QObject* parent);
    ~SharedMemoryManager();
    
    
};