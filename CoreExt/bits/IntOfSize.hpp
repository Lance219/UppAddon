namespace _detail{
	template<unsigned>struct IntOfSize_;

	template<unsigned>struct UIntOfSize_;

	template<>struct  IntOfSize_<1>
	{
		typedef   int8_t type;
	};

	template<>struct  IntOfSize_<2>
	{
		typedef  int16_t type;
	};

	template<>struct  IntOfSize_<4>
	{
		typedef  int32_t type;
	};

	template<>struct  IntOfSize_<8>
	{
		typedef  int64_t type;
	};

	template<>struct UIntOfSize_<1>
	{
		typedef  uint8_t type;
	};

	template<>struct UIntOfSize_<2>
	{
		typedef uint16_t type;
	};

	template<>struct UIntOfSize_<4>
	{
		typedef uint32_t type;
	};

	template<>struct UIntOfSize_<8>
	{
		typedef uint64_t type;
	};
}//eons _

template <unsigned t>
using IntOfSize = typename _detail::IntOfSize_<t>::type;

template <unsigned t>
using UIntOfSize = typename _detail::UIntOfSize_<t>::type;


using ptrint = IntOfSize< sizeof(void*) >;
using ptruint = UIntOfSize< sizeof(void*) >;
using boolint = IntOfSize< sizeof(bool) >;

// merge different integral types of the same size to one.
// e.g. long and int may both be of 32 bits,
// in some case we only care the actual size and signedness of
// an integral type
//
namespace _detail{
	template <typename T>
	struct TrimIntegral{
		typedef T type;
	};
	template <>
	struct TrimIntegral<bool>{
		typedef bool type;
	};
	
	template <std::signed_integral T>
	struct TrimIntegral<T>{
		typedef lz::IntOfSize<sizeof(T)> type;
	};

	template <std::unsigned_integral T>
	struct TrimIntegral<T>{
		typedef lz::UIntOfSize<sizeof(T)> type;
	};
}

template <typename T>
using TrimIntegral = typename _detail::TrimIntegral<T>::type;

template <class T>
IntOfSize<sizeof(T)>& AsInt(T& a)
{
	return *reinterpret_cast<IntOfSize<sizeof(T)> *>(&a);
}

template <class T>
UIntOfSize<sizeof(T)>& AsUInt(T& a)
{
	return *reinterpret_cast<UIntOfSize<sizeof(T)> *>(&a);
}

template <class T>
const IntOfSize<sizeof(T)> AsInt(const T& a)
{
	return *reinterpret_cast<const IntOfSize<sizeof(T)> *>(&a);
}

template <class T>
const UIntOfSize<sizeof(T)> AsUInt(const T& a)
{
	return *reinterpret_cast<const UIntOfSize<sizeof(T)> *>(&a);
}

