#ifndef _DataSet_DataSet_h_
#define _DataSet_DataSet_h_

#include <Core/Core.h>
#include <stdexcept>

#include <CoreExt/CoreExt.h>
#include "bits/Util.h"


NAMESPACE_UPP
#include "bits/FieldDef.h"
#include "bits/RecordDef.h"
#include "bits/RecordSet.h"
//#include "bits/RecordSetNField.hpp"

// DataSet is an array of RecordSets.

class DataSet
{
		typedef DataSet Self;

	public:
		DataSet() = default;

		DataSet ( DataSet && ds ) = default;
//		{
//			*this = std::move ( ds );
//		}

		DataSet ( const Self& rhs ) = delete;
		Self& operator= ( const Self& rhs ) = delete;

		Self& operator= ( DataSet && rhs )=default;

		// return index of the named recordset in the
		//      current DataSet. size_t(-1) if not exists.
		// this also rely on the underlying container's supporting
		// array-like random access. May change in the future!!
		int IndexOf ( const char *RecordSetName ) const;

//		template <class Self>
//		auto&& operator[] (this Self&& self, int index )
//		{
//			return self.recordSets[index];
//		}

		RecordSet& operator[] ( int index )
		{
			return recordSets[index];
		}

		const RecordSet& operator[] ( int index ) const
		{
			return recordSets[index];
		}

		RecordSet& operator[] ( const char * name )
		{
			return ( *this ) [ IndexOf ( name ) ];
		}

		const RecordSet& operator[] ( const char * name ) const
		{
			return ( *this ) [ IndexOf ( name ) ];
		}

		unsigned GetCount() const
		{
			return recordSets.GetCount();
		}

		void Reserve ( int num_rset )
		{
			recordSets.Reserve ( num_rset );
		}

		RecordSet& Add ( const char * recordset_name )
		{
			// allow multiple RecordSet with the same name?
			// yes for now
			RecordSet *rs = new RecordSet();
			rs->SetName ( recordset_name );
			recordSets.Add ( rs );
			return *rs;
		}

		RecordSet& Add ( RecordSet && rset, const char *name = nullptr )
		{
			RecordSet *rs = new RecordSet();
			( *rs ) = std::move ( rset );

			if ( name )
				rs->SetName ( name );

			recordSets.Add ( rs );

			return*rs;
		}

		bool Remove ( int index )
		{
			if ( index >= 0 && index < recordSets.GetCount() )
			{
				recordSets.Remove ( index );
				return true;
			}
			return false;
		}

		bool Remove ( const char *name )
		{
			if(int i = IndexOf( name); i>=0 )[[likely]]
				return Remove ( i );
			return false;
			
		}

		bool Contains ( const char *name ) const
		{
			return IndexOf ( name ) != -1;
		}

		void Clear()
		{
			recordSets.Clear();
		}

	private:
		Array<RecordSet> recordSets;

};

END_UPP_NAMESPACE

#endif
