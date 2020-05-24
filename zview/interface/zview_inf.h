#pragma once
#include <stddef.h>

class ZviewInf
{
public:
    virtual int addPoints(const char *name, size_t npoints, const float *xyz)=0;
    virtual int addPointsColor(const char *name, size_t npoints, const void *xyzrgba)=0;
    virtual int addMesh(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices)=0;
    virtual int addMeshColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices)=0;
    virtual int addEdges(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices)=0;
    virtual int addEdgesColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices)=0;
    virtual bool loadFile(const char *filename)=0;
    virtual bool removeShape(int key)=0;
    static ZviewInf* create();
    virtual void destroy() = 0;

    virtual ~ZviewInf(){}
};
