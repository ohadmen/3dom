#pragma once
#include <QSharedMemory>
#include <QSystemSemaphore>
#include "zview/common/common_types.h"
class ZviewInfImpl
{

public:
    ZviewInfImpl();
    
    int addPoints(const char* name,size_t npoints,const float* xyz);
    int addPointsColor(const char* name,size_t npoints,const void* xyzrgba);
    int addMesh(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices);
    int addMeshColor(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices);
    int addEdges(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices);
    int addEdgesColor(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices);
    static constexpr size_t SHARED_MEMORY_SIZE_BYTES = size_t(1) << 25 ; //~34Mbyte
    static constexpr char INTERFACE_TO_ZVIEW_SHARED_MEM_KEY[] = "zview_from_interface" ;
    static constexpr char ZVIEW_TO_INTERFACE_SHARED_MEM_KEY[] = "zview_to_interface" ;
    static constexpr char INTERFACE_LOCK_KEY[] = "zview_lock" ;
    
    enum class Command
    {
        UNKNOWN,
        ADD_PCL,
        ADD_EDGES,
        ADD_MESH,
        UPDATE_VERTICES,
        REMOVE_SHAPE,
        ACK_ADD,
    };

private:
    QSharedMemory m_data;
    QSharedMemory m_ack;
    QSystemSemaphore m_lock;

    char* sharedMemData();

    bool privWriteCmd(const Command &cmd);
    bool privWriteName(size_t* offsetP,const char *name);
    bool privWritePoints(size_t* offsetP,size_t npoints, const float* xyz);
    bool privWritePointsColor(size_t* offsetP, size_t npoints, const void *xyzrgba);
    bool privWriteEdges(size_t* offsetP, size_t nedges, const void *edges);
    bool privWriteFaces(size_t* offsetP, size_t nfaces, const void *faces);
    int privGetObjKey();
};