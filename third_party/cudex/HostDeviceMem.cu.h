#pragma once

#include <cuda.h>
#include <stdexcept>


template<class T>
class HostDeviceMem
{
    T* _hostPtr;
    T* _devicePtr;
    size_t _n;
public:
    HostDeviceMem(size_t n,T initVal = T(0)):_n(n)
    {
        _hostPtr = new T[n];
        size_t sz = n * sizeof(T);
        cudaError_t e = cudaMalloc(&_devicePtr, sz);
        if(e != cudaError_t::cudaSuccess)
            throw std::runtime_error("[x] could not allocated cuda memory");
        for(size_t i=0;i!=_n;++i)
            _devicePtr[i]=initVal;
        std::fill(beginH(),endH(),initVal);

    }
    ~HostDeviceMem()
    {
        delete[] _hostPtr;
        cudaFree(_devicePtr);
    }
    size_t size() const {return _n;}
    bool host2device() const
    {
        cudaError_t e = cudaMemcpy(_devicePtr, _hostPtr, _n * sizeof(T), cudaMemcpyHostToDevice);
        return e == cudaError_t::cudaSuccess;
    }
    bool device2host() const
    {
        cudaError_t e = cudaMemcpy(_hostPtr,_devicePtr , _n * sizeof(T), cudaMemcpyDeviceToHost);
        return e == cudaError_t::cudaSuccess;

    }

          T* beginH()       {return _hostPtr;}
    const T* beginH() const {return _hostPtr;}
          T* endH()         {return _hostPtr+_n;}
    const T* endH()   const {return _hostPtr+_n;}
          T* beginD()       {return _devicePtr;}
    const T* beginD() const {return _devicePtr;}
          T* endD()         {return _devicePtr+_n;}
    const T* endD()   const {return _devicePtr+_n;}

    

};