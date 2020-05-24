#include "zview_inf_impl.h"
#include <QtCore/QDebug>
#include "zview/io/read_file_list.h"
#include <iostream>

class ShapeAddVisitor
{
    ZviewInfImpl *m_zv;

public:
    ShapeAddVisitor(ZviewInfImpl *zv) : m_zv(zv) {}
    int operator()(const Types::Pcl &obj)
     {return m_zv->addPointsColor(obj.getName().c_str(),obj.v().size(),&obj.v()[0]); }
    int operator()(const Types::Edges &obj) 
    {return m_zv->addEdgesColor(obj.getName().c_str(),obj.v().size(),&obj.v()[0],obj.e().size(),&obj.e()[0]); }
    int operator()(const Types::Mesh &obj)
    {return m_zv->addMeshColor(obj.getName().c_str(),obj.v().size(),&obj.v()[0],obj.f().size(),&obj.f()[0]); }
};



bool ZviewInfImpl::loadFile(const char *filename)
{
    QStringList list;
    list.push_back(filename);
    std::vector<Types::Shape> objs = io::readFileList(list);
    if(objs.empty())
        return false;
    ShapeAddVisitor w(this);
    for (const auto &objv : objs)
    {

        int retval = std::visit(w, objv);
        if(retval==-1)
        return false;
    }
    return true;
}

void ZviewInfImpl::initSharedMem(QSharedMemory *data, QSharedMemory *ack)
{

    data->setKey(ZviewInfImpl::INTERFACE_TO_ZVIEW_SHARED_MEM_KEY);
    if (data->attach())
    {
        qDebug() << "Attached to data shared memory";
    }
    else if (!data->create(ZviewInfImpl::SHARED_MEMORY_SIZE_BYTES))
    {
        qFatal("could not attach to data shared memory: %s", data->errorString().toStdString().c_str());
    }

    ack->setKey(ZviewInfImpl::ZVIEW_TO_INTERFACE_SHARED_MEM_KEY);
    if (ack->attach())
    {
        qDebug() << "Attached to ack shared memory";
    }
    else if (!ack->create(sizeof(ZviewInfImpl::Command) + 8))
    {
        qFatal("could not attach to ack shared memory: %s", data->errorString().toStdString().c_str());
    }
}

ZviewInfImpl::ZviewInfImpl() : m_lock(ZviewInfImpl::INTERFACE_LOCK_KEY, 0, QSystemSemaphore::Create)
{
    ZviewInfImpl::initSharedMem(&m_data, &m_ack);
}
ZviewInfImpl::~ZviewInfImpl(){}

bool ZviewInfImpl::privWriteName(size_t *offsetP, const char *name)
{
    char *to = sharedMemData();
    strcpy(to + *offsetP,name);
    *offsetP += strlen(name) + 1;
    return true;
}
char *ZviewInfImpl::sharedMemData() { return static_cast<char *>(m_data.data()); }
bool ZviewInfImpl::privWritePoints(size_t *offsetP, size_t npoints, const float *xyz)
{
    size_t sz = npoints * 4 * sizeof(float);
    if (*offsetP + sz > SHARED_MEMORY_SIZE_BYTES)
        return false;

    float *to = reinterpret_cast<float *>(sharedMemData() + *offsetP);

    for (size_t i = 0; i != npoints; ++i)
    {
        to[i * 4 + 0] = xyz[i * 3 + 0];
        to[i * 4 + 1] = xyz[i * 3 + 1];
        to[i * 4 + 2] = xyz[i * 3 + 2];
        to[i * 4 + 2] = 0;
    }
    *offsetP += sz;
    return true;
}
bool ZviewInfImpl::privWritePointsColor(size_t *offsetP, size_t npoints, const void *xyzrgba)
{

    size_t sz = npoints * 4 * sizeof(float);
    if (*offsetP + sz > SHARED_MEMORY_SIZE_BYTES)
        return false;

    {
        char *to = sharedMemData() + *offsetP;
        memcpy(to, &npoints, sizeof(size_t));
        *offsetP += sizeof(size_t);
    }
    {
        char *to = sharedMemData() + *offsetP;
        memcpy(to, xyzrgba, sz);
        *offsetP += sz;
    }

    return true;
}
bool ZviewInfImpl::privWriteEdges(size_t *offsetP, size_t nedges, const void *edges)
{
    size_t sz = nedges * 2 * sizeof(int32_t);
    if (*offsetP + sz > SHARED_MEMORY_SIZE_BYTES)
    {
        return false;
    }
    {
        char *to = sharedMemData() + *offsetP;
        memcpy(to, &nedges, sizeof(size_t));
        *offsetP += sizeof(size_t);
    }
    {
        char *to = sharedMemData() + *offsetP;
        memcpy(to, edges, sz);
        *offsetP += sz;
    }
    return true;
}
bool ZviewInfImpl::privWriteFaces(size_t *offsetP, size_t nfaces, const void *faces)
{
    size_t sz = nfaces * 3 * sizeof(int32_t);
    if (*offsetP + sz > SHARED_MEMORY_SIZE_BYTES)
    {
        return false;
    }
    {
        char *to = sharedMemData() + *offsetP;
        memcpy(to, &nfaces, sizeof(size_t));
        *offsetP += sizeof(size_t);
    }
    {
        char *to = sharedMemData() + *offsetP;
        memcpy(to, faces, sz);
        *offsetP += sz;
    }
    return true;
}

