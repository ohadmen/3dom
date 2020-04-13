#include <locale>
#include <algorithm>

#include "drawables_buffer.h"
#include "zview/gui/drawables/drawable_mesh.h"

DrawablesBuffer::DrawablesBuffer()
{
}

void DrawablesBuffer::addShape(const Types::Shape &objv, const std::string &name)
{
    if (std::holds_alternative<Types::Mesh>(objv))
    {
        // const Types::Mesh &obj = std::get<Types::Mesh>(objv);
        // std::unique_ptr<DrawableMesh> dobjP(new DrawableMesh(name));
        // dobjP.get()->v().resize(obj.v().size());
        // std::copy(obj.v().begin(), obj.v().end(), dobjP.get()->v().begin());
        // dobjP.get()->f().resize(obj.f().size());
        // std::copy(obj.f().begin(), obj.f().end(), dobjP.get()->f().begin());
        // m_drawobjs[m_uniqueKeyCounter] = std::move(dobjP);
    }
    else
    {
        throw std::runtime_error("not implemnted yet");
    }

    ++m_uniqueKeyCounter;
}

// DrawablesBuffer::BaseTypeVector::iterator DrawablesBuffer::begin() { return m_drawobjs.begin(); }
// DrawablesBuffer::BaseTypeVector::const_iterator DrawablesBuffer::begin() const { return m_drawobjs.begin(); }
// DrawablesBuffer::BaseTypeVector::const_iterator DrawablesBuffer::cbegin() const { return begin(); }
// DrawablesBuffer::BaseTypeVector::iterator DrawablesBuffer::end() { return m_drawobjs.end(); }
// DrawablesBuffer::BaseTypeVector::const_iterator DrawablesBuffer::end() const { return m_drawobjs.end(); }
// DrawablesBuffer::BaseTypeVector::const_iterator DrawablesBuffer::cend() const { return end(); }
