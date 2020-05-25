#include "zview_inf_impl.h"
#include <QtCore/QDebug>
#include "zview/io/read_file_list.h"
#include "zview/common/mem_stream.h"
#include <iostream>

ZviewInfImpl::ZviewInfImpl() : m_lock(ZviewInfImpl::INTERFACE_LOCK_KEY, 0, QSystemSemaphore::Create)
{
    ZviewInfImpl::initSharedMem(&m_data, &m_ack);
}
ZviewInfImpl::~ZviewInfImpl() {}

void ZviewInfImpl::destroy()
{
    delete this;
}

bool ZviewInfImpl::savePly(const char *fn)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::SAVE_PLY << fn;
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::SAVE_PLY_ACK);
}
bool ZviewInfImpl::setCameraLookAt(float x, float y, float z)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::SET_CAM_LOOKAT << x << y << z;
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::SET_CAM_LOOKAT_ACK);
    return false;
}
bool ZviewInfImpl::setCameraPosition(float x, float y, float z)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::SET_CAM_POS << x << y << z;
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::SET_CAM_POS_ACK);
    return false;
}

class ShapeAddVisitor
{
    ZviewInfImpl *m_zv;

public:
    ShapeAddVisitor(ZviewInfImpl *zv) : m_zv(zv) {}
    int operator()(const Types::Pcl &obj)
    {
        return m_zv->addColoredPoints(obj.getName().c_str(), obj.v().size(), &obj.v()[0]);
    }
    int operator()(const Types::Edges &obj)
    {
        return m_zv->addEdgesColor(obj.getName().c_str(), obj.v().size(), &obj.v()[0], obj.e().size(), &obj.e()[0]);
    }
    int operator()(const Types::Mesh &obj)
    {
        return m_zv->addMeshColor(obj.getName().c_str(), obj.v().size(), &obj.v()[0], obj.f().size(), &obj.f()[0]);
    }
};

bool ZviewInfImpl::loadFile(const char *filename)
{
    QStringList list;
    list.push_back(filename);
    std::vector<Types::Shape> objs = io::readFileList(list);
    if (objs.empty())
        return false;
    ShapeAddVisitor w(this);
    for (const auto &objv : objs)
    {

        int retval = std::visit(w, objv);
        if (retval == -1)
            return false;
    }
    return true;
}

void ZviewInfImpl::initSharedMem(QSharedMemory *data, QSharedMemory *ack)
{

    data->setKey(ZviewInfImpl::INTERFACE_TO_ZVIEW_SHARED_MEM_KEY);
    if (data->attach())
    {
        //qDebug() << "Attached to data shared memory";
    }
    else if (!data->create(ZviewInfImpl::SHARED_MEMORY_SIZE_BYTES))
    {
        qFatal("could not attach to data shared memory: %s", data->errorString().toStdString().c_str());
    }

    ack->setKey(ZviewInfImpl::ZVIEW_TO_INTERFACE_SHARED_MEM_KEY);
    if (ack->attach())
    {
        //qDebug() << "Attached to ack shared memory";
    }
    else if (!ack->create(sizeof(ZviewInfImpl::Command) + 8))
    {
        qFatal("could not attach to ack shared memory: %s", data->errorString().toStdString().c_str());
    }
}

void privWritePoints(MemStream& ms, size_t nelems, const float *xyz)
{
    ms << nelems;
    for (size_t i = 0; i != nelems; ++i)
    {
        ms << xyz[i * 3 + 0]
           << xyz[i * 3 + 1]
           << xyz[i * 3 + 2]
           << xyz[i * 3 + 3]
           << 0;
    }
}
void privWritePointsColor(MemStream& ms, size_t nelems, const void *xyzrgba)
{

    size_t sz = nelems * 4 * sizeof(float);
    ms << nelems;
    ms.copyFrom(xyzrgba,sz);
}
void privWriteEdges(MemStream& ms, size_t nelems, const void *edges)
{
    size_t sz = nelems * 2 * sizeof(int32_t);
    ms << nelems;
    ms.copyFrom(edges,sz);
}
void privWriteFaces(MemStream& ms, size_t nelems, const void *faces)
{
    size_t sz = nelems * 3 * sizeof(int32_t);
    ms << nelems;
    ms.copyFrom(faces,sz);
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


int ZviewInfImpl::addPoints(const char *name, size_t npoints, const float *xyz)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::ADD_PCL << name;
    privWritePoints(ms, npoints, xyz);
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addColoredPoints(const char *name, size_t npoints, const void *xyzrgba)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::ADD_PCL << name;
    privWritePointsColor(ms, npoints, xyzrgba);
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addMesh(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::ADD_MESH << name;
    privWritePoints(ms, npoints, xyz);
    privWriteFaces(ms, nfaces, indices);
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addMeshColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::ADD_MESH << name;
    privWritePointsColor(ms, npoints, xyzrgba);
    privWriteFaces(ms, nfaces, indices);
    m_data.unlock();
    m_lock.release();

    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addEdges(const char *name, size_t npoints, const float *xyz, size_t nedges, const void *indices)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::ADD_MESH << name;
    privWritePoints(ms, npoints, xyz);
    privWriteEdges(ms, nedges, indices);
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}
int ZviewInfImpl::addEdgesColor(const char *name, size_t npoints, const void *xyzrgba, size_t nedges, const void *indices)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::ADD_MESH << name;
    privWritePointsColor(ms, npoints, xyzrgba);
    privWriteEdges(ms, nedges, indices);
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::ADD_SHAPE_ACK);
}

bool ZviewInfImpl::removeShape(int key)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::REMOVE_SHAPE << qint64(key);
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::REMOVE_SHAPE_ACK);
}

bool ZviewInfImpl::updatePoints(int key, size_t npoints, const float *xyz)
{
    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::UPDATE_PCL << key << npoints;
    privWritePoints(ms, npoints, xyz);
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::UPDATE_PCL_ACK);
}
bool ZviewInfImpl::updateColoredPoints(int key, size_t npoints, const void *xyzrgba)
{

    m_data.lock();
    MemStream ms(m_data.data());
    ms << Command::UPDATE_PCL << key << npoints;
    privWritePointsColor(ms, npoints, xyzrgba);
    m_data.unlock();
    m_lock.release();
    return privGetAck(CommandAck::UPDATE_PCL_ACK);
}


#if defined(_MSC_VER)
extern "C" __declspec(dllexport) __cdecl ZviewInfImpl *create_zviewinf()
{

    return new ZviewInfImpl;
}
#endif
