#include <locale>
#include <algorithm>

#include "drawables_buffer.h"
#include "zview/gui/drawables/drawable_mesh.h"
#include "zview/gui/drawables/drawable_edges.h"
#include "zview/gui/drawables/drawable_pcl.h"

DrawablesBuffer::DrawablesBuffer()
{
}

size_t DrawablesBuffer::size() const { return m_drawobjs.size(); }

struct Shape2drawable
{
    std::unique_ptr<DrawableBase> operator()(const Types::Pcl &obj)
    {
        std::unique_ptr<DrawablePcl> dobjP(new DrawablePcl(obj.getName()));
        dobjP.get()->v().resize(obj.v().size());
        std::copy(obj.v().begin(), obj.v().end(), dobjP.get()->v().begin());
        return dobjP;
    }
    std::unique_ptr<DrawableBase> operator()(const Types::Edges &obj)
    {
        std::unique_ptr<DrawableEdges> dobjP(new DrawableEdges(obj.getName()));
        dobjP.get()->v().resize(obj.v().size());
        std::copy(obj.v().begin(), obj.v().end(), dobjP.get()->v().begin());
        dobjP.get()->e().resize(obj.e().size());
        std::copy(obj.e().begin(), obj.e().end(), dobjP.get()->e().begin());
        return dobjP;
    }
    std::unique_ptr<DrawableBase> operator()(const Types::Mesh &obj)
    {
        std::unique_ptr<DrawableMesh> dobjP(new DrawableMesh(obj.getName()));
        dobjP.get()->v().resize(obj.v().size());
        std::copy(obj.v().begin(), obj.v().end(), dobjP.get()->v().begin());
        dobjP.get()->f().resize(obj.f().size());
        std::copy(obj.f().begin(), obj.f().end(), dobjP.get()->f().begin());
        return dobjP;
    }
};

size_t DrawablesBuffer::addShape(const Types::Shape &objv)
{

    m_drawobjs[m_uniqueKeyCounter] = std::visit(Shape2drawable(), objv);
    emit shapeAdded(QString::fromStdString(m_drawobjs[m_uniqueKeyCounter]->getName()), m_uniqueKeyCounter);
    emit updateCanvas();
    return m_uniqueKeyCounter++;
}

bool DrawablesBuffer::removeShape(size_t key)
{
    auto it = m_drawobjs.find(key);
    if (it == m_drawobjs.end())
        return false;
    m_drawobjs.erase(it);
    emit shapeRemoved(key);
    emit updateCanvas();
    return true;
}

bool DrawablesBuffer::updateVertexBuffer(size_t key, const Types::VertData *pcl, size_t n)
{
    auto it = m_drawobjs.find(key);
    if (it == m_drawobjs.end())
        return false;
    DrawableBase *obj = it->second.get();
    obj->updateVertexBuffer(pcl, n);
    return true;
}

DrawablesBuffer::BaseTypeVector::iterator DrawablesBuffer::begin() { return m_drawobjs.begin(); }
DrawablesBuffer::BaseTypeVector::const_iterator DrawablesBuffer::begin() const { return m_drawobjs.begin(); }
DrawablesBuffer::BaseTypeVector::const_iterator DrawablesBuffer::cbegin() const { return begin(); }
DrawablesBuffer::BaseTypeVector::iterator DrawablesBuffer::end() { return m_drawobjs.end(); }
DrawablesBuffer::BaseTypeVector::const_iterator DrawablesBuffer::end() const { return m_drawobjs.end(); }
DrawablesBuffer::BaseTypeVector::const_iterator DrawablesBuffer::cend() const { return end(); }
bool DrawablesBuffer::setShapeVisability(size_t key, bool isvis)
{
    auto it = m_drawobjs.find(key);
    if (it == m_drawobjs.end())
        return false;
    DrawableBase *obj = it->second.get();
    obj->setActive(isvis);
    emit updateCanvas();
    return true;
}
