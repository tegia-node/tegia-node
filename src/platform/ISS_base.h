#ifndef H_ISS_BASE
#define H_ISS_BASE


#include "core.h"
#include "ISS.h"
#include "db2/mysql/dao/access.h"

//#include "Node.h"
//#include "threads/thread.h"

#include "Node2.h"
#include "db2/account/DUser.h"


namespace ISS {

  //  ---------------------------------------
  //  Возвращает роль пользователя с ид user_id в кейсе c ид case_id
  int getRoleInCase(int user_id, int case_id)
  {
    int role_id = 0;
    string query = st::format() << "SELECT `space_users`.`space_role_id` FROM `cases` JOIN `space_users` ON `cases`.`id_space` = `space_users`.`space_id` \
                      WHERE     `cases`.`_gid` = " << to_string(case_id) << 
                      " AND `space_users`.`_user_id` = " << to_string(user_id);

    //auto pool = POOL("platform");

    auto ret = tegia::mysql::query("platform",query);
    if (ret->getRecNum() == 1)
    {
      LDEBUG("ISS::getRoleInCase role_id: " + ret->get("space_users::space_role_id", 0) );
      role_id = core::cast<int>(ret->get("space_users::space_role_id", 0));
      LDEBUG(to_string(role_id));
    }
    delete ret;

    return role_id;
  }



  //  ---------------------------------------
  //  Возвращает класс кейса по его _gid
  std::string getCaseClass(int _gid)
  {
     string query = st::format() << "SELECT `cases`.`class` FROM `cases` WHERE `cases`.`_gid` = " << to_string(_gid);    
     auto res = tegia::mysql::query("platform",query);
     if(res->count() == 0)
     {
        return res->get("cases::class",1);
     }
     else
     {
        return "@noclass";
     }

     delete res;
  }


};

#endif