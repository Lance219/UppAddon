#include "../DataSet.h"

BEGIN_NAMESPACE_LZ

RecordDef& RecordDef::AppendRecordDef ( const RecordDef& rhs )
{

	for ( const auto& fld : rhs.fields )
	{
		auto& _fld = Add ( fld.name, fld.tid );
		_fld.SetNullable ( fld.IsNullable() );
	}

	PhysicalLayout();

	return *this;
}

int RecordDef::GetFieldIndex ( const char * fldName ) const
{
	for ( int i = 0; i < fields.GetCount(); ++i )
	{
		if ( _stricmp ( GetField ( i ).name, fldName ) == 0 )
			return i;
	}

	return -1;
}

static int cmppfield ( const void* f1, const void * f2 )
{
	FieldDef& a = **reinterpret_cast<FieldDef * const*> ( f1 );
	FieldDef& b = **reinterpret_cast<FieldDef * const*> ( f2 );
	return int ( b.GetFieldSize() - a.GetFieldSize() );
}


RecordDef& RecordDef::PhysicalLayout()
{
	Vector<void *> precdef;
	Vector<void *> freebits;

	for ( int i = 0; i < fields.GetCount();++i )
		precdef.Add ( &fields[i] );

	qsort ( &precdef[0], precdef.GetCount(), sizeof ( void* ), cmppfield );

	int start = 0, i = 0; //fields.size();

	FieldDef* f;

	int max_align = 0;

	for ( ; i < precdef.GetCount() && ( f = ( FieldDef* ) precdef[i] )->tid != FT_BIT; ++i )
	{
		f = ( FieldDef* ) precdef[i];
		int align = ( int ) f->TypeAlign();

		if ( align > max_align )
			max_align = align;

		ASSERT_ ( align != 0, "align should not be 0!" );

		int padding = ( align - start % align ) % align;

		for ( lz::ptrint j = 0; j < padding*8; ++j )
			freebits.Add ( ( void* ) ( start + j ) );

		start += padding;

		// remember any padding!!
		f->start = start;

		start += ( int ) f->GetFieldSize();
	}

	// again!
	start *= 8; // translate to bits

	int j = 0;

	for ( i = 0; i < precdef.size(); ++i )
	{
		f = ( FieldDef* ) precdef[i];

		if ( f->IsNullable() )
		{
			if ( j < freebits.GetCount() )
				f->nullbit = j++;
			else
				f->nullbit = start++;
		}

		if ( f->tid == FT_BIT )
		{
			if ( j < freebits.GetCount() )
				f->start = j++;
			else
				f->start = start++;
		}

	}

	recordSize = max_align == 0 ? 0 :

				 ( ( start - 1 ) / 8 + max_align ) / max_align * max_align; // align to 4 bytes. Should it be 8 bytes on 64bit machine??

	return *this;
}

// mostly for debug purpose
//
String RecordDef::ToString() const
{
	String s;
	s << "RecordDef Record Size:" << this->GetRecordSize() << "\n";

	for ( int i = 0; i < fields.GetCount(); ++i )
	{
		const FieldDef& f = fields[i];
		s << "\tName: " << f.name << " Type:" << TypeName ( f.tid ) << "(" << static_cast<int> ( f.tid )

		<< ") Start: " << f.start << "\n";
	}

	return s;
}

END_NAMESPACE
