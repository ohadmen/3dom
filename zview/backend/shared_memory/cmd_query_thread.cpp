#include "cmd_query_thread.h"
#include "zview/interface/zview_inf_impl.h"
#include "zview/gui/drawables/drawables_buffer.h"

#include <QDebug>

char *CmdQueryThread::sharedMemData() { return static_cast<char *>(m_data.data()); }

std::string CmdQueryThread::privReadName(size_t* offsetP)
{
    std::string name(sharedMemData()+*offsetP);
    *offsetP  +=name.size()+1;
    return name;
}
template<class T>
std::vector<T> privReadvector(char* dataptr,size_t* offsetP)
{
    size_t sz;
    memcpy(&sz,dataptr+*offsetP,sizeof(size_t));
    std::vector<T> vect(sz);
    memcpy(&vect[0],dataptr+*offsetP+sizeof(size_t),sizeof(T)*sz);
    *offsetP +=sz*sizeof(T);
    return vect;

    
}
size_t CmdQueryThread::privReadShape(ZviewInfImpl::Command cmd)
{
switch(cmd)
{
        case ZviewInfImpl::Command::ADD_PCL:
    {
        size_t offset=sizeof(ZviewInfImpl::Command);
        Types::Pcl obj(privReadName(&offset));
        obj.v() = privReadvector<Types::VertData>(sharedMemData(),&offset);
        return drawablesBuffer.addShape(std::move(obj));
    }

    case ZviewInfImpl::Command::ADD_EDGES:
    {
        size_t offset=sizeof(ZviewInfImpl::Command);
        Types::Edges obj(privReadName(&offset));
        obj.v() = privReadvector<Types::VertData>(sharedMemData(),&offset);
        obj.e() = privReadvector<Types::EdgeIndx>(sharedMemData(),&offset);
        return drawablesBuffer.addShape(std::move(obj));
    }
    case ZviewInfImpl::Command::ADD_MESH:
    {
        size_t offset=sizeof(ZviewInfImpl::Command);
        Types::Mesh obj(privReadName(&offset));
        obj.v() = privReadvector<Types::VertData>(sharedMemData(),&offset);
        obj.f() = privReadvector<Types::FaceIndx>(sharedMemData(),&offset);
        return drawablesBuffer.addShape(std::move(obj));
    }
    default:
    {
        throw std::runtime_error("unknown shape");
    }
}
}

void CmdQueryThread::privReadData()
{
    ZviewInfImpl::Command cmd;
    memcpy(&cmd,m_data.data(),sizeof(ZviewInfImpl::Command));
    switch(cmd)
    {
    case ZviewInfImpl::Command::ADD_PCL:
    case ZviewInfImpl::Command::ADD_EDGES:
    case ZviewInfImpl::Command::ADD_MESH:
    {
        size_t key = privReadShape(cmd);
    }
    default:
        throw std::runtime_error("unknown command");
    }
}


void CmdQueryThread::run()
{   
    
    while(true)
    {
        m_lock.acquire();
        qDebug() << "aqquired";
        if(m_stop)
            break;
        m_data.lock();
        privReadData();
        m_data.unlock();
    }
    m_done = true;
}

CmdQueryThread::CmdQueryThread(QObject *parent) : QThread(parent),m_lock("zview_lock", 0, QSystemSemaphore::Create),m_stop(false),m_done(false)
{
    m_data.setNativeKey(ZviewInfImpl::INTERFACE_TO_ZVIEW_SHARED_MEM_KEY);
    if (m_data.isAttached())
        m_data.detach();
    if (!m_data.create(ZviewInfImpl::SHARED_MEMORY_SIZE_BYTES))
        throw std::runtime_error("could not attach to command shared memory");

    m_ack.setNativeKey(ZviewInfImpl::ZVIEW_TO_INTERFACE_SHARED_MEM_KEY);
    if (m_ack.isAttached())
        m_ack.detach();
    if (!m_ack.create(sizeof(ZviewInfImpl::Command)+8))
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
    qDebug() << "done";


}
