#pragma once
#include <QtCore/QSharedMemory>
#include <QObject>
#include "zview/gui/drawables/drawables_buffer.h"
class SharedMemoryManager : public QObject
{
    Q_OBJECT
    
    
public:
    SharedMemoryManager(QObject* parent);
    ~SharedMemoryManager();
signals:
    void stop();
public slots:
    void cmdRecieved(const QString &cmd);
    
};