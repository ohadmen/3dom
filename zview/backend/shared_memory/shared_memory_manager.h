#pragma once
#include <QtCore/QSharedMemory>
#include <QObject>
#include "zview/gui/drawables/drawables_buffer.h"
#include "zview/interface/zview_inf_impl.h"
class CmdQueryThread;
struct ReadAck;
class SharedMemoryManager : public QObject
{
    Q_OBJECT
    CmdQueryThread* m_th;

    QSharedMemory m_data;
    QSharedMemory m_ack;

    const char* sharedMemData() const;
    ReadAck privReadData() const;
    std::string privReadName(const char* dataptr,size_t* offsetP) const;
    qint64 privReadShape(ZviewInfImpl::Command cmd) const;

public:
    SharedMemoryManager(QObject* parent);
    ~SharedMemoryManager();

    void proccessSharedMemory();
    
    
};