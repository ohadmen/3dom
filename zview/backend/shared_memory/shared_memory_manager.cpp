#include "shared_memory_manager.h"
#include "zview/gui/drawables/drawables_buffer.h"
#include "cmd_query_thread.h"
#include <QThread>
#include <QDebug>

        struct ReadAck {
        ZviewInfImpl::CommandAck cmd;
        qint64 key;
        };


std::string SharedMemoryManager::privReadName(const char* dataptr,size_t* offsetP) const
{
    std::string name(dataptr+*offsetP);
    *offsetP  +=name.size()+1;
    return name;
}
template<class T>
std::vector<T> privReadvector(const char* dataptr,size_t* offsetP) 
{
    size_t sz;
    memcpy(&sz,dataptr+*offsetP,sizeof(size_t));
    std::vector<T> vect(sz);
    memcpy(&vect[0],dataptr+*offsetP+sizeof(size_t),sizeof(T)*sz);
    *offsetP +=sz*sizeof(T)+sizeof(size_t);
    return vect;

    
}
qint64 SharedMemoryManager::privReadShape(ZviewInfImpl::Command cmd) const
{
    const char* data = static_cast<const char *>(m_data.constData());
switch(cmd)
{
        case ZviewInfImpl::Command::ADD_PCL:
    {
        size_t offset=sizeof(ZviewInfImpl::Command);
        Types::Pcl obj(privReadName(data,&offset));
        obj.v() = privReadvector<Types::VertData>(data,&offset);
        return drawablesBuffer.addShape(std::move(obj));
    }

    case ZviewInfImpl::Command::ADD_EDGES:
    {
        size_t offset=sizeof(ZviewInfImpl::Command);
        Types::Edges obj(privReadName(data,&offset));
        obj.v() = privReadvector<Types::VertData>(data,&offset);
        obj.e() = privReadvector<Types::EdgeIndx>(data,&offset);
        return drawablesBuffer.addShape(std::move(obj));
    }
    case ZviewInfImpl::Command::ADD_MESH:
    {
        size_t offset=sizeof(ZviewInfImpl::Command);
        Types::Mesh obj(privReadName(data,&offset));
        obj.v() = privReadvector<Types::VertData>(data,&offset);
        obj.f() = privReadvector<Types::FaceIndx>(data,&offset);
        return drawablesBuffer.addShape(std::move(obj));
    }
    default:
    {
        throw std::runtime_error("unknown shape");
    }
}
}


ReadAck SharedMemoryManager::privReadData() const
{
    ZviewInfImpl::Command cmd;
    memcpy(&cmd,m_data.data(),sizeof(ZviewInfImpl::Command));
    switch(cmd)
    {
    case ZviewInfImpl::Command::ADD_PCL:
    case ZviewInfImpl::Command::ADD_EDGES:
    case ZviewInfImpl::Command::ADD_MESH:
    {
        qint64 key = privReadShape(cmd);
        return ReadAck{ZviewInfImpl::CommandAck::ADD_SHAPE_ACK,key};

        
    }
    case ZviewInfImpl::Command::REMOVE_SHAPE:
    {
        qint64 key;
        const char* source = static_cast<const char*>(m_data.constData())+sizeof(ZviewInfImpl::Command);
        memcpy(&key,source,sizeof(qint64));
        bool ok = drawablesBuffer.removeShape(key);
        return ReadAck{ZviewInfImpl::CommandAck::REMOVE_SHAPE_ACK,qint64{ok}};
    }
    default:
        qWarning() <<"unknown command:" <<  size_t(cmd);
        
    }
    return ReadAck{ZviewInfImpl::CommandAck::UNKNOWN,0};
}

void SharedMemoryManager::proccessSharedMemory()
{
        m_data.lock();
        ReadAck ack = privReadData();
        m_data.unlock();
        m_ack.lock();
        *static_cast<ReadAck*>(m_ack.data())=ack;
        m_ack.unlock();

}

SharedMemoryManager::SharedMemoryManager(QObject *parent)  : QObject(parent)
{

    ZviewInfImpl::initSharedMem(&m_data,&m_ack);
    m_th = new CmdQueryThread(this);
    connect(m_th, &CmdQueryThread::dataReady, this, &SharedMemoryManager::proccessSharedMemory);
    connect(m_th, &CmdQueryThread::finished, m_th, &QObject::deleteLater);
    m_th->start();
    
}

SharedMemoryManager::~SharedMemoryManager()
{
    m_th->stop();
    delete m_th;
}