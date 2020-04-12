#pragma once
#include <map>
#include <memory>
#include "drawable_base.h"


#define drawablesBuffer DrawablesBuffer::i()
class DrawablesBuffer : public QObject
{
    Q_OBJECT
	typedef std::map <size_t,std::unique_ptr<DrawableBase>> BaseTypeVector;
	BaseTypeVector m_drawobjs;
	size_t m_uniqueKeyCounter=0;

	
public:
	DrawablesBuffer();


	void addShape(const Types::Shape& obj,const std::string& name);

    

	BaseTypeVector::iterator	   begin();
	BaseTypeVector::const_iterator begin() const;
	BaseTypeVector::const_iterator cbegin() const;
	BaseTypeVector::iterator	   end();
	BaseTypeVector::const_iterator end() const;
	BaseTypeVector::const_iterator cend() const;


    void addDebug();
  
};
