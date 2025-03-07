
#include "FieldValue.h"

class RecordSet
{
	public:
		class RecordSetNField; // for method chaining.
		class Iterator;
		class Record;
		using FieldValue = FieldDef::FieldValue;

		RecordSet()
				: data ( nullptr ), recordCount ( 0 ), capacity ( 0 ), curRecord ( 0 )
		{
			name[0] = '\0';
		}

		RecordSet ( RecordDef && recordDef )
				: recordDef ( std::move ( recordDef ) ),
				data ( nullptr ), recordCount ( 0 ), capacity ( 0 ), curRecord ( 0 )
		{
			name[0] = '\0';
		}

		RecordSet ( RecordSet && rs )
		{

			struct s
			{
				char buff[sizeof ( RecordSet ) ];
			};
			memcpy(this, &rs, sizeof(*this));
			new(&rs)RecordSet;
//			data = nullptr;
//			std::swap ( * ( s* ) this, * ( s* ) &rs );
		}

		RecordSet& operator= ( RecordSet && rs )
		{

			struct s
			{
				char buff[sizeof ( rs ) ];
			};

			std::swap ( * ( s* ) this, * ( s* ) &rs );
			return *this;
		}


		RecordSet& SetName ( const char * name )
		{
			strncpy ( this->name, name, max_name_len );
			return *this;
		}

		const char * GetName() const
		{
			return name;
		}

		const char * GetRowBase ( size_t row ) const
		{
			return data + row*GetRecordDef().GetRecordSize();
		}

		auto GetFieldIndex(const char * fldName)const
		{
			return recordDef.GetFieldIndex(fldName);
		}

		auto GetFieldCount() const
		{
			return recordDef.GetFieldCount();
		}

		String ToString() const
		{
			String s;

			if ( recordDef.GetFieldCount() == 0 )
				return s;
			
			s << GetName() <<"\n";

			for ( int i = 0; i < GetFieldCount(); ++i )
			{
				if ( i != 0 )
					s << '\t';

				s << recordDef.GetFieldDef ( i ).GetFieldName() << '('
				<< TypeName ( recordDef.GetFieldDef ( i ).GetFieldType() ) << ')';
			}

			s << "\n---------------------------------\n";

			for ( size_t i = 0; i < RecordCount(); ++i )
			{
				for ( size_t j = 0; j < recordDef.GetFieldCount(); ++j )
				{
					String t;

					if ( j != 0 )
						s << '\t';

					s << const_cast<RecordSet&> ( *this ) ( i, j ).Get ( t );
				}

				s << '\n';
			}

			return s;
		}

		void Clear();

		~RecordSet();

		const RecordDef& GetRecordDef() const
		{
			return recordDef;
		}

		RecordSet& SetRecordDef ( const RecordDef& rdf );
		RecordSet& SetRecordDef ( RecordDef && rdf );
		RecordSet& AppendRecordDef ( const RecordDef& rdf );
		RecordSet& Reserve ( size_t count );


		template <class T>
		RecordSetNField AddField ( const char * fieldname );

		RecordSetNField AddField ( const char * fieldname, FieldTypeId id );

		RecordSet& PhysicalLayout()
		{
			recordDef.PhysicalLayout();
			return *this;
		}

		Iterator begin() const;
		Iterator Begin() const;
		Iterator end() const;
		Iterator End() const;

		void Sort ( const Vector<int>& fields );
		void Sort ( const char * sortby)
		{
			Vector<int> by = ParseSortBy(sortby);
			Sort(sortby);
		}
//		template <typename...Ts>
//		void Sort(Ts... args)
//		{
//			Vector<int> by;
//			if constexpr(sizeof...(args)>=1){
//				GetSortBy(by, std::forward<Ts>(args)...);
//			}
//			Sort(by);
//		}

		RecordSet& Erase ( int i );
		RecordSet& Erase ( int i, int j );
		RecordSet& Erase ( const Vector<int>& i );
		RecordSet& EraseAll();

