#include "drawable_base.h"




DrawableBase::DrawableBase(const std::string& name) :m_name(name),m_active(true),m_vBuff(QOpenGLBuffer::VertexBuffer) {}


bool DrawableBase::isActive() const { return m_active; }
void DrawableBase::setActive(bool a) { m_active = a; }
const std::string& DrawableBase::getName() const { return m_name; }


void DrawableBase::privInitShader(const QString &shaderName)
{
    
    // QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //     qDebug() << it.next();
    // }

    // Compile vertex shader
    if (!m_meshShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/v"+shaderName))
        throw std::runtime_error("could not find glsl vertex shader file");
        

    // Compile fragment shader
    if (!m_meshShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/f"+shaderName))
        throw std::runtime_error("could not find glsl Fragment shader file");
    // Link shader pipeline
    if (!m_meshShader.link())
        throw std::runtime_error("could not link shader");
    
}


DrawableBase::~DrawableBase(){}


void DrawableBase::updateVertexBuffer(const Types::VertData* data,size_t n)
{
    size_t nbytes = n*sizeof(Types::VertData);
    m_vBuff.bind();
    auto ptr = m_vBuff.mapRange(0, nbytes, QOpenGLBuffer::RangeInvalidateBuffer | QOpenGLBuffer::RangeWrite);
    memcpy(ptr, data,  nbytes);
    m_vBuff.unmap();
    m_vBuff.release();

}