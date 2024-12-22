class RecordSet::Record // act as the reference class
{
		Iterator& operator &() const;

	public:
	Record() = default;
		Record ( RecordDef* rdef, void * base ) : rdef ( rdef ), base ( base ) {}

		Record ( const Record& r ) : rdef ( r.rdef ), base ( r.base ) {}

		FieldValue operator[] ( int fieldIndex )
		{
			return FieldValue ( &rdef->GetFieldDef ( fieldIndex ), base );
		}

		const FieldValue operator[] ( int fieldIndex ) const
		{
			return FieldValue ( &rdef->GetFieldDef ( fieldIndex ), base );
		}

		FieldValue operator[] ( char * fieldName )
		{
			return ( *this ) [ rdef->GetFieldIndex ( const_cast<const char*> ( fieldName ) ) ];
		}

		const FieldValue operator[] ( char * fieldName ) const
		{
			return ( *this ) [ rdef->GetFieldIndex ( const_cast<const char*> ( fieldName ) ) ];
		}

		FieldValue operator[] ( const char * fieldName )
		{
			return ( *this ) [ rdef->GetFieldIndex ( fieldName ) ];
		}

		const FieldValue operator[] ( const char * fieldName ) const
		{
			return ( *this ) [ rdef->GetFieldIndex ( fieldName ) ];
		}

		String ToString() const
		{
			String s;

			if ( rdef != nullptr )
				for ( int i = 0; i < rdef->GetFieldCount();++i )
					s << ( *this ) [i] << ',';

			return s;
		}

	protected:
		const RecordDef* rdef;
		void  * base;
};


class RecordSet::Iterator
{
public:
	using value_type = RecordSet::Record;
	using difference_type = std::ptrdiff_t;
	using pointer = value_type*;
	using reference = value_type&;
	using iterator_category = std::random_access_iterator_tag;
	
	Iterator ( const RecordDef* rdef, void *base )
			: rdef ( rdef ), base ( base )
	{
	}

	Iterator ( const Iterator& i )
			: rdef ( i.rdef ), base ( i.base )
	{
	}

	Iterator& operator= ( const Iterator& rhs )
	{
		rdef = rhs.rdef;
		base = rhs.base;
		return *this;
	}

	reference operator*()
	{
		return *reinterpret_cast<Record*> ( this );
	}
	
	pointer operator->(){
		return reinterpret_cast<Record*> ( this );
	}
	reference operator[](difference_type n)
	{
		return *(*this+=n);
	}
	

	Iterator& operator++()
	{
		* ( lz::ptrint* ) &base += rdef->GetRecordSize();
		return *this;
	}

	Iterator operator++ ( int )
	{
		Iterator i ( *this );
		++*this;
		return i;
	}

	Iterator operator--()
	{
		* ( lz::ptrint* ) &base -= rdef->GetRecordSize();
		return *this;
	}

	Iterator operator-- ( int )
	{
		Iterator i ( *this );
		--*this;
		return i;
	}

	Iterator& operator+= ( int n )
	{
		* ( lz::ptrint* ) &base += n * rdef->GetRecordSize();
		return *this;
	}

	Iterator operator+ ( int n )
	{
		Iterator i ( *this );
		return i += n;
	}

	Iterator& operator-= ( int n )
	{
		return *this+=-n;
	}
	Iterator operator- ( int n )
	{
		return ( *this ) + ( -n );
	}

	int operator- ( Iterator i )
	{
		ASSERT ( i.rdef == rdef && ( ( char* ) base - ( char* ) i.base ) % i.rdef->GetRecordSize() == 0 );
		return int ( ( ( char* ) base - ( char* ) i.base ) / ( int ) rdef->GetRecordSize() );

	}

	Record operator[] ( int n )
	{
		Iterator i ( *this );
		i += n;
		return *reinterpret_cast<Record*> ( &i );
	}

	friend std::strong_ordering operator<=>(const Iterator& lhs, const Iterator& rhs)
	{
		ASSERT ( lhs.rdef == rhs.rdef );
		return lhs.base <=> rhs.base;
	}
		
//		bool operator< ( const Iterator& rhs ) const
//		{
//			ASSERT ( rdef == rhs.rdef );
//			return base < rhs.base;
//		}

		bool operator == ( const Iterator& rhs ) const
		{
			ASSERT ( rdef == rhs.rdef );
			return base == rhs.base;
		}

//		bool operator != ( const Iterator& rhs ) const
//		{
//			ASSERT ( rdef == rhs.rdef );
//			return base != rhs.base;
//		}

	protected:
		const RecordDef* rdef;
		void     * base;

		friend class RecordSet;
};