		// will position cursor to the newly appended row so
		// that it can be modified conveniently
		//
		RecordSet& Append()
		{
			return AppendRecord();
		}

		RecordSet& AppendRecord();

		void First()
		{
			curRecord = 0;
		}

		void Last()
		{
			curRecord = recordCount - 1;
		}

		void Next()
		{
			if ( !Eof() )[[likely]]
				++curRecord;
		}

		void Prev()
		{
			if ( !Bof() )[[likely]]
				--curRecord;
		}

		void Goto ( int recNo )
		{
			if ( recNo > recordCount )[[unlikely]]
				recNo = recordCount;

			curRecord = recNo;
		}

		bool Eof() const
		{
			return curRecord >= recordCount;
		}

		bool Bof() const
		{
			return curRecord == -1;
		}

		bool IsEmpty() const
		{
			return recordCount == 0;
		}

		int RecordCount() const
		{
			return recordCount;
		}

		int RecordNo() const noexcept
		{
			return curRecord;
		}

		int RecordSize() const noexcept
		{
			return GetRecordDef().GetRecordSize();
		}

		int GetCount() const noexcept
		{
			return recordCount;
		}

		Record operator[] ( int i );

		const Record operator[] ( int i ) const;

		FieldValue operator() ( int fieldIndex )
		{
			return FieldValue ( &GetRecordDef().GetFieldDef ( fieldIndex ),
								const_cast<char*> ( GetRowBase ( curRecord ) ) );
		}

		FieldValue operator[] ( const char* fieldName )
		{
			if(int i = GetFieldIndex ( fieldName ); i!=-1)[[likely]]
			{
				return this->operator() ( i );
			}else{
				char msg[100];
				snprintf(msg, sizeof msg,"{%s} not present in the RecordDef", fieldName);
				throw std::runtime_error(msg);
			}
		}

		FieldValue operator() ( const char* fieldName )
		{
			return ( *this ) [fieldName];
		}

		FieldValue operator() ( size_t row, size_t col )
		{
			return FieldValue ( &GetRecordDef().GetFieldDef ( ( int ) col ),
								const_cast<char*> ( GetRowBase ( row ) )
							  );
		}

		FieldValue operator() ( size_t row, int col )
		{
			return ( *this ) ( row, ( size_t ) col );
		}

		FieldValue operator() ( int row, int col )
		{
			return ( *this ) ( ( size_t ) row, ( size_t ) col );
		}

		FieldValue operator() ( size_t row, const char *fldname )
		{
			return ( *this ) ( row, GetFieldIndex ( fldname ) );
		}


		//RecordSet& operator=(RecordSet&& rhs);


//	void Dump()
//	{
//		for(unsigned i=0; i<recordCount;++i)
//		{
//			for(unsigned j=0; j<recordDef.GetFieldCount();++j)
//				DUMP(
//		}
//		for(unsigned i=0; i<recordCount; ++i)
//			DumpRecord(i, recordDef.GetRecordSize());
//	}
//
//	void DumpRecord(int i, size_t size)
//	{
//		char * p=data+i*size;
//		String s;
//		LOG(String().Cat()<<"DUMPING RECORD "<<i<<" STARTING "
//		    <<(void*)(data+i*size)<<" SIZE "<<size);
//		for(size_t j=0; j<size; ++j)
//		{
//			s<<DumpByte(p[j]);
//		}
//		LOG(s);
//	}
//
//	String DumpByte(int c)
//	{
//		String s;
//		s<< Hex( (c & 0xF0)>>4 )
//		 << Hex( c & 0xF);
//		return s;
//	}
//
//	char Hex(int c)
//	{
//		if(c<10)
//			return c+'0';
//		else
//			return c-10+'A';
//	}
		static constexpr size_t max_name_len = 31;

		static void DoSwapRecord ( Iterator i, Iterator j );

	protected:
		RecordDef recordDef;
		char * data; // means void*, simpler to operate on
		char   name[max_name_len];
		int recordCount;
		int capacity;
		int curRecord;

