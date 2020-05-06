#include <array>
#include <cuda_runtime.h>
#include "third_party/cudex/HostDeviceMem.cu.h"

class MeshPicker
{
    float3 *m_v;
    size_t m_nv;
    int3 *m_f;
    size_t m_nf;
    HostDeviceMem<float> m_res;

public:
    MeshPicker(size_t nv, size_t nf);
    void setVptr(float3 *p);
    void setFptr(float3 *p);
    std::array<float, 3> pick(const std::array<float, 3> &p, const std::array<float, 3> &n);
};
