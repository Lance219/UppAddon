using String = Upp::String;

void SetBit ( void * start, size_t offset, bool v );
bool GetBit ( const void * start, size_t offset );
void SetBit ( void * start, size_t offset );
void UnsetBit ( void * start, size_t offset );
inline void ClearBit ( void * start, size_t offset )
{
	UnsetBit ( start, offset );
}

bool ParseBool ( const char * input, bool& output );

Upp::String sql_quote ( const char * s );
Upp::String sql_like ( const char *s );
Upp::String SqlQuoteIdentifier ( const char *s );

class bit;
// blob is moveable, but no need to let the world know.
//
// I never had a chance to use Blob yet, so it's not tested and might not work at all.

class Blob
{

	public:
		Blob() : p ( NULL ) {}

		~Blob()
		{
			delete[] ( char* ) p;
		}

		Blob ( Blob && b )
		{
			*this = std::move ( b );
		}

		Blob ( const Blob& b )
				: p ( ( rblob* ) new char[b.p->len+sizeof ( p->len ) ] )
		{
			memcpy ( p, b.p, b.p->len + sizeof ( p->len ) );
		}

		Blob& operator= ( Blob && b )
		{
			std::swap ( p, b.p );
			return *this;
		}

		Blob& operator= ( const Blob& b )
		{
			if ( this != &b )
			{
				this->~Blob();
				new ( this ) Blob ( b );
			}

			return *this;
		}

		size_t Length() const
		{
			return p ? p->len : 0;
		}

		char * Bytes()
		{
			return p->data;
		}

		const char * Bytes() const
		{
			return p->data;
		}

	private:

		struct rblob
		{
			size_t len;
			char data[1];
		};

		rblob * p;
};

// for a field, we want to know:
// 1. what we need to do after it's moved to a new location?
// 2. how to destruct it?
// 3. how to copy it?
//
// for PODs, all above are simple and in the same way

enum FieldTypeId:
unsigned char
{
	//  !!!!!DO NOT CHANGE ORDER OR ADD ENTRY OR SET VALUE FOR ENTRIES BELOW!!!
	FT_BIT = 0,
	FT_BOOL = 1, //1
	FT_INT8 = 2, //2 char
	FT_INT16 = 3, //3
	FT_INT32 = 4, //4
	FT_INT64 = 5,
	FT_UINT8 = 6, //unsigned char
	FT_UINT16 = 7, //7
	FT_UINT32 = 8,
	FT_UINT64 = 9,
	FT_FLOAT = 10, //10
	FT_DOUBLE = 11,
	FT_STRING = 12,
	FT_DATE = 13,   // Upp::Date
	FT_TIME = 14,	// Upp::Time
	FT_BLOB = 15,
	FT_VALUE = 16, //u++ value
	FT_PCHAR, // char *
	// !!!! DO NOT CHANGE ORDER OR ADD ENTRY OR SET VALUE FOR ABOVE ENTRIES!!!
	FT_UDTSTART, // user defined type start

	FT_UNKNOWN = 255
};


// run-time type information
int TypeAlign ( FieldTypeId fid );
int TypeSize ( FieldTypeId fid );
const char* TypeName ( FieldTypeId fid );
//bool IsPOD(FieldTypeId fid);


// compile time type-to-id conversion
//
template <typename T>

struct Type2Id
{
	typedef const T& return_type;
	static constexpr FieldTypeId id = FT_UNKNOWN;
	//static const unsigned align=4; // default 4 bytes align
};


template <>

struct Type2Id<bit>
{
	typedef bool return_type;
	static constexpr FieldTypeId id = FT_BIT;
	static constexpr unsigned align = 0; // special case, not strictly necessary
	static constexpr unsigned size = 0;
};

#define STDT2IDENTRY(t, tid) template<>struct Type2Id<t>{\
	typedef t return_type;\
	static constexpr FieldTypeId id=FT_##tid;\
	static constexpr size_t align=alignof(t);\
	static constexpr size_t size=sizeof(t);\
}
STDT2IDENTRY ( bool,     BOOL );
STDT2IDENTRY ( std::int8_t,    INT8 );
STDT2IDENTRY ( std::uint8_t,   UINT8 );
STDT2IDENTRY ( std::int16_t,   INT16 );
STDT2IDENTRY ( std::uint16_t,  UINT16 );
STDT2IDENTRY ( std::int32_t,   INT32 );
STDT2IDENTRY ( std::uint32_t,  UINT32 );
STDT2IDENTRY ( std::int64_t,   INT64 );
STDT2IDENTRY ( std::uint64_t,  UINT64 );
STDT2IDENTRY ( float,   FLOAT );
STDT2IDENTRY ( double, DOUBLE );
STDT2IDENTRY ( String, STRING );
STDT2IDENTRY ( Date,     DATE );
STDT2IDENTRY ( Time,     TIME );
STDT2IDENTRY ( Blob,     BLOB );
STDT2IDENTRY ( Value,     VALUE );
#undef STDT2IDENTRY


// Id-to-type conversion

template <FieldTypeId>struct Id2Type;

#define Id2TypeEntry(Id, Type) template<>\
struct Id2Type<Id> \
{\
	typedef Type type;\
}