	private:
		RecordSet& operator= ( const RecordSet& rhs );

		Vector<int> ParseSortBy(const char* sortby);


	public:

};

#include "RecordSetNField.hpp"
#include "RecordSetReferenceIterator.hpp"

template <class T>
inline RecordSet::RecordSetNField RecordSet::AddField ( const char * fieldname )
{
	if ( recordCount != 0 )
		throw std::runtime_error ( "RecordDef not expected to be resized after populated" );

	return RecordSetNField(*this, recordDef.Add<T> ( fieldname ) );
}

inline RecordSet::RecordSetNField RecordSet::AddField ( const char * fieldname, FieldTypeId id )
{
	if ( recordCount != 0 )
		throw std::runtime_error ( "RecordDef not expected to be changed after being populated" );

	return RecordSetNField{*this, recordDef.Add ( fieldname, id )};
}


inline RecordSet::Iterator RecordSet::begin() const
{
	return Begin();
}

inline RecordSet::Iterator RecordSet::Begin() const
{
	return Iterator ( &GetRecordDef(), data );
}

inline RecordSet::Iterator RecordSet::end() const
{
	return End();
}

inline RecordSet::Iterator RecordSet::End() const
{
	return Iterator ( &GetRecordDef(), const_cast<char*> ( GetRowBase ( recordCount ) ) );
}

inline RecordSet::Record RecordSet::operator[] ( int i )
{
	return Record ( &recordDef, const_cast<char*> ( GetRowBase ( i ) ) );
}

inline const RecordSet::Record RecordSet::operator[] ( int i ) const
{
	return const_cast<RecordSet&> ( *this ) [i];
}


// not intended for bit fields
inline const char * FieldDef::FieldValue::location() const
{
	ASSERT(fldDef->tid != FT_BIT);
	return ( const char* ) base + fldDef->start;
}

inline bool FieldDef::FieldValue::IsNull() const
{
//	RLOG("In FieldValue::IsNull()");
	return  fldDef->FieldIsNull ( base );
}

inline void FieldDef::FieldValue::MarkAsSet()
{
	const_cast<FieldDef&> (
		*fldDef
	).MarkAsSet ( base );
}

inline void FieldDef::FieldValue::MarkAsNotSet()
{
	const_cast<FieldDef&> (
		*fldDef
	).MarkAsNotSet ( base );
}



namespace
{
	template <class T>

	struct BitRet
	{
		static inline T& cast ( const void * p )
		{
			return const_cast<T&> ( *reinterpret_cast<const T*> ( p ) );
		}
	};

	template <>

	struct BitRet<bit>
	{
		static inline bool cast ( const void * p )
		{
			return p != NULL;
		}
	};
}

template <class T>
inline	const typename Return<T>::type FieldDef::FieldValue::Get() const
{
#ifdef _DEBUG
	auto id = Type2Id<lz::TrimIntegral<T>>::id;

//	if constexpr ( std::signed_integral<T> )
//		id = Type2Id < lz::IntOfSize < sizeof ( T ) >>::id;
//	else
//		if constexpr ( std::unsigned_integral<T> )
//			id = Type2Id < lz::UIntOfSize < sizeof ( T ) >>::id;

	if ( fldDef->tid != id )
	{
		String s;
		s << __FILE__ << ':' << __LINE__
		<< "[DataSet][";
//			if(fldDef->parent)
//				s<<fldDef->parent->name<<"][";
		s << fldDef->name << "] using data type "
		<< TypeName ( fldDef->tid ) << " as type "
		<< TypeName ( id ) << " is not safe. Please double check!"
		// <<typeid(T).name()
		;
		LOG ( s );
	}

	ASSERT ( fldDef->tid == id );

#endif
	return BitRet<T>::cast ( DoGet() );
}


Upp::String SqlFieldValue (FieldDef::FieldValue v );

//END_UPP_NAMESPACE

