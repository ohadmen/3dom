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
	using BaseTypeVector = std::map<size_t, std::unique_ptr<DrawableBase>>;
	size_t size() const;
	DrawablesBuffer(DrawablesBuffer const &) = delete;
	void operator=(DrawablesBuffer const &) = delete;

	static DrawablesBuffer &i()
	{
		static DrawablesBuffer obj;
		return obj;
	}

	size_t addShape(const Types::Shape &obj);
	bool removeShape(size_t key);

	bool updateVertexBuffer(size_t key, const Types::VertData *pcl, size_t n);

	BaseTypeVector::iterator begin();
	BaseTypeVector::const_iterator begin() const;
	BaseTypeVector::const_iterator cbegin() const;
	BaseTypeVector::iterator end();
	BaseTypeVector::const_iterator end() const;
	BaseTypeVector::const_iterator cend() const;

	void addDebug();
public slots:
	bool setShapeVisability(size_t key, bool isvis);
signals:
	void shapeAdded(QString name, size_t key);
	void shapeRemoved(size_t key);
	void updateCanvas();

private:
	BaseTypeVector m_drawobjs;
	size_t m_uniqueKeyCounter = 0;

	DrawablesBuffer();
};
