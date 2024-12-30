#ifndef _CoreExt_CoreExt_h_
#define _CoreExt_CoreExt_h_

#include <Core/Core.h>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>

#include <Core/Core.h>

#ifdef THISFN
#undef THISFN
#define THISFN(memfun) std::bind_front(&std::remove_cvref_t<decltype(*this)>::memfun, this)
#endif

#define BEGIN_NAMESPACE_LZ namespace lz{
#define END_NAMESPACE }

#define INOUT
#define OUT


#ifndef _CXX17_CONSTEXPR
# if __cplusplus >= 201703L
#  define _CXX17_CONSTEXPR constexpr
# else
#  define _CXX17_CONSTEXPR
# endif
#endif

#ifndef _CXX20_CONSTEXPR
# if __cplusplus >= 202002L
#  define _CXX20_CONSTEXPR constexpr
# else
#  define _CXX20_CONSTEXPR
# endif
#endif

#ifndef _CXX23_CONSTEXPR
# if __cplusplus >= 202100L
#  define _CXX23_CONSTEXPR constexpr
# else
#  define _CXX23_CONSTEXPR
# endif
#endif


BEGIN_NAMESPACE_LZ
#include "bits/using_upp_names.h"
#include "bits/IntOfSize.hpp"

enum class tribool : char {
	False, No		= False,	Off = False,
	True,  Yes		= True,		On  = Yes,
	Null,  Unknown  = Null
};

// a compile time
// const char * s = "Hello, world!";
// has program life time.
// sometimes we want to differentiate it from a const char * from
//    String s("Hello, Upp User!");
//    const char * t = s;
//
// This class serves this purpose.
//
// basically it's a way to tell program/functions that the StrLiteral
// associated 'const char *' can be guaranteed to be valid, and no copy/
// free is needed for readonly access.
//
class StrLiteral
{
public:
	constexpr StrLiteral(const StrLiteral& l) = default;
	explicit constexpr StrLiteral(const char * s):p(s){}
	
	constexpr operator const char *()const{ return p; }
	constexpr const char * get()const{ return p; }
	
private:
	const char * p = nullptr;
};


inline constexpr StrLiteral operator""_z(const char *str, std::size_t)
{
	return StrLiteral(str);
}

#define tz(s) StrLiteral(t_(s))
//#include "bit.h"

template <class T>
struct ptr_as_int {
    ptrint p;
    ptr_as_int() = default;
    ptr_as_int(const ptr_as_int&) = default;
    ptr_as_int(T* ptr)
        : p(reinterpret_cast<lz::ptrint&>(*reinterpret_cast<lz::ptrint*>(&ptr)))
    {
    }
    ptr_as_int(const T* ptr)
        : ptr_as_int(const_cast<T*>(ptr))
    {
    }
    operator T*() { return reinterpret_cast<T*>(*reinterpret_cast<T**>(&p)); }
    operator const T*() const { return reinterpret_cast<const T*>(*reinterpret_cast<const T**>(&p)); }
    ptr_as_int& operator++()
    {
        ++p;
        return *this;
    }
    ptr_as_int& operator--()
    {
        --p;
        return *this;
    }
    ptr_as_int operator++(int)
    {
        ptr_as_int t(*this);
        ++p;
        return t;
    }
    ptr_as_int operator--(int)
    {
        ptr_as_int t(*this);
        --p;
        return t;
    }
    ptr_as_int& operator+=(int i)
    {
        p += i;
        return *this;
    }
    ptr_as_int& operator-=(int i)
    {
        p -= i;
        return *this;
    }
    ptr_as_int operator+(int i)
    {
        ptr_as_int t(*this);
        t += i;
        return t;
    }
    ptr_as_int operator-(int i)
    {
        ptr_as_int t(*this);
        t -= i;
        return t;
    }
    ptr_as_int& operator&=(lz::ptrint v)
    {
        p &= v;
        return *this;
    }
    ptr_as_int& operator|=(lz::ptrint v)
    {
        p |= v;
        return *this;
    }
    lz::ptrint operator|(lz::ptrint v) { return p | v; }
    lz::ptrint operator&(lz::ptrint v) { return p & v; }
};

namespace _detail {
    struct S {
        int i;
        virtual ~S();
    };
}

// detect and remember if the current c++ compiler put
// vptr for a virtual class who has no parent at the beginning or the end of
// the class object.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
inline constexpr bool vptr_at_beginning = offsetof(_detail::S, i) == 0;
#pragma GCC diagnostic pop

#include "bits/vector.hpp"

// note: only checked against GCC version of std::string
#include "bits/basic_string_relocate.hpp"
END_NAMESPACE

template<>
inline bool Upp::IsNull(const lz::tribool& b)
{
	return b == lz::tribool::Null;
}


#endif
