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
    static std::string getUniqueName(const std::string &name)
    {
        std::string suggestedName = name;
        int counter = 1;
        while (drawablesBuffer.exists(suggestedName))
        {
            suggestedName = name + "_" + std::to_string(counter++);
        }
        return suggestedName;
    }
    std::unique_ptr<DrawableBase> operator()(const Types::Pcl &obj)
    {
        std::unique_ptr<DrawablePcl> dobjP(new DrawablePcl(getUniqueName(obj.getName())));
        dobjP.get()->v().resize(obj.v().size());
        std::copy(obj.v().begin(), obj.v().end(), dobjP.get()->v().begin());
        return dobjP;
    }
    std::unique_ptr<DrawableBase> operator()(const Types::Edges &obj)
    {
        std::unique_ptr<DrawableEdges> dobjP(new DrawableEdges(getUniqueName(obj.getName())));
        dobjP.get()->v().resize(obj.v().size());
        std::copy(obj.v().begin(), obj.v().end(), dobjP.get()->v().begin());
        dobjP.get()->e().resize(obj.e().size());
        std::copy(obj.e().begin(), obj.e().end(), dobjP.get()->e().begin());
        return dobjP;
    }
    std::unique_ptr<DrawableBase> operator()(const Types::Mesh &obj)
    {
        std::unique_ptr<DrawableMesh> dobjP(new DrawableMesh(getUniqueName(obj.getName())));
        dobjP.get()->v().resize(obj.v().size());
        std::copy(obj.v().begin(), obj.v().end(), dobjP.get()->v().begin());
        dobjP.get()->f().resize(obj.f().size());
        std::copy(obj.f().begin(), obj.f().end(), dobjP.get()->f().begin());
        return dobjP;
    }
};
Types::Roi3d DrawablesBuffer::get3dbbox(int key)
{
    Types::Roi3d objsbbox;
    if (key == -1)
    {
        for (const auto &o : *this)
        {
            Types::Roi3d bbox = o.second.get()->get3dbbox();
            objsbbox |= bbox;
        }
    }
    else
    {
        auto it = m_drawobjs.find(size_t(key));
        if (it != m_drawobjs.end())
        {
            objsbbox = it->second.get()->get3dbbox();
        }
    }
    return objsbbox;
}
bool DrawablesBuffer::exists(const std::string &name) const
{
    for (const auto &a : m_drawobjs)
    {
        if (name == a.second->getName())
        {
            return true;
        }
    }
    return false;
}
size_t DrawablesBuffer::addShape(const Types::Shape &objv)
{

    m_drawobjs[m_uniqueKeyCounter] = std::visit(Shape2drawable(), objv);
    emit shapeAdded(QString::fromStdString(m_drawobjs[m_uniqueKeyCounter]->getName()), m_uniqueKeyCounter);
    emit updateCanvas();
    return m_uniqueKeyCounter++;
}

bool DrawablesBuffer::removeShape(int key)
{
    if (key == -1)
        return false;
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
