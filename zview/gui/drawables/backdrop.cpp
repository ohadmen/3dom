#include "backdrop.h"
#include "zview/common/common_types.h"
#include "zview/common/params.h"
// #include "QtCore/qdiriterator.h"



void Backdrop::setBGcolor()
{
    std::array<Types::VertData, 4> vbuf;
    if (Params::whiteBackground())
        vbuf = {
            Types::VertData(-1.0f, -1.0f, 0.99f, 255, 255, 255),
            Types::VertData(-1.0f, +1.0f, 0.99f, 255, 255, 255),
            Types::VertData(+1.0f, -1.0f, 0.99f, 255, 255, 255),
            Types::VertData(+1.0f, +1.0f, 0.99f, 255, 255, 255)};
    else
        vbuf = {
            Types::VertData(-1.0f, -1.0f, 0.99f, 0.00 * 255, 0.10 * 255, 0.15 * 255),
            Types::VertData(-1.0f, +1.0f, 0.99f, 0.03 * 255, 0.21 * 255, 0.26 * 255),
            Types::VertData(+1.0f, -1.0f, 0.99f, 0.00 * 255, 0.12 * 255, 0.18 * 255),
            Types::VertData(+1.0f, +1.0f, 0.99f, 0.06 * 255, 0.26 * 255, 0.30 * 255)};

    if (m_verts.isCreated())
        m_verts.destroy();

    m_verts.create();
    m_verts.bind();
    m_verts.allocate(vbuf.data(), sizeof(vbuf));
    m_verts.release();
}
Backdrop::Backdrop():DrawableBase("Backdrop"){}

void Backdrop::initializeGL()
{
    Q_INIT_RESOURCE(shaders);
    initializeOpenGLFunctions();
    initShader("mesh");
    setBGcolor();

}

void Backdrop::initShader(const QString &shaderName)
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
void Backdrop::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void Backdrop::paintGL(const QMatrix4x4& , int )
{
    m_meshShader.bind();
    m_verts.bind();
    QMatrix4x4 eye;
    eye.setToIdentity();
    m_meshShader.setUniformValue("mvp_matrix", eye);
    m_meshShader.setUniformValue("u_txt", 1);

    int vp = m_meshShader.attributeLocation("a_xyz");
    m_meshShader.enableAttributeArray(vp);
    m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(Types::VertData));

    int vc = m_meshShader.attributeLocation("a_rgb");
    m_meshShader.enableAttributeArray(vc);
    m_meshShader.setAttributeBuffer(vc, GL_UNSIGNED_BYTE, 3 * sizeof(float), 4, sizeof(Types::VertData));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_verts.release();
    m_meshShader.release();
}
QVector3D Backdrop::picking(const QVector3D& p, const QVector3D& n)const{return QVector3D();}
Types::Roi3d Backdrop::get3dbbox()const{return Types::Roi3d();}