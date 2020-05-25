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
    bool savePly(const char* fn) override;
    bool setCameraLookAt(float x,float y,float z) override;
    bool setCameraPosition(float x,float y,float z) override;
    bool updatePoints(int key,size_t npoints,const float* xyz) override;
    bool updateColoredPoints(int key,size_t npoints,const void* xyzrgba) override;
    int addPoints(const char *name, size_t npoints, const float *xyz) override;
    int addColoredPoints(const char* name,size_t npoints,const void* xyzrgba) override;
    int addMesh(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices) override;
    int addMeshColor(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices) override;
    int addEdges(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices) override;
    int addEdgesColor(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices) override;
    bool loadFile(const char *filename) override;
    bool removeShape(int key) override;
    void destroy() override;

    static constexpr size_t SHARED_MEMORY_SIZE_BYTES = size_t(1) << 25 ; //~34Mbyte,to support RealSense XVGA depth buffer
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
        REMOVE_SHAPE,
        UPDATE_PCL,
        SAVE_PLY,
        SET_CAM_LOOKAT,
        SET_CAM_POS
    };
    enum class CommandAck
    {
        UNKNOWN,
        ADD_SHAPE_ACK,
        REMOVE_SHAPE_ACK,
        UPDATE_PCL_ACK,
        SAVE_PLY_ACK,
        SET_CAM_LOOKAT_ACK,
        SET_CAM_POS_ACK

    };

    

private:
    QSharedMemory m_data;
    QSharedMemory m_ack;
    QSystemSemaphore m_lock;


    int privGetAck(CommandAck expected);
    
};
