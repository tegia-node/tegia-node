#ifndef H_CORE_FACET
#define H_CORE_FACET


// https://en.cppreference.com/w/cpp/locale/codecvt
// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert


template<class Facet>
struct deletable_facet : Facet
{
	template<class ...Args>
	deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
	~deletable_facet() {}
};


#endif