#include "zview_inf_impl.h"

ZviewInfImpl::ZviewInfImpl() : m_lock(INTERFACE_LOCK_KEY, 0, QSystemSemaphore::Create)
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
bool ZviewInfImpl::privWriteName(size_t *offsetP, const char *name)
{
    char *to = sharedMemData();
    strcpy(to + sizeof(Command), name);
    *offsetP = strlen(name) + sizeof(Command);
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

    memcpy(sharedMemData() + *offsetP, &npoints, sizeof(size_t));
    float *to = reinterpret_cast<float *>(sharedMemData() + *offsetP + sizeof(size_t));

    memcpy(to, xyzrgba, sz);
    *offsetP += sz;
    return true;
}
bool ZviewInfImpl::privWriteEdges(size_t *offsetP, size_t nedges, const void *edges)
{
    size_t sz = nedges * 2 * sizeof(int32_t);
    if (*offsetP + sz > SHARED_MEMORY_SIZE_BYTES)
        return false;

    memcpy(sharedMemData() + *offsetP, &nedges, sizeof(size_t));
    int32_t *to = reinterpret_cast<int32_t *>(sharedMemData() + *offsetP + sizeof(size_t));

    memcpy(to, edges, sz);
    *offsetP += sz;
    return true;
}
bool ZviewInfImpl::privWriteFaces(size_t *offsetP, size_t nfaces, const void *faces)
{
    size_t sz = nfaces * 3 * sizeof(int32_t);
    if (*offsetP + sz > SHARED_MEMORY_SIZE_BYTES)
        return false;
    memcpy(sharedMemData() + *offsetP, &nfaces, sizeof(size_t));
    int32_t *to = reinterpret_cast<int32_t *>(sharedMemData() + *offsetP + sizeof(size_t));
    memcpy(to, faces, sz);
    *offsetP += sz;
    return true;
}

int ZviewInfImpl::privGetObjKey()
{
    
    auto tic = std::chrono::high_resolution_clock::now();
    float timeElapsed;
    Command cmd;
    do
    {
        auto toc = std::chrono::high_resolution_clock::now();
        timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(toc - tic).count();
        m_ack.lock();
        memcpy(&cmd,m_ack.data(),sizeof(Command));
        m_ack.unlock();
        if(cmd==Command::ACK_ADD)
        {
            int32_t retval;
            memcpy(&retval,static_cast<char*>(m_ack.data())+sizeof(Command),sizeof(int32_t));
            *static_cast<Command*>(m_ack.data())=Command::UNKNOWN;
            return retval;
        }
        
        
    } while (timeElapsed<1);
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
    size_t offset;
    if (!privWriteName(&offset, name))
        return false;
    if (!privWritePoints(&offset, npoints, xyz))
        return false;
    m_data.unlock();
    m_lock.release();
    return privGetObjKey();
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
    return privGetObjKey();
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
    return privGetObjKey();
}
int ZviewInfImpl::addMeshColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices)
{
    m_data.lock();
    if (!privWriteCmd(Command::ADD_MESH))
        return false;
    size_t offset;
    if (!privWriteName(&offset, name))
        return false;
    if (!privWritePointsColor(&offset, npoints, xyzrgba))
        return false;
    if (!privWriteFaces(&offset, nfaces, indices))
        return false;
    m_data.unlock();
    m_lock.release();
    return privGetObjKey();
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
    return privGetObjKey();
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
    return privGetObjKey();
}
