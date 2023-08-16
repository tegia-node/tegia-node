#ifndef H_DAO_PROPERTY
#define H_DAO_PROPERTY

/** ****************************************************************************************

   \addtogroup   PROPERTY Модуль Property    
   \ingroup DAO
   \brief        Данный модуль реализует основные функции работы со свойствами. Реализован по мотивам 
                 статьи: http://habrahabr.ru/post/121799/

*/
///@{


   #include <string>
   #include <functional>
   //#include "../../cstring.h"
   #include <vector>

   //#include "../mysql/dao/dao_object.h"              




template<class T>
class Prop
{
    public:

    T *_val;   

    template<class C>
     Prop(std::string const &name, C* obj)
     {
       this->_val = new T;
         obj->test.insert(std::make_pair(name, std::make_pair(this->_val,typeid(*this->_val).name())));
     };

    // GETTER
    virtual T operator() (void) const 
    {
        return *(this->_val);
    };

    // SETTER
    virtual T operator() (const T &value)
    {
       *(this->_val) = value;
         return *(this->_val);
    };
};


/** ****************************************************************************************

   \brief        Класс абстрактного свойства

   Класс реализует основные функции работы со свойствами. Реализован по мотивам статьи:
   http://habrahabr.ru/post/121799/
   
*/
   class abstractProperty2
   {
      public: 
         virtual ~abstractProperty2() { };
   };


   template<class T> class Property2: public abstractProperty2
   {
      //friend class dao::cAccess;
    protected:

      T * value;
      std::string name;


    public:
      template<class P>
      Property2(std::string const &name, P *that, T *val, const T &value, std::function<T(void)> getter = nullptr, std::function<T(T)> setter = nullptr)
      {
         this->name   = name;
         this->value  = val; //сохраняем ссылку на поле

         //auto fn = std::bind(this, that);
         //that->access->add2(name, this);

         //std::cout << "typeid(fn).name() = " << typeid(fn).name() << std::endl;

         if(getter == nullptr)
         {
            this->getter = std::bind(&Property2<T>::_getter, this);
         }
         else
         {
            this->getter = getter;
         }

         if(setter == nullptr)
         {
            this->setter = std::bind(&Property2<T>::_setter, this, std::placeholders::_1);
         }
         else
         {
            this->setter = setter;
         }
         
         (this->setter)(value);
         
      };

      Property2()
      {
         this->name   = "";
         this->value  = nullptr; //сохраняем ссылку на поле
         this->getter = nullptr;
         this->setter = nullptr;
      };

      ~Property2()
      {
         //delete 
      };



      void init(std::string const &name, T * val, std::function<T(void)> getter = nullptr, std::function<T(T)> setter = nullptr)
      {         
         this->name   = name;
         this->value  = val; //сохраняем ссылку на поле
         if(getter == nullptr)
         {
            this->getter = std::bind(&Property2<T>::_getter, this);
         }
         else
         {
            this->getter = getter;
         }

         if(setter == nullptr)
         {
            this->setter = std::bind(&Property2<T>::_setter, this, std::placeholders::_1);
         }
         else
         {
            this->setter = setter;
         }
      };


      
      // GETTER
      virtual T operator() (void) const 
      {
         return (this->getter)();
      };

      // SETTER
      virtual T operator() (const T &value)
      {
         return (this->setter)(value);
      };





      std::function<T(void)> getter;
      std::function<T(T)>    setter;


      virtual T _getter() const
      {
         return *(this->value); 
      }; 

      virtual T _setter(const T &value) 
      {
         *(this->value) = value; 
         return *(this->value);
      };      
   };





   template<class T>
   std::ostream& operator<<(std::ostream &out, const Property2<T> &val)
   {
      out << val();
      return out;
   }

  
   
///@}
#endif   
