#include "zview_inf_impl.h"

ZviewInfImpl::ZviewInfImpl():m_lock("zview_lock", 0, QSystemSemaphore::Create)
{
    
       m_cmd.setNativeKey("zview_cmd");
    if (m_cmd.isAttached())
        m_cmd.detach();
    if (!m_cmd.create(cmdSize))
        throw std::runtime_error("could not attach to command shared memory");
}
int ZviewInfImpl::addMesh()
{

}