#include "zview_inf.h"
#include "zview_inf_impl.h"
ZviewInf* ZviewInf::create()
{
    return new ZviewInfImpl;
}
