#include "zview_inf.h"
#include "zview_inf_impl.h"
ZviewInf* ZviewInf::create()
{
    return new ZviewInfImpl;
}


#if defined(_MSC_VER)
extern "C" __declspec(dllexport) ZviewInfImpl* __cdecl create_zviewinf()
{
    return static_cast<ZviewInfImpl*>(ZviewInf::create());

}
#endif
