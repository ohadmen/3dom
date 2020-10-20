#pragma once
#include <stddef.h>
#include <unordered_map>

class ZviewInf
{
public:
    virtual int getHandleNumFromString(const char* name)=0;
    virtual int getLastKeyStroke()=0;
    virtual bool savePly(const char* fn)=0;
    virtual bool setCameraLookAt(float ex,float ey,float ez,float cx,float cy,float cz,float ux,float uy,float uz)=0;
    virtual bool updatePoints(int key,size_t npoints ,const float* xyz)=0;
    virtual bool updateColoredPoints(int key,size_t npoints,const void* xyzrgba)=0;
    virtual int addPoints(const char *name, size_t npoints, const float *xyz)=0;
    virtual int addColoredPoints(const char *name, size_t npoints, const void *xyzrgba)=0;
    virtual int addMesh(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices)=0;
    virtual int addColoredMesh(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices)=0;
    virtual int addEdges(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices)=0;
    virtual int addColoredEdges(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices)=0;
    virtual bool loadFile(const char *filename)=0;
    virtual bool removeShape(int key)=0;
    static ZviewInf* create();
    virtual void destroy() = 0;

    virtual ~ZviewInf(){}
};
