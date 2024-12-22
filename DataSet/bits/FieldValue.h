class RecordSet;
class FieldDef;

class FieldDef::FieldValue
{

public:
	FieldValue ( const FieldDef *fldDef, void *base )
			: fldDef ( fldDef ), base ( base )
	{
	}

	// this should be the super fast way of accessing stored object
	// but it's less safe! use with caution!!
	//
	template <class T>
	const typename Return<T>::type Get() const;

	template <class T>
	typename Return<T>::type As() const
	{
		return const_cast<typename Return<T>::type> ( Get<T>() );
	}

	// translation performed if necessary, slower but safer
	//
	template <class T>
	T& Get ( T& var ) const
	{
		DoGet ( &var, Type2Id < lz::TrimIntegral < T >>::id );
		return var;
	}

	template <class T>
	void Set ( const T& v )
	{
		auto srcid = Type2Id<lz::TrimIntegral<T>>::id;
		auto destid = fldDef->tid;
		
		if constexpr (std::is_assignable_v<T, T> || std::is_trivial_v<T>)
		{
			if( fldDef->tid == srcid )[[likely]]
			{
#ifdef _DEBUG
				static bool flag;
				if(!flag){
					LOG(String().Cat()<<basic_types[fldDef->tid].name <<" TAKES PATH Set()::is_assignable");
					flag = true;
				}
#endif
				*reinterpret_cast<T*>(location()) = v;
				MarkAsSet();
				return;
			}
		}
		{
#ifdef _DEBUG
			static bool flag;
			if(!flag){
				LOG(String().Cat()<<basic_types[fldDef->tid].name <<" TAKES PATH Set()::generic");
				flag = true;
			}
#endif
			DoSet ( &v, Type2Id < lz::TrimIntegral < T >>::id );
		}
	}

	void Set ( const char* v )
	{
		DoSet ( v, FT_PCHAR );
	}

	void Set ( char* v )
	{
		DoSet ( const_cast<const char*> ( v ), FT_PCHAR );
	}

	void SetBit ( bool v )
	{
		ASSERT ( fldDef->tid == FT_BIT );
		::SetBit ( base, fldDef->start, v );
		MarkAsSet();
	}

	template <class T>
	operator T() const
	{
		lz::TrimIntegral<T> t;
		return Get ( t );
	}

//	template <class T>
//	operator T(){
//		typename lz::Map<T>::type t;
//		return const_cast<const FieldValue*>(this)->Get(t);
//	}

	template <class T>
	typename Type2Id<lz::TrimIntegral<T>>::return_type
	operator= ( const T& rhs )
	{
		Set ( rhs );
		return rhs;
	}

	inline bool IsNull() const;
	void SetNull();

	// for debug
	void dumpflags();

	inline const char * GetRowBase();
	String ToString() const;

	const FieldDef& GetFieldDef() const
	{
		return *fldDef;
	}

	// naviagtion next field/next row, etc

	inline char * location()
	{
		return const_cast<char*> (
				   const_cast<const FieldValue&> ( *this ).location()
			   );
	}

	inline const char * location() const;

	// flag the bit that determines whether a field has been set (NOT NULL)
	//
	void MarkAsSet();
	void MarkAsNotSet();
	// internal use, valid only when field type id is not FT_BIT
	//

	// return 0/1 for boolean type
	const void * DoGet() const;
	void DoSet ( const void * data, FieldTypeId inTid );

private:
	const FieldDef * fldDef;
	void     * base;

	void PODSet ( const void* data );
	void *DoGet ( void * dest, FieldTypeId destTid ) const;


	// all type should accept input in the format of traditional const char *
	//
	void SetFromPChar ( const char * input );
//friend String& operator<<(String& lhs, const FieldValue& rhs);
//friend String& operator<<(String& lhs, FieldValue& rhs);

	friend class RecordSet;
};

