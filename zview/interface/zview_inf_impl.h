#pragma once
#include <QSharedMemory>
#include <QSystemSemaphore>


class ZviewInfImpl
{
    static constexpr int cmdSize = 8;
    QSharedMemory m_cmd;
    QSystemSemaphore m_lock;
    public:
        ZviewInfImpl();
        int addMesh();
};