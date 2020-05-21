#include <locale>
#include <algorithm>

#include "drawables_buffer.h"
#include "zview/gui/drawables/drawable_mesh.h"
#include "zview/gui/drawables/drawable_edges.h"
#include "zview/gui/drawables/drawable_pcl.h"

std::string getUniqueName(const std::string &name)
{
    std::string suggestedName = name;
    int counter = 1;
    while (drawablesBuffer.exists(suggestedName))
    {
        suggestedName = name + "_" + std::to_string(counter++);
    }
    return suggestedName;
}

DrawablesBuffer::DrawablesBuffer()
{
}



size_t DrawablesBuffer::size() const { return m_drawobjs.size(); }

struct Shape2drawable
{
    std::unique_ptr<DrawableBase> operator()(Types::Pcl&& obj)
    {
        std::unique_ptr<DrawablePcl> dobjP(new DrawablePcl(getUniqueName(obj.getName())));
        dobjP.get()->v() = std::move(obj.v());
        return dobjP;
    }
    std::unique_ptr<DrawableBase> operator()(Types::Edges&& obj)
    {
        std::unique_ptr<DrawableEdges> dobjP(new DrawableEdges(getUniqueName(obj.getName())));
        dobjP.get()->v() = std::move(obj.v());
        dobjP.get()->e() = std::move(obj.e());
        return dobjP;
    }
    std::unique_ptr<DrawableBase> operator()(Types::Mesh && obj)
    {
        std::unique_ptr<DrawableMesh> dobjP(new DrawableMesh(getUniqueName(obj.getName())));
        dobjP.get()->v() = std::move(obj.v());
        dobjP.get()->f() = std::move(obj.f());
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
qint64 DrawablesBuffer::addShape(Types::Shape&& objv)
{
    m_drawobjs[m_uniqueKeyCounter] = std::visit(Shape2drawable(), std::move(objv));
    emit shapeAdded(QString::fromStdString(m_drawobjs[m_uniqueKeyCounter]->getName()), m_uniqueKeyCounter);
    emit updateCanvas();
    return m_uniqueKeyCounter++;
    
}
qint64 DrawablesBuffer::addShape(const Types::Shape &objv)
{
    Types::Shape objvCopy = objv;
    return addShape(std::move(objvCopy));
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
bool DrawablesBuffer::setShapeVisability(qint64 key, bool isvis)
{
    auto it = m_drawobjs.find(key);
    if (it == m_drawobjs.end())
        return false;
    DrawableBase *obj = it->second.get();
    obj->setActive(isvis);
    emit updateCanvas();
    return true;
}
