#ifndef _H_VK_API_ERROR_
#define _H_VK_API_ERROR_


#define _VK_HTTP_ERROR_     1
#define _VK_METHOD_ERROR_   2

namespace vk_api { 

class error
{
  public:
    error(unsigned int type, unsigned int code, nlohmann::json info)
    { 
       this->type = type;
       this->code = code;
       this->info = info;
    };
    virtual ~error(){};

 
 // тип ошибки. 
    // 1 - ошибка http
    // 2 - ошибка VK
    unsigned int type;
 // Код ошибки   
    unsigned int code;
 // текстовое описание ошибки
    nlohmann::json info;
};

} // end namespace vk_api



#endif 

