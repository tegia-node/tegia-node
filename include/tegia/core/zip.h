#ifndef H_TEGIA_CORE_ZIP
#define H_TEGIA_CORE_ZIP
// --------------------------------------------------------------------

// C++ STL
#include <fstream>
#include <filesystem>
#include <iostream>
#include <functional>
#include <format>

// VENDORS 
#include <zip.h>


namespace tegia {

class zip 
{
	public:
		void iterator(const std::string &filename, std::function<int(const std::string &, const std::string &)> _fn);

	private:
		std::string filename;

		zip_t *zip_file; // дескриптор zip файла
		int err; // переменая для возврата кодов ошибок
		int files_total; // количество файлов в архиве

		struct zip_stat file_info; // информация о файле
		struct zip_file *file_in_zip; // дексриптор файла внутри архива
		struct tm file_time;

		int file_number;
		int r;
		char buffer[10000];


}; // class zip


//
//
//


void zip::iterator(const std::string &filename, std::function<int(const std::string &, const std::string &)> _fn)
{
	// открываем файл zip с именем переданным в качестве параметра
	this->zip_file = zip_open(filename.c_str(), 0, &err);
	if (!zip_file) {

		fprintf(stderr,"Error: can't open file %s\n",filename.c_str());
		return; // "error";
	}

	files_total = zip_get_num_files(this->zip_file); // количество файлов в архиве

	//
	// Читаем файлы 
	//			

	for (int i = 0; i < files_total; i++) 
	{
		// открываем файл внутри архива по номеру file_number
		std::string filedata = "";
		file_in_zip = zip_fopen_index(zip_file, i, 0);
		if (file_in_zip) 
		{
			zip_stat_index(zip_file, i, 0, &file_info);
			// читаем в цикле содержимое файла и выводим
			while ( (r = zip_fread(file_in_zip, buffer, sizeof(buffer))) > 0) 
			{
				//printf("%s",buffer);
				filedata.append(buffer,r);
			};

			_fn(std::string(this->file_info.name),filedata);

			// закрываем файл внутри архива
			zip_fclose(file_in_zip);
		} 
		else 
		{
			fprintf(stderr,"Error: can't open file %d in zip\n",i);
			exit(0);
		};
	}

	zip_close(this->zip_file);
};


} // END namespace tegia






#endif