#ifndef _CoreExt_CoreExt_h_
#define _CoreExt_CoreExt_h_

#include <type_traits>
#include <cstdint>
#include <limits>
#include <concepts>
#include <utility>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <cassert>
#include <Core/Core.h>

// if you ever need to #include <CtrCore/CtrlCore.h>, do it before include this one
// to avoid collision
//
#ifdef THISFN
#	undef THISFN
#	define THISFN(memfun) std::bind_front(&std::rm_cvref<decltype(*this)>::memfun, this)
#endif


namespace lz{
#include "bits/IntOfSize.hpp"
//#include "bit.h"

template <class T>
struct ptr_as_int{
	lz::ptrint p;
	ptr_as_int()=default;
	ptr_as_int(const ptr_as_int&)=default;
	ptr_as_int(T* ptr) : p(reinterpret_cast<lz::ptrint&>(*reinterpret_cast<lz::ptrint*>(&ptr))){}
	ptr_as_int(const T* ptr) :ptr_as_int(const_cast<T*>(ptr)){}
	operator T*(){ return reinterpret_cast<T*>(*reinterpret_cast<T**>(&p)); }
	operator const T*()const{ return reinterpret_cast<const T*>(*reinterpret_cast<const T**>(&p)); }
	ptr_as_int& operator ++(){ ++p; return *this; }
	ptr_as_int& operator --(){ --p; return *this; }
	ptr_as_int operator ++(int){ ptr_as_int t(*this); ++p; return t; }
	ptr_as_int operator --(int){ ptr_as_int t(*this); --p; return t; }
	ptr_as_int& operator +=(int i){ p += i; return *this; }
	ptr_as_int& operator -=(int i){ p -= i; return *this; }
	ptr_as_int operator +(int  i){ ptr_as_int t(*this); t+=i; return t; }
	ptr_as_int operator -(int  i){ ptr_as_int t(*this); t-=i; return t; }
	ptr_as_int& operator &=(lz::ptrint v){ p &= v; return *this; }
	ptr_as_int& operator |=(lz::ptrint v){ p |= v; return *this; }
	lz::ptrint operator | (lz::ptrint v){ return p | v; }
	lz::ptrint operator & (lz::ptrint v){ return p & v; }
};

namespace _detl{
	struct S{
		int i;
		virtual ~S();
	};
}

// detect and remember if the current c++ compiler put
// vptr for a virtual class who has no parent at the beginning or the end of 
// the class object.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
inline constexpr bool vptr_at_beginning = offsetof(_detl::S, i)==0;
#pragma GCC diagnostic pop

#include "bits/vector.hpp"

// note: only checked against GCC version of std::string
#include "bits/basic_string_relocate.hpp"
}//eons

#endif