int ZviewInfImpl::privGetAck(CommandAck expectedAck)
{

    auto tic = std::chrono::high_resolution_clock::now();
    float timeElapsed;

    CommandAck ack;
    do
    {
        auto toc = std::chrono::high_resolution_clock::now();
        timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(toc - tic).count();
        m_ack.lock();
        memcpy(&ack, m_ack.data(), sizeof(Command));
        m_ack.unlock();
        if (ack == expectedAck)
        {
            qint64 retval;
            memcpy(&retval, static_cast<char *>(m_ack.data()) + sizeof(Command), sizeof(qint64));
            *static_cast<Command *>(m_ack.data()) = Command::UNKNOWN;
            return int(retval);
        }

    } while (timeElapsed < 1);
    return -1;
}

bool ZviewInfImpl::privWriteCmd(const Command &cmd)
{

    char *to = sharedMemData();
    memcpy(to, &cmd, sizeof(Command));
    return true;
}

int ZviewInfImpl::addPoints(const char *name, size_t npoints, const float *xyz)
{
    m_data.lock();
    if (!privWriteCmd(Command::ADD_PCL))
        return false;
    size_t offset = sizeof(Command);
    if (!privWriteName(&offset, name))
        return false;
    if (!privWritePoints(&offset, npoints, xyz))
        return false;
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addPointsColor(const char *name, size_t npoints, const void *xyzrgba)
{
    m_data.lock();
    if (!privWriteCmd(Command::ADD_PCL))
        return false;
    size_t offset;
    if (!privWriteName(&offset, name))
        return false;
    if (!privWritePointsColor(&offset, npoints, xyzrgba))
        return false;
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addMesh(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices)
{
    m_data.lock();
    if (!privWriteCmd(Command::ADD_MESH))
        return false;
    size_t offset;
    if (!privWriteName(&offset, name))
        return false;
    if (!privWritePoints(&offset, npoints, xyz))
        return false;
    if (!privWriteFaces(&offset, nfaces, indices))
        return false;
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addMeshColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices)
{
    m_data.lock();
    if (!privWriteCmd(Command::ADD_MESH))
        return false;
    size_t offset = sizeof(Command);
    if (!privWriteName(&offset, name))
        return false;
    if (!privWritePointsColor(&offset, npoints, xyzrgba))
        return false;
    if (!privWriteFaces(&offset, nfaces, indices))
        return false;
    m_data.unlock();
    m_lock.release();

    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addEdges(const char *name, size_t npoints, const float *xyz, size_t nedges, const void *indices)
{
    m_data.lock();
    if (!privWriteCmd(Command::ADD_MESH))
        return false;
    size_t offset;
    if (!privWriteName(&offset, name))
        return false;
    if (!privWritePoints(&offset, npoints, xyz))
        return false;
    if (!privWriteEdges(&offset, nedges, indices))
        return false;
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addEdgesColor(const char *name, size_t npoints, const void *xyzrgba, size_t nedges, const void *indices)
{
    m_data.lock();
    if (!privWriteCmd(Command::ADD_MESH))
        return false;
    size_t offset;
    if (!privWriteName(&offset, name))
        return false;
    if (!privWritePointsColor(&offset, npoints, xyzrgba))
        return false;
    if (!privWriteEdges(&offset, nedges, indices))
        return false;
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}

bool ZviewInfImpl::removeShape(int key)
{
    m_data.lock();
    if (!privWriteCmd(Command::REMOVE_SHAPE))
        return false;
    qint64 key64 = key;
    memcpy(sharedMemData() + sizeof(Command), &key64, sizeof(qint64));
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::REMOVE_SHAPE_ACK);
}
void ZviewInfImpl::destroy()
{
    delete this;
}

extern "C" __declspec(dllexport) ZviewInfImpl* __cdecl create_zviewinf()
{

    return new ZviewInfImpl;

}
