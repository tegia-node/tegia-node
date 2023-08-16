#ifndef H_ISS
#define H_ISS
// --------------------------------------------------------------------

// STL

// Libs

// Tegia headers
   #include <tegia/core/core.h>



namespace ISS {
	//int getRoleInCase(int user_id, int case_id);
	//int getRoleInSpace(int user_id, int space_id);

	template <class T>
	bool CheckAccess(T* obj, const std::string &access)
	{
		return true;
	}

};

#endif