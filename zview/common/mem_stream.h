#pragma once
#include <stdlib.h>
#include <cstring> //strlen,strcpy
#include <string>
#include <vector>

class ConstMemStream
{
protected:
    const char *m_data;
    size_t m_byteCounter;

public:
    ConstMemStream(const void *ptr) : m_data(static_cast<const char *>(ptr)), m_byteCounter{0} {}

    template <class T>
    T getMemPtr(int offset = 0) const { return reinterpret_cast<T>(m_data + m_byteCounter + offset); }

    void copyTo(void* ptr, size_t nBytes)
    {
        memcpy(ptr,getMemPtr<const char*>(), nBytes);
        m_byteCounter +=nBytes;
    }

    friend ConstMemStream& operator>>(ConstMemStream &mem, std::string &val);
    template <class T>
    friend  inline ConstMemStream& operator>>(ConstMemStream &mem, std::vector<T> &preallocVector);

    template <class T>
    friend ConstMemStream& operator>>(ConstMemStream &mem, T &val);
    

};

class MemStream : public ConstMemStream
{
public:
    MemStream(void *ptr) : ConstMemStream(ptr) {}

    template <class T>
    T getMemPtr(int offset = 0) { return reinterpret_cast<T>(const_cast<char*>(m_data) + m_byteCounter + offset); }
    void copyFrom(const void* ptr, size_t nBytes)
    {
        memcpy(getMemPtr<char*>(), ptr, nBytes);
        m_byteCounter +=nBytes;
    }
    template<class T>
    void writeConstant(const T& val, size_t count)
    {
        T* p=getMemPtr<T*>();
        std::fill(p,p+count,val);
        m_byteCounter += count*sizeof(T);
            
    }

    template <class T>
    friend MemStream& operator<<(MemStream &mem, const T &val);
    friend MemStream& operator<<(MemStream &mem, const std::string &val);
    friend MemStream& operator<<(MemStream &mem, const char* val);
};

inline MemStream& operator<<(MemStream &mem, const char* val)
{
    strcpy(mem.getMemPtr<char *>(), val);
    mem.m_byteCounter += strlen(val) + 1;
    return mem;
}
inline MemStream& operator<<(MemStream &mem, const std::string &val)
{
    mem.m_byteCounter += val.copy(mem.getMemPtr<char *>(), val.size() + 1);
    return mem;
}
template <class T>
inline  MemStream& operator<<(MemStream &mem, const T &val)
{
    *mem.getMemPtr<T *>() = val;
    mem.m_byteCounter += sizeof(T);
    return mem;
}

inline  ConstMemStream& operator>>(ConstMemStream &mem, std::string &val)
{
    val = std::string(mem.getMemPtr<const char *>());
    mem.m_byteCounter += val.size() + 1;
    return mem;
}
template <class T>
inline ConstMemStream& operator>>(ConstMemStream &mem, std::vector<T> &preallocVector)
{
    
    size_t sz = sizeof(T)*preallocVector.size();
    mem.copyTo(&preallocVector[0],sz);
    return mem;
}
template <class T>
inline ConstMemStream& operator>>(ConstMemStream &mem, T &val)
{
    val = *mem.getMemPtr<const T *>();
    mem.m_byteCounter += sizeof(T);
    return mem;
}
