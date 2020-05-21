#pragma once
#include <stddef.h>
class ZviewInfImpl;
class ZviewIntrace
{
    ZviewInfImpl *zv;

public:
    ZviewIntrace();
    ~ZviewIntrace();

    int addPoints(const char *name, size_t npoints, const float *xyz);
    int addPointsColor(const char *name, size_t npoints, const void *xyzrgba);
    int addMesh(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices);
    int addMeshColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices);
    int addEdges(const char *name, size_t npoints, const float *xyz, size_t nfaces, const void *indices);
    int addEdgesColor(const char *name, size_t npoints, const void *xyzrgba, size_t nfaces, const void *indices);

    bool loadFile(const char *filename);
    bool removeShape(int key);
};