#pragma once
#include <QSharedMemory>
#include <QSystemSemaphore>
#include "zview/common/common_types.h"
#include "zview_inf.h"
class ZviewInfImpl: public ZviewInf
{

public:
    ZviewInfImpl();
    ~ZviewInfImpl();
    
    int addPoints(const char *name, size_t npoints, const float *xyz) override;
    int addPointsColor(const char* name,size_t npoints,const void* xyzrgba) override;
    int addMesh(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices) override;
    int addMeshColor(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices) override;
    int addEdges(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices) override;
    int addEdgesColor(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices) override;
    bool loadFile(const char *filename) override;
    bool removeShape(int key) override;
    void destroy() override;

    static constexpr size_t SHARED_MEMORY_SIZE_BYTES = size_t(1) << 25 ; //~34Mbyte
    static constexpr char INTERFACE_TO_ZVIEW_SHARED_MEM_KEY[] = "zview_from_interface" ;
    static constexpr char ZVIEW_TO_INTERFACE_SHARED_MEM_KEY[] = "zview_to_interface" ;
    static constexpr char INTERFACE_LOCK_KEY[] = "zview_lock" ;
    
    static void initSharedMem(QSharedMemory* data,QSharedMemory* ack);

    enum class Command
    {
        UNKNOWN,
        ADD_PCL,
        ADD_EDGES,
        ADD_MESH,
        UPDATE_VERTICES,
        REMOVE_SHAPE,
    };
    enum class CommandAck
    {
        UNKNOWN,
        ADD_SHAPE_ACK,
        REMOVE_SHAPE_ACK,

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
    int privGetAck(CommandAck expected);
};
