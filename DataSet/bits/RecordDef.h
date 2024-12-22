
//NAMESPACE_UPP

class RecordDef
{

	public:
		class FieldValue;
		
		
		RecordDef() {}

		RecordDef ( RecordDef && rhs )
		{
			fields = pick ( rhs.fields );
			rhs.fields.Clear();
			recordSize = rhs.recordSize;
			rhs.recordSize = 0;
		}

		RecordDef ( const RecordDef& rhs )
		{
			AppendRecordDef ( rhs );
		}

		int GetRecordSize() const
		{
			return recordSize;
		}

		RecordDef& operator= ( const RecordDef& rhs )
		{
			this->~RecordDef();
			return *new ( this ) RecordDef ( rhs );
		}

		RecordDef& operator= ( RecordDef && rhs )
		{
			this->~RecordDef();
			return *new ( this ) RecordDef ( std::move ( rhs ) );
		}

		RecordDef& AppendRecordDef ( const RecordDef& rhs );

		RecordDef& Reserve ( unsigned n )
		{
			fields.Reserve ( n );
			return *this;
		}

		RecordDef& PhysicalLayout();

		const FieldDef& GetFieldDef ( int index ) const
		{
			return fields[index];
		}

		const FieldDef& GetFieldDef ( const char * fldName ) const
		{
			return fields[GetFieldIndex ( fldName ) ];
		}

		int GetFieldCount() const
		{
			return fields.GetCount();
		}

		int GetFieldIndex ( const char * fldName ) const;

		String ToString() const;

		template <class T>
		FieldDef& Add ( const char * name );

		FieldDef& Add ( const char * name, FieldTypeId id )
		{
			ASSERT_(id!=FT_UNKNOWN, "Type unknown or not supported!");
			return DoAdd ( id, name );
		}

	private:
		Array<FieldDef> fields;
		int		  recordSize;

		//RecordDef(const RecordDef& rhs);
		// no out-of-bound check
		//
		const FieldDef& GetField ( int index ) const
		{
			return fields[index];
		}

		FieldDef& GetField ( int index )
		{
			return fields[index];
		}

		FieldDef& DoAdd ( FieldTypeId fid, const char *name )
		{
			return fields.Create<FieldDef> ( fid, name, 1 );
		}

};


template <class T>
FieldDef& RecordDef::Add ( const char * name )
{
	return DoAdd ( Type2Id < lz::TrimIntegral < T >>::id, name );
}

//END_UPP_NAMESPACE
