// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

   #include "../Node2.h"
   #include <tegia/Actor/Actor2.h>



   Actor2::Actor2(const std::string &name, nlohmann::json &data)
   {  LOG_TRACE       

      //this->isWork = false;
      this->name = name;
      this->type = "Actor2";
   }; 


   /////////////////////////////////////////////////////////////////////////////////////////////
   Actor2::~Actor2()
   {  LOG_TRACE
      
      // TODO: При удалении актора теряют смысл все поставленные в очередь обработки сообщения. 
      //       Необходимо их корректно удалить из очереди
   };


   /////////////////////////////////////////////////////////////////////////////////////////////
   int Actor2::init(const nlohmann::json &data)
   {  LOG_TRACE
      //std::cout << "     " << this->name << " init" << std::endl;
      return 0;
   };


   bool Actor2::add_route(const std::string &route, std::function<std::string(const std::shared_ptr<message_t2>, const nlohmann::json &)> func)
   {
      std::cout << "add_route = " << route << std::endl;
      this->router.insert(std::make_pair(route,func));
      //this->router_list.push_back(route);
      return true; 
   };



   std::string Actor2::worker(const std::string &route, const std::shared_ptr<message_t2> &message, const nlohmann::json &route_params)
   {
      auto it = this->router.find(route);
      if(it != this->router.end())
      {
         return it->second(message,route_params);
      }
      return " ";
   };


/**
  \brief     

*/
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::string Actor2::defmsg(const std::shared_ptr<message_t2> &message, const nlohmann::json &route_params)
    {  LOG_TRACE

       auto node = Platform::Node::instance();

       LDEBUG("[" << this->type << "::" << this->name << "] message (" << message->action << ") = " << message->data.dump() );

       return "ok";
    };



/**
  \brief     

*/
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::string Actor2::rexit(const std::shared_ptr<message_t2> &message, const nlohmann::json &route_params)
    {  LOG_TRACE

       LDEBUG("[" << this->type << "::" << this->name << "] message (" << message->action << ") = " << message->data.dump() );

       return "ok";
    };


/**
  \brief   Получение имени актора   
  \return Строка с именем актора
*/

    /////////////////////////////////////////////////////////////////////////////////////////////
    std::string Actor2::getName()
    {  LOG_TRACE
       return this->name;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    std::string Actor2::getType()
    {  LOG_TRACE
       return this->type;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    int Actor2::sendMessage(const std::string & actor, 
                            const std::shared_ptr<message_t2> message,
                            int priority)
    {
       Platform::Node::instance()->SendMessage(actor, message, priority);
       return 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    int Actor2::sendMessage(const std::string & actor, 
                            nlohmann::json &data, 
                            int priority)
    {
       std::shared_ptr<message_t2> message(new message_t2("", "", data ));
       Platform::Node::instance()->SendMessage(actor, message, priority);      
       return 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    void Actor2::createActor(const std::string & type, 
                             const std::string & name, 
                             nlohmann::json &init)
    {
       Platform::Node::instance()->createActor(type,name,init); 
    }


    std::shared_ptr<message_t2> Actor2::callActor(const std::string & actor, nlohmann::json &data)
    {
       std::shared_ptr<message_t2> message(new message_t2("", "", data ));
       Platform::Node::instance()->SendMessageS(actor, message);
       return message;
    };


