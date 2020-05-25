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

    template<class T>
    T privGetMemPtr(size_t offset=0)const { return reinterpret_cast<T>(static_cast<const char*>(m_data.constData())+offset); }


    ReadAck privReadData() const;
    std::string privReadName(const char* dataptr,size_t* offsetP) const;
    

public:
    SharedMemoryManager(QObject* parent);
    ~SharedMemoryManager();

    void proccessSharedMemory();
signals:
    void signal_savePly(const QString& str) const;
    
    void signal_setCamLookAt(const QVector3D& eye,const QVector3D& center,const QVector3D& up) const;

    
    
};