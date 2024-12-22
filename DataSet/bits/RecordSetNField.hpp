
// a nested class for RecordSet
// to support chaining after RecordSet::AddField(...)
//
class RecordSet::RecordSetNField{
public:
	RecordSetNField(RecordSet& r, FieldDef& f): rset(&r), fdef(&f){}
	RecordSetNField(const RecordSetNField& c): rset(c.rset), fdef(c.fdef)   {}
	
	explicit operator RecordSet& (){  return *rset; }
	explicit operator FieldDef& () {  return *fdef; }
	auto& AddField( const char * fieldname, FieldTypeId id ){
		fdef = rset->AddField(fieldname, id).fdef;
		return *this;
	}
	
	template <class T>
	auto& AddField( const char * fieldname ){
		fdef = rset->AddField<T>( fieldname ).fdef;
		return *this;
	}
	auto& PhysicalLayout(){
		rset->PhysicalLayout();
		return *this;
	}
	
	auto& SetName ( const char * name )
	{
		rset->SetName( name );
		return *this;
	}

	const char * GetName() const
	{
		return rset->name;
	}

	const char * GetRowBase ( size_t row ) const
	{
		return rset->GetRowBase( row );
	}
	
	int GetFieldCount() const
	{
		return rset->GetFieldCount();
	}

	void Clear()
	{
		rset->Clear();
	}
	
	auto& SetNullable(bool v = true){
		fdef->SetNullable( v );
		return *this;
	}

	bool IsNullable() const
	{
		return fdef->IsNullable();
	}

	size_t SizeInBits() const
	{
		return fdef->SizeInBits();
	}

	size_t TypeAlign() const
	{
		return fdef->TypeAlign ( );
	}

	size_t TypeSize() const
	{
		return fdef->TypeSize ( );
	}

	unsigned GetStart() const
	{
		return fdef->GetStart();
	}

	const char * GetFieldName() const
	{
		return fdef->GetFieldName();
	}


	size_t GetFieldSize() const
	{
		return TypeSize();
	}


	bool FieldIsNull ( const void * base ) const
	{
		return fdef->FieldIsNull( base );
	}

	void MarkAsSet ( void * base )noexcept
	{
		fdef->MarkAsSet( base );
	}

	void MarkAsNotSet ( void *base )
	{
		fdef->MarkAsNotSet( base );
	}

	FieldTypeId GetFieldType() const
	{
		return fdef->GetFieldType();
	}


private:
	RecordSet * rset;
	FieldDef *  fdef;
};
