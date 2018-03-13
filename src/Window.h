#pragma 
#include <QMainWindow>
#include <QtCore\qmimedata.h>
#include "Canvas.h"
class Window : public QMainWindow
{
	Q_OBJECT
      
private:
	Canvas m_canvas;
    const QObject* m_parent;
public:
	Canvas & canvas() { return m_canvas; }
	explicit Window(QObject *parent = 0):m_parent(parent)
    {
    }
    void loadMeshFromFile(const QString& meshfn)
    {
        int token = Loader::i().load(meshfn);
        m_canvas.setToken(token);
        m_canvas.cam2geometry();
    }
protected:
    void dragEnterEvent(QDragEnterEvent *event)
    {
        
        if (event->mimeData()->hasUrls())
        {
            auto urls = event->mimeData()->urls();
            if (urls.size() == 1 && urls.front().path().endsWith(".stl"))
                event->acceptProposedAction();
        }
    }
    void dropEvent(QDropEvent *event)
    {
        QString meshfn = event->mimeData()->urls().front().toLocalFile();
        loadMeshFromFile(meshfn);

    }
   
};