#include <CtrlLibExt/CtrlLibExt.h>

namespace lz{

constexpr lz::ptruint max_combined_id =
	( static_cast<lz::ptruint> ( 1 ) << ( sizeof ( void* ) * 8 - 10 ) )
	- 1;
GridEz::Row::~Row()
{
	if ( ( lz::AsInt ( info ) & 3 ) == 0 )
		delete info;
}

int  GridEz::Row::DataStorage() const
{
	if ( lz::AsInt ( info ) == 0 )
		return DT_NODATA;

	if ( lz::AsInt ( info ) & 2 )
		return DT_EXTERNAL;

	if ( lz::AsInt ( info ) & 1 )
		return ( lz::AsUInt ( info ) >> 1 ) >= 1024 ? DT_EXTERNAL : DT_NODATA;

	return info->data_type;
}

String GridEz::Row::GetText ( int col ) const
{
	if ( info && ( lz::AsInt ( info ) &3 ) == 0 && col < info->cells.GetCount() )
	{
		return info->cells[col].GetDataString();
	}

	return String();
}

Value GridEz::Row::GetData ( int col ) const
{
	if ( info && ( lz::AsInt ( info ) &3 ) == 0 && col < info->cells.GetCount() )
	{
		return info->cells[col].GetData();
	}

	return String();
}


int GridEz::Row::Height() const
{
	int h;

	if ( !HasRowData() )
	{
		int64 id;
		GetCombinedDataIdNHeight ( id, h );
	}

	else
		h = info->height;

	return h;
}

GridEz::Row& GridEz::Row::Height ( int h )
{
	if ( !HasRowData() )
	{
		int _h;
		int64 id;
		GetCombinedDataIdNHeight ( id, _h );
		StoreDataIDAndHeight ( id, h );
	}

	else
		info->height = h;

	return *this;
}

GridEz::Row& GridEz::Row::StoreDataIDAndHeight ( int64 dataid, int h )
{

	using namespace lz;
	ASSERT ( h >= DEFAULT_ROW_HEIGHT );
	ASSERT ( dataid >= -1 );

#define TO_PTRUINT(x) static_cast<lz::ptruint>(x)

	bool done = false;

	switch ( ( static_cast<int> ( dataid == -1 ) << 1 ) | static_cast<int> ( h == DEFAULT_ROW_HEIGHT ) )
	{

		case 0: // valid data id, no default height

			if ( h < 256 && dataid < (
					 // biggest dataid can be stored with total bits minus 8MSB and 2LSB
					 lz::ptruint ( 1 ) << ( sizeof ( info ) *8 - 8 - 2 )
				 )
			   )
			{
				AsUInt ( info ) = ( TO_PTRUINT ( h ) << ( sizeof ( info ) * 8 - 8 ) ) // put height to 8 MSB
								  + ( TO_PTRUINT ( dataid ) << 2 ) // put data id to desired bits
								  + 2; // so that we know it's a combination of height and data id
				done = true;
			}

			break;

		case 1: // has data id, default row height

			if ( dataid < ( TO_PTRUINT ( 1 ) << ( sizeof ( info ) *8 - 1 ) ) - 1024 )
				done = true, AsUInt ( info ) = ( ( static_cast<ptruint> ( dataid ) + 1024 ) << 1 ) + 1;

			break;

		case 2: //no data, non-default row height
			if ( h < 1024 )
				done = true, AsUInt ( info ) = ( h << 1 ) + 1;

			break;

		case 3: // has both data ID and non=default height
			if ( h < 256 && dataid < max_combined_id )
				done = true, AsUInt ( info ) = ( TO_PTRUINT ( h ) << ( sizeof ( info ) * 8 - 8 ) ) // height to the 8 MSB
						+ ( TO_PTRUINT ( dataid ) << 2 ) // dataid to the designated position
						+ 2;

	}

	if ( !done )
	{
		info = RowData::NewFrom ( dataid, h );
	}

	return *this;

#undef TO_PTRUINT
}

int64 GridEz::Row::RowDataId() const
{
	if ( !HasRowData() )
	{
		int h;
		int64 id;
		GetCombinedDataIdNHeight ( id, h );
		return id;
	}

	return info->DataRowId();
}

GridEz::Row& GridEz::Row::RowDataId ( int64 id )
{
	if ( !HasRowData() )
	{
		int h;
		int64 oldid;
		GetCombinedDataIdNHeight ( oldid, h );
		StoreDataIDAndHeight ( id, h );

	}

	else
		info->DataRowId ( id );

	return *this;
}

void GridEz::Row::GetCombinedDataIdNHeight ( int64& dataid, int& height ) const
{
	ASSERT ( !HasRowData() );

	if ( info == nullptr )
		dataid = -1, height = DEFAULT_ROW_HEIGHT;
	else
		if ( lz::AsInt ( info ) & 1 ) // LSB is set, either dataid or height
		{
			lz::ptruint v = lz::AsUInt ( info ) >> 1;

			if ( v < 1024 )
				dataid = -1, height = static_cast<int> ( v );
			else
				dataid = v - 1024, height = DEFAULT_ROW_HEIGHT;
		}

		else
		{
			height = static_cast<int> ( lz::AsUInt ( info ) >> ( sizeof ( info ) * 8 - 8 ) );
			dataid = ( lz::AsUInt ( info ) >> 2 ) & max_combined_id;
		}
}


GridEz::Row& GridEz::Row::Bold ( tribool v )
{
	if ( v != tribool::Null || HasRowData() )
	{
		AllocRowFormat();
		info->Bold ( v );
	}

	return *this;
}

GridEz::Row& GridEz::Row::HorzAlign ( Alignment align )
{
	ASSERT ( align >= ALIGN_NULL && align <= ALIGN_CENTER );

	if ( align != ALIGN_NULL || !HasRowData() )
	{
		AllocRowFormat();
		info->HorzAlign ( align );
	}

	return *this;

}

GridEz::Row& GridEz::Row::VertAlign ( Alignment align )
{
	ASSERT ( align >= ALIGN_NULL && align <= ALIGN_CENTER );

	if ( align != ALIGN_NULL || !HasRowData() )
	{
		AllocRowFormat();
		info->VertAlign ( align );
	}

	return *this;
}


void GridEz::Row::AllocRowFormat()
{
	if ( !HasRowData() )
	{
		int64 id;
		int h;

		GetCombinedDataIdNHeight ( id, h );
		info = new RowData;
		info->data_type = ( id == -1 ) ? DT_NODATA : DT_EXTERNAL;
		info->datarowid = id;
		info->height = h;
	}
}


namespace
{
//	typedef GridEz::Row::RowData RowData;
//	// empty base class optimization doesn't work here, manually construct structs
//
//	struct fff: public RowData
//	{
//		int64 rowdataid;
//	};
//
//	struct tff: public RowData
//	{
//		void *data;
//	};
//
//	struct ftf: public RowData
//	{
//		void*	cellfmts;
//		CellFormat rowfmt;
//		int64 rowdataid;
//	};
//
//	struct ttf: public RowData
//	{
//		void *data;
//		void*	cellfmts;
//		CellFormat rowfmt;
//	};
//
//	struct fft: public RowData
//	{
//		void*	cellfmts;
//		int64 rowdataid;
//	};
//
//	struct tft: public RowData
//	{
//		void *data;
//		void *cellfmts;
//	};
//
//	struct ftt: public RowData
//	{
//		void* cellfmts;
//		CellFormat rowfmt;
//		int64 rowdataid;
//	};
//
//	struct ttt: public RowData
//	{
//		void *data;
//		void *cellfmts;
//		CellFormat rowfmt;
//	};
}


GridEz::Row::RowData::~RowData()
{
}



//size_t	GridEz::Row::RowData::ActualSize ( bool local_data, bool rowfmt, bool cellfmt )
//{
//	static size_t sz[] =
//	{
//		sizeof ( fff ),
//		sizeof ( tff ),
//		sizeof ( ftf ),
//		sizeof ( ttf ),
//		sizeof ( fft ),
//		sizeof ( tft ),
//		sizeof ( ftt ),
//		sizeof ( ttt )
//	};
//	return sz[ int ( local_data ) | ( int ( rowfmt ) <<1 ) | ( int ( rowfmt ) <<1 ) ];
//}

GridEz::Row::RowData*	GridEz::Row::RowData::NewFrom ( int64 dataid, int height )
{
	RowData* p = new RowData;
	p->data_type = DT_EXTERNAL;
	p->DataRowId ( dataid );
	p->Height ( height );
	return p;
}

int64      GridEz::Row::RowData::DataRowId() const
{
	return this->data_type == DT_EXTERNAL ? datarowid : -1;
}


GridEz::Row::RowData&	GridEz::Row::RowData::DataRowId ( int64 id )
{
	ASSERT ( this->data_type == DT_EXTERNAL );
	this->datarowid = id;
	return *this;
}

GridEz::Row::RowData&	GridEz::Row::RowData::Height ( int height )
{
	this->height = height;
	return *this;
}

CellFormat& GridEz::Row::RowData::RowFormat()
{
	char * p = reinterpret_cast<char *> ( this ) + sizeof ( *this ); // what compiler think RowData's size is

	if ( !this->hasrowfmt )
		return Single<CellFormat>(nullptr); // return a Null format as it has none.

	p += this->data_type == DT_EXTERNAL ? sizeof ( int64 ) : sizeof ( void * ); // skip data_id or data pointer

	return *reinterpret_cast<CellFormat*> ( p );
}


END_NAMESPACE
