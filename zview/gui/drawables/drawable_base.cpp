#include "drawable_base.h"
#include "drawable_common.h"




DrawableBase::DrawableBase() :m_active(true),m_vBuff(QOpenGLBuffer::VertexBuffer) {}


bool DrawableBase::isActive() const { return m_active; }
void DrawableBase::setActive(bool a) { m_active = a; }



void DrawableBase::privInitShader(const QString &shaderName)
{
    DrawableCommon::initShadar(&m_meshShader,shaderName);
    
}


DrawableBase::~DrawableBase(){}


bool DrawableBase::updateVertexBuffer(const Types::VertData* data,size_t n)
{
    size_t nbytes = n*sizeof(Types::VertData);
    m_vBuff.bind();
    auto ptr = m_vBuff.mapRange(0, nbytes, QOpenGLBuffer::RangeInvalidateBuffer | QOpenGLBuffer::RangeWrite);
    if(!ptr)
    return false;
        memcpy(ptr, data,  nbytes);
    m_vBuff.unmap();
    m_vBuff.release();
    return true;

}