#ifndef H_CORE_PERSON
#define H_CORE_PERSON

#include <iostream>
#include <tuple>
#include <codecvt>
#include <iconv.h>
#include <locale>

#include <tegia/const.h>
#include <tegia/core/core.h>
#include <tegia/core/cast.h>
#include <tegia/core/string.h>
#include <tegia/db/mysql/mysql.h>

/*

================================================

КОДЫ ВИДОВ ДОКУМЕНТОВ, УДОСТОВЕРЯЮЩИХ ЛИЧНОСТЬ НАЛОГОПЛАТЕЛЬЩИКА
 
01 Паспорт гражданина СССР
03 Свидетельство о рождении
05 Справка об освобождении из места лишения свободы
07 Военный билет
08 Временное удостоверение, выданное взамен военного билета
10 Паспорт иностранного гражданина
11 Свидетельство о рассмотрении ходатайства о признании лица беженцем на территории Российской Федерации по существу
12 Вид на жительство в Российской Федерации
13 Удостоверение беженца
14 Временное удостоверение личности гражданина Российской Федерации
15 Разрешение на временное проживание в Российской Федерации
18 Свидетельство о предоставлении временного убежища на территории Российской Федерации
21 Паспорт гражданина Российской Федерации
23 Свидетельство о рождении, выданное уполномоченным органом иностранного государства
24 Удостоверение личности военнослужащего Российской Федерации
26 Паспорт моряка
27 Военный билет офицера запаса
60 Документы, подтверждающие факт регистрации по месту жительства
61 Свидетельство о регистрации по месту жительства
62 Вид на жительство иностранного гражданина

================================================

*/


namespace tegia {
namespace person {

class names
{
	protected:
		static names * _self;
		void load();

		std::unordered_map<std::string,int> _names;

	public:
		names();
		~names();

		static names * instance();

		std::tuple<std::string,int> get(const std::string &name);
};

//
// Разбивает строку на одельные компоненты: <Фамилия, Имя, Отчество>
//


std::tuple<int,std::string,std::string,std::string> parse_fio(const std::string &fio);

//
// Приводит имя к формальному варианту написания (Первая строчная, потом прописные)
//

std::string normal_name(const std::string &name);

int check_sex(const std::string &patronymic);
int check_sex(const std::string &name, const std::string &patronymic);

}  // END namespace person
}  // END namespace tegia



#endif