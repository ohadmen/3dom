#pragma once
#include <QtCore\qglobal.h>

template<typename bits>
class QBitFlags
{
public:
    QBitFlags::QBitFlags  (bool up = true) : _flag(up ? 0x1 | -0x1 : 0), _bitsSize(sizeof(bits) << 3){}
    inline bits flag      () const        {return _flag;}
    inline void up        (quint8 value)  {if(value < _bitsSize)_flag |= bt(value);}
    inline void upAll     ()              {_flag |= ~_flag;}
    inline void down      (quint8 value)  {if(value < _bitsSize)_flag &= ~bt(value);}
    inline void downAll   ()              {_flag = bits(0x0);}
    inline bool check     (quint8 value)  {return value < _bitsSize ? _flag & bt(value) : false;}
    inline bool operator()()              {return _flag != 0x0;}
    inline bool operator()(quint8 value)  {return check(value);}
    inline void operator+=(quint8 value)  {up(value);}
    inline void operator-=(quint8 value)  {down(value);}
    inline void operator++(int)           {upAll();}
    inline void operator--(int)           {downAll();}
protected:
    inline bits bt(quint8 value) const {return bits(0x1) << value;}
    bits   _flag;
    quint8 _bitsSize;
};

#ifndef QT_NO_DEBUG_STREAM
template<typename bits>
QDebug operator << (QDebug dbg, const QBitFlags<bits> &value)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << QString::number(value.flag(), 2);
    return dbg;
}
#endif
