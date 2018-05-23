// ********************************************************************
//	created:	2011/12/28
//	created:	28:12:2011   10:33
//	filename: 	C:\ALG_TEAM\COMMON\SOURCE\xPressRegCV\Array2d.h
//	author:		O. Menashe
//	
//	purpose:	
// *******************************************************************/

#pragma once
#pragma warning(disable:4996)//std::_Copy_impl': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'

#include <stdexcept>        // to get runtime_error
#include <iomanip>          //setw



template<class T>
class Array2d
{
    //data holder
    T*  m_data;
    int m_rows;
    int m_cols;

  


    void privReset()
    {
        //if(m_data!=nullptr && m_isDataOwner)
        //    delete[] m_data;
     m_rows = m_cols = 0;
    }

   
  


public:

    typedef T                               Pixel      ;


    ~Array2d ()  //d-tor
    {
        privReset();
    }

    Array2d ():m_rows(0),m_cols(0),m_data(){}                  // default c-tor
    Array2d (int r,int c):m_rows(r),m_cols(c),m_data(new T[r*c]){}
    Array2d (int r,int c, T initVal):m_rows(r),m_cols(c),m_data(new T[r*c])
    {
       
        for(int i=0;i!=m_rows*m_cols;++i)
            m_data[i] = initVal;
    }

   
   
    Array2d ( const Array2d& obj) 			       // copy c-tor
        :m_rows(obj.m_rows)
        ,m_cols(obj.m_cols)
        ,m_data(obj.m_data)
    {
        size_t sz = m_row*m_cols;
        m_data = new T[sz];
        std::copy(obj.m_data,obj.m_data+sz,m_data)
    }


    Array2d ( Array2d&& obj) 			       // move c-tor
        : m_cols(obj.m_cols)
        , m_rows(obj.m_rows)
   
        ,m_data(obj.m_data)
    {
        
        //kill
        obj.m_data = nullptr;
        obj.m_cols = obj.m_rows = 0;
    

    }



    Array2d& operator= ( const Array2d& other )  // assignment
    {  
        if (this != &other)
        {        // different objects ?
            this->~Array2d();                // -yes. Kill old content of target.
            std::allocator<Array2d>().construct(this ,other); // Run copy c-tor, using construct.
        }
        return *this;
    }


    Array2d& operator= ( Array2d&& other )  // move - assignment
    {
        if (this != &other)
        {
            //kill this obj
            privReset(); 
            // now copy m_data-ptr from obj and kill ptr inside of obj
            m_cols = other.m_cols;
            m_rows = other.m_rows;
            m_data = other.m_data;
            
            //kill
            other.m_data = nullptr;
            other.m_cols = other.m_rows = 0;
          
        }
        return *this;
    }

    T* operator[](int r)               {return m_data + r*m_cols;   }    
    const T* operator[](int r)   const {return m_data + r*m_cols;   }    


  
    int getHeight() const {return m_rows;}
    int getWidth() const {return m_cols;}

   
    const T* begin()const{return m_data;}
          T* begin()     {return m_data;}

    const T* end()const{return m_data+m_rows*m_cols;}
          T* end()     {return m_data+m_rows*m_cols;}


    friend std::ostream& operator << (std::ostream& s , const Array2d& obj) 
    {
        s << "[" << std::endl;
        for(int y=0;y!=obj.m_rows;++y)
        {
            s << " ";
            for(int x=0;x!=obj.m_cols;++x)
            {
                s << std::setw(13) << obj[y][x] <<",";
            }
            s  << std::endl;
        }
        s << "]" << std::endl;
        return s;
    }



};


