#pragma once
#include "Array2d.h"
class VertMap
{
    struct Vert
    {
        float x;
        float y;
        float z;
        float r;
        float g;
        float b;
    };
    Array2d<Vert> m_map;

    //float* getVertices()
    //{
    //    return reinterpret_cast<float*>(&m_mbleap[0][0]);
    //}

};