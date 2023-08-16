#include <tegia/db/mysql/mysql_records.h>
#include "mysql_record_info.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                               //
// MYSQL RECORDS CLASS                                                                           //
//                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia { 
namespace mysql {

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   Конструктор класса
	\authors Горячев Игорь 
	\details 
		Поскольку создавать объект record разумно только из результатов выполненного SQL-запроса, то 
		конструктор размещен в защищенной области видимости. Это гарантирует, что объект record будет 
		создан корректно только в классе db_connect.
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

records::records(
	const std::string& query, 
	const int& last_insert_id): 
	query(query), 
	_last_insert_id(last_insert_id),
	code(0),
	info("")
{
	
};

records::~records()
{
	for(auto it = this->fields_offset.begin(); it != this->fields_offset.end(); it++)
	{
		delete it->second;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   
	\authors Горячев Игорь 
	\details 
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void records::setRecNum(int recNum)
{
	this->rec_num = recNum;
	this->recs = std::vector<std::unordered_map<std::string, std::string> >(this->rec_num);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   Добавление информации о поле данных
	\authors Горячев Игорь 
	\details 
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void records::add_field(MYSQL_FIELD * field)
{
	std::string s_table = "";
	if(field->table != NULL)
	{
		s_table = std::string(field->table);
	}

	std::string s_def = "";
	if(field->def != NULL)
	{
		s_def = std::string(field->def);
	}

	std::string fname = std::string(field->table) + "::" + std::string(field->name);
	
	record_info *tfield = new record_info(
		std::string(field->name), 
		s_table,
		s_def,
		field->type,
		field->length,
		field->max_length,
		field->flags,
		field->decimals);

	this->fields_name.insert(std::pair<std::string,record_info*>( fname, tfield ) );
	this->fields_offset.insert(std::pair<int,record_info*>(this->fields_num, tfield) ); 
	this->fields_num++;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   Добавление информации о строке результата
	\authors Горячев Игорь 
	\details 
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void records::add_row(const MYSQL_ROW &row, const int &offset)
{
	int i = 0;
	for(auto it = this->fields_offset.begin(); it != this->fields_offset.end(); it++)
	{
		std::string name = it->second->table + "::" + it->second->name;
		if(row[i] == nullptr)
		{
			this->recs[offset][name] = "";
		}
		else
		{
			this->recs[offset][name] = row[i];
		}
		i++;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   Получение значение поля данных по имени и позиции в результатах запроса
	\authors Горячев Игорь
	\details 
		key = "table_name::field_name"
*/  
///////////////////////////////////////////////////////////////////////////////////////////////////

std::string records::get(const std::string &key, const int offset)
{
	if (offset < this->rec_num)
	{
		//  Ищем ключ
		auto find_key = this->recs[offset].find(key);

		if ( find_key != end(this->recs[offset]))
		{
			return find_key->second;
		}
		else
		{
			return "";
		}
	}
	else
	{
		return "";
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   
	\authors Горячев Игорь 
	\details 
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json records::json()
{
	nlohmann::json root;

	if (this->rec_num == 0)
	{
		return root;          
	}

	for(int i = 0; i < this->rec_num; i++)
	{
		nlohmann::json record;
		for(auto it = this->recs.at(i).begin(); it != this->recs.at(i).end(); it++)
		{
			std::string field_name = it->first.substr( it->first.find("::") + 2);
			record[field_name] = it->second;
		}

		root.push_back(record);
	}

	return root;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   Возвращает число строк, полученных из MySQL
	\authors Горячев Игорь
	\details 
*/  
///////////////////////////////////////////////////////////////////////////////////////////////////

int records::count()
{
	return rec_num;
};
			
///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   
	\authors Горячев Игорь 
	\details 
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

int records::getLastInsertId() const
{
	return _last_insert_id;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief   
	\authors Горячев Игорь 
	\details 
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

int records::get_affected_rows() const
{
	return this->_affected_rows;
};


}   // end namespace mysql
}   // end namespace tegia