Id2TypeEntry ( FT_BIT,       bit );
Id2TypeEntry ( FT_BOOL,     bool );
Id2TypeEntry ( FT_INT8,     std::int8_t );
Id2TypeEntry ( FT_INT16,    std::int16_t );
Id2TypeEntry ( FT_INT32,    std::int32_t );
Id2TypeEntry ( FT_INT64,    std::int64_t );
Id2TypeEntry ( FT_UINT8,   std::uint8_t );
Id2TypeEntry ( FT_UINT16,  std::uint16_t );
Id2TypeEntry ( FT_UINT32,  std::uint32_t );
Id2TypeEntry ( FT_UINT64,  std::uint64_t );
Id2TypeEntry ( FT_FLOAT,   float );
Id2TypeEntry ( FT_DOUBLE, double );
Id2TypeEntry ( FT_STRING,		String );
Id2TypeEntry ( FT_DATE,		Date );
Id2TypeEntry ( FT_TIME,     Time );
Id2TypeEntry ( FT_BLOB,     Blob );
Id2TypeEntry ( FT_VALUE,     Value );

struct TypeInfo
{
#ifdef _DEBUG
	FieldTypeId id;
#endif
	const char *name;
	int16 size;
	int16 align;
};

#ifdef _DEBUG
#define L_ENTRY(name, id, Type) {id, name, Type2Id<Id2Type<id>::type>::size,\
	Type2Id<Id2Type<id>::type>::align}
#else
#define L_ENTRY(name, id,  Type) {name, Type2Id<Id2Type<id>::type>::size, \
	Type2Id<Id2Type<id>::type>::align}
#endif

inline static const TypeInfo basic_types[] =
{
//	FT_BIT = 0,
//	FT_BOOL = 1, //1
//	FT_INT8 = 2, //2 char
//	FT_INT16 = 3, //3
//	FT_INT32 = 4, //4
//	FT_INT64 = 5,
//	FT_UINT8 = 6, //unsigned char
//	FT_UINT16 = 7, //7
//	FT_UINT32 = 8,
//	FT_UINT64 = 9,
//	FT_FLOAT = 10, //10
//	FT_DOUBLE = 11,
//	FT_STRING = 12,
//	FT_DATE = 13,   // Upp::Date
//	FT_TIME = 14,	// Upp::Time
//	FT_BLOB = 15,
//	FT_VALUE = 16, //u++ value
	L_ENTRY ( "bit",    FT_BIT,      Bool ),
	L_ENTRY ( "bool",   FT_BOOL,     Bool ),
	L_ENTRY ( "int8",   FT_INT8,     Int8 ),
	L_ENTRY ( "int16",   FT_INT16,     Int16 ),
	L_ENTRY ( "int32",   FT_INT32,     Int32 ),
	L_ENTRY ( "int64",   FT_INT64,     Int64 ),
	L_ENTRY ( "uint8",  FT_UINT8,   UInt8 ),
	L_ENTRY ( "uint16",  FT_UINT16,   UInt16 ),
	L_ENTRY ( "uint32",  FT_UINT32,   UInt32 ),
	L_ENTRY ( "uint64",  FT_UINT64,   UInt64 ),
	L_ENTRY ( "float",  FT_FLOAT,   Float ),
	L_ENTRY ( "double", FT_DOUBLE, Double ),
	L_ENTRY ( "string", FT_STRING, String ),
	L_ENTRY ( "date",   FT_DATE,     Date ),
	L_ENTRY ( "time",   FT_TIME,     Time ),
	L_ENTRY ( "blob",   FT_BLOB,     Blob ),
	L_ENTRY ( "value",   FT_VALUE,     Value ),
};



template <class T> struct Return
{
	typedef T& type;
};

template <> struct Return<bit>
{
	typedef bool type;
};

int compare ( FieldTypeId fid, const void * lhs, const void * rhs );

template <class T>
inline Upp::String SqlFieldValue1 ( T v )
{
	return Upp::String().Cat() << v;
}

template <>
inline Upp::String SqlFieldValue1<Upp::String> ( Upp::String v )
{
	return sql_quote ( v );
}

template <>
inline Upp::String SqlFieldValue1<const char*> ( const char* v )
{
	return sql_quote ( v );
}

template <>
inline Upp::String SqlFieldValue1<Upp::Date> ( Upp::Date v )
{
	return Upp::String().Cat() << '\'' << Upp::FormatDate ( v, "YYYY-MM-DD" ) << '\'';
}

template <>
inline Upp::String SqlFieldValue1<Upp::Time> ( Upp:: Time v )
{
	return Upp::String().Cat() << '\'' << Upp::FormatTime ( v, "YYYY-MM-DD hh:mm:ss" ) << '\'';
}


template <class T>
Upp::String SqlFieldValue ( T v )
{
	if ( Upp::IsNull ( v ) )
	{
		return "NULL";
	}

	return SqlFieldValue1 ( v );
}

template<>
inline Upp::String SqlFieldValue<const char*> ( const char* v )
{
	if ( v == nullptr )
		return "NULL";

	return SqlFieldValue1 ( v );
}
