#pragma once
#include <map>
#include <memory>
#include "drawable_base.h"

#define drawablesBuffer DrawablesBuffer::i()
class DrawablesBuffer
{

	using BaseTypeVector = std::map<size_t, std::unique_ptr<DrawableBase>>;
	BaseTypeVector m_drawobjs;
	size_t m_uniqueKeyCounter = 0;

	DrawablesBuffer();

public:
	DrawablesBuffer(DrawablesBuffer const &) = delete;
	void operator=(DrawablesBuffer const &) = delete;

	static DrawablesBuffer &i()
	{
		static DrawablesBuffer obj;
		return obj;
	}

	size_t addShape(const Types::Shape &obj, const std::string &name);
	bool removeShape(size_t key);

	bool updateVertexBuffer(size_t key,const Types::VertData* pcl,size_t n);

	BaseTypeVector::iterator	   begin();
	BaseTypeVector::const_iterator begin() const;
	BaseTypeVector::const_iterator cbegin() const;
	BaseTypeVector::iterator	   end();
	BaseTypeVector::const_iterator end() const;
	BaseTypeVector::const_iterator cend() const;

	void addDebug();
};
