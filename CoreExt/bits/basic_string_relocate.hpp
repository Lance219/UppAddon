#include <string>

// std::basic_string (possibly) has a pointer to relative to *this;
// make it workable with lz::vector

template <typename chT>
void DoPostRelocationAdjustment(
	std::basic_string<chT>* obj,
	const std::basic_string<chT>* old
)
{
	// we understand the first member of *this is the pointer to
	// data, but let's make sure.
	//
#ifdef _DEBUG
	std::string s("test");
	ASSERT( *reinterpret_cast<lz::ptrint*>(&s) == lz::AsInt(s.data()) );
#endif

	if(	lz::AsInt( obj->data() ) == lz::AsInt( old ) + 16 )
		*reinterpret_cast<lz::ptrint*>(obj) = lz::AsInt( obj ) + 16;
}

template <typename chT>
struct relocate_traits< std::basic_string<chT> >{
	using T = std::basic_string<chT>;
	static constexpr int value = 2; // old address is supplied.
	static void Do(T* obj, const T* old)noexcept{
		DoPostRelocationAdjustment(obj, old);
	}
};

//};
//
