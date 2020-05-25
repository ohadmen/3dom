#pragma once
#include <map>
#include <memory>
#include "drawable_base.h"
#include <QtCore/QString>

#define drawablesBuffer DrawablesBuffer::i()
class DrawablesBuffer : public QObject
{
	Q_OBJECT
public:
	static DrawablesBuffer& i()
	{
		static DrawablesBuffer obj;
		return obj;
	}
	using BaseTypeVector = std::map<qint64, std::unique_ptr<DrawableBase>>;
	size_t size() const;
	DrawablesBuffer(DrawablesBuffer const &) = delete;
	void operator=(DrawablesBuffer const &) = delete;

	qint64 addShape(Types::Shape&& objv);
	qint64 addShape(const Types::Shape &obj);
	bool removeShape(qint64 key);

	bool updateVertexBuffer(size_t key, const Types::VertData *pcl, size_t n);

	BaseTypeVector::iterator begin();
	BaseTypeVector::const_iterator begin() const;
	BaseTypeVector::const_iterator cbegin() const;
	BaseTypeVector::iterator end();
	BaseTypeVector::const_iterator end() const;
	BaseTypeVector::const_iterator cend() const;

	Types::Roi3d get3dbbox(int handleNum=-1);
	bool exists(const std::string& name) const;
public slots:
	bool setShapeVisability(qint64 key, bool isvis);
signals:
	void signal_shapeAdded(QString name, qint64 key);
	void signal_shapeRemoved(qint64 key);
	void signal_updateCanvas();

private:
	BaseTypeVector m_drawobjs;
	size_t m_uniqueKeyCounter = 0;

	DrawablesBuffer();
};
