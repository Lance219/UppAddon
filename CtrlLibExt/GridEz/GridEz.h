#ifndef _GridEz_GridEz_h
#define _GridEz_GridEz_h

#include <type_traits>
//#include <CtrlLibExt/CtrlLibExt.h>

BEGIN_NAMESPACE_LZ

#include "bits/CellFormat.h"
#include "bits/UseCellFormatMethod.h"
#include "bits/GridCell.h"

class GridEz : public Ctrl
{
	public:
		void  LeftDown ( Point p, dword ) override;
		void  LeftDouble ( Point p, dword ) override;
		void  RightDown ( Point p, dword flags ) override;
		void  MouseMove ( Point p, dword ) override;
		Image CursorImage ( Point p, dword ) override;
		void MouseWheel ( Point p, int zdelta, dword keyflags ) override;

		void Paint ( Draw& w ) override;
		void State ( int reason ) override;

		bool Key ( dword key, int ) override;


	public:
		static int const DT_NODATA = 0, DT_EXTERNAL = 1, DT_LOCAL = 2;
		
#		include "bits/Column.h"
#		include "bits/Row.h"
#		include "bits/JoinedCellManager.h"
#		include "bits/ClientAreaManager.h"
#		include "bits/CachedData.h"

	struct CellPoint
	{
		int row, col;
		Point point;
	};


	public:
		typedef GridEz Self;

		GridEz();
		
		Column& AddColumn ( int width = DEFAULT_COL_WIDTH, int dataColumnId = -1 );
		Column& AddColumn ( String name, int width = DEFAULT_COL_WIDTH, int dataColumnId = -1 );
		Column& AddColumn ( StrLiteral name, int width = DEFAULT_COL_WIDTH, int dataColumnId = -1 );
		Row& InsertRow ( int index, int height = DEFAULT_ROW_HEIGHT );
		
		template < typename...Types>
		Row& AddHeightedRow ( int height, Types&&...args );

		template < typename...Types>
		Row& AddRow ( Types&&...args )
		{
			return AddHeightedRow ( DEFAULT_ROW_HEIGHT, std::forward<Types>(args)... );
		}

		template <typename...Types>
		Row& InsertHeightedRow ( int index, int height, Types&&...args );

		template <typename...Types>
		Row& InsertRow ( int index, Types&&...args )
		{
			return InsertHeightedRow ( index, DEFAULT_ROW_HEIGHT,
				std::forward<Types>(args)... );
		}


		Row& AddExternalRow ( uint64 rowid, int height = DEFAULT_ROW_HEIGHT );
		Row& InsertExternalRow ( int index, uint64 rowid, int height = DEFAULT_ROW_HEIGHT );

		template <typename...Types>
		Row& AddNonfixedRow ( Types&&...args )
		{
			return InsertRow ( GetCount() - GetFixedBottomRowCount(),
				std::forward<Types>(args)... );
		}

		template <typename...Types>
		Row& AddHeightedNonfixedRow ( int height, Types&&...args )
		{
			return InsertHeightedRow (
					   GetCount() - GetFixedBottomRowCount(), height,
					   std::forward<Types>(args)...
				   );
		}
		
		bool	IsColResizable(int col_index)const{
			return cols[col_index].IsResizable();
		}

		GridEz& Remove ( int row_index );

		GridEz& Clear ( bool columns = true );
		GridEz& ClearNonfixedRows();

		GridEz& Set ( int row, int col );

		template <typename T>
		GridEz& Set ( int row, int col, T value );

		Value Get0 ( int row, int col ) const;

		Value Get ( int col ) const
		{
			return Get0 ( currow, col );
		}

		GridEz& SetRowCount ( int n );
		GridEz& SetColCount ( int n );
		GridEz& SetFixedLeftColCount ( int n );
		GridEz& SetFixedRightColCount ( int n );
		GridEz& SetFixedTopRowCount ( int n );
		GridEz& SetFixedBottomRowCount ( int n );
		GridEz& JoinCells ( int startcol, int startrow, int countcol, int countrow );
		GridEz& UnjoinCells ( int col, int row );
		
		GridEz& FitColumns(bool fit = true)		{ fitcolumns = fit; return *this; }
		GridEz& NoFitColumns()					{ return FitColumns(false);		  }
		bool	IsFitColumns()const				{ return fitcolumns;			  }

		GridEz& FitRows(bool fit = true)		{ fitrows = fit; return *this;	  }
		GridEz& NoFitRows()						{ return FitRows(false);		  }
		bool	IsFitRows()const				{ return fitrows;				  }
		
		GridEz& DefaultColWidth ( int w )
		{
			ASSERT ( w > 0 );
			defaultcolwidth = w;
			return *this;
		}

		GridEz& DefaultRowHeight ( int h )
		{
			ASSERT ( h > 0 );
			defaultrowheight = h;
			return *this;
		}

		GridEz& DefaultColWidthFromFont()
		{
			defaultcolwidth = Font().GetAveWidth() * 20;
			return *this;
		}

		GridEz& DefaultRowHeightFromFont()
		{
			defaultrowheight = int ( Font().GetHeight() * 1.5 );
			return *this;
		}

		Row& operator[] ( int i )
		{
			ASSERT ( i >= 0 && i < GetRowCount() );
			return rows[i];
		}

		const Row& operator[] ( int i ) const
		{
			return const_cast<const Row&> (
					   const_cast<GridEz&> ( *this ) [i]
				   );
		}

		int GetRowCount() const
		{
			return rows.size();
		}

		int GetCount() const
		{
			return GetRowCount();
		}

		int GetColCount() const
		{
			return cols.size();
		}

		int GetRowHeight(int row_index)
		{
			ASSERT( row_index>=0 && row_index<rows.GetCount());
			return rows[row_index].Height() == DEFAULT_ROW_HEIGHT ? defaultrowheight : rows[row_index].Height();
		}
		
		int GetColWidth(int col_index)
		{
			ASSERT( col_index>=0 && col_index<rows.GetCount());
			return cols[col_index].Width() == DEFAULT_COL_WIDTH ? defaultcolwidth : cols[col_index].Width();
		}

		int GetColFullWidth(int col_index)
		{
			ASSERT( col_index>=0 && col_index<rows.GetCount() );
			return cols[col_index].FullWidth() == DEFAULT_COL_WIDTH ? defaultcolwidth : cols[col_index].FullWidth();
		}

		int GetIndexOfFirstNonfixedRow() const
		{
			return GetFixedTopRowCount();
		}

		int GetIndexOfLastNonfixedRow() const
		{
			return GetCount() - GetFixedBottomRowCount() - 1;
		}

		int GetNonfixedRowCount() const
		{
			return GetRowCount() - GetFixedRowCount();
		}

		bool IsFixedRowWantCursor() const
		{
			return fixedrowwantcursor;
		}

		bool IsFixedColWantCursor() const
		{
			return fixedcolwantcursor;
		}

		GridEz& FixedRowWantCursor ( bool b = true )
		{
			fixedrowwantcursor = b;
			return *this;
		}

		GridEz& FixedColWantCursor ( bool b = true )
		{
			fixedcolwantcursor = b;
			return *this;
		}

		bool IsFixedTopRow ( int r ) const
		{
			ASSERT ( r >= 0 );
			return r < fixedtoprows;
		}

		bool IsFixedBtmRow ( int r ) const
		{
			ASSERT ( r >= 0 );
			return r >= GetRowCount() - fixedbtmrows;
		}

		bool IsFixedLeftCol ( int col) const
		{
			ASSERT ( col >= 0 );
			return col < fixedleftcols;
		}

		bool IsFixedRightCol ( int col ) const
		{
			ASSERT ( col >= 0 );
			return col >= GetColCount() - fixedrightcols;
		}

		bool IsFixedRow ( int r ) const
		{
			return IsFixedTopRow ( r ) || IsFixedBtmRow ( r );
		}

		bool IsFixedCol ( int c ) const
		{
			return IsFixedLeftCol ( c ) || IsFixedRightCol ( c );
		}

		bool IsRowInA ( int r ) const
		{
			return IsFixedTopRow ( r );
		}

		bool IsRowInE ( int r ) const
		{
			return !IsFixedTopRow ( r ) && !IsFixedBtmRow ( r );
		}

		bool IsRowInI ( int r ) const
		{
			return IsFixedBtmRow ( r );
		}

		int GetFixedLeftColCount() const
		{
			return fixedleftcols;
		}

		int GetFixedRightColCount() const
		{
			return fixedrightcols;
		}

		int GetFixedTopRowCount() const
		{
			return fixedtoprows;
		}

		int GetFixedBottomRowCount() const
		{
			return fixedbtmrows;
		}

		int GetFixedRowCount() const
		{
			return fixedtoprows + fixedbtmrows;
		}

		CellPoint& GridToCell ( Point p, CellPoint& cp );
		CellPoint GridToCell ( Point p )
		{
			CellPoint q;
			return GridToCell ( p, q );
		}

		int GetColumnIndexAt ( int x ) const
		{
			return cached_data.map.GetColumnIndexAt ( x );
		}

		int GetRowIndexAt ( int y ) const
		{
			return cached_data.map.GetRowIndexAt ( y );
		}

		int GridToFPx ( int x ) const
		{
			return cached_data.map.col.GridToFullPicture ( x );
		}

		int GridToFPy ( int y ) const
		{
			return cached_data.map.row.GridToFullPicture ( y );
		}

		Point GridToFP ( Point p ) const
		{
			return {GridToFPx ( p.x ), GridToFPy ( p.y ) };
		}

		GridEz& ScrollInto ( int row )
		{
			cached_data.map.row.MakeVisible ( row );
			return *this;
		}

		int GetTotalCy()const
		{
			const_cast<GridEz*>(this)->CheckUpdateCache();
			
			ASSERT( cached_data.who == this && cached_data.map.row.ends.GetCount() == GetCount() );

			return cached_data.map.row.end ( GetCount() - 1 );
		}

		int GetTotalCx()const
		{
			const_cast<GridEz*>(this)->CheckUpdateCache();
			ASSERT( cached_data.who == this && cached_data.map.col.ends.GetCount() == GetColCount() );

			return cached_data.map.col.end ( GetColCount() - 1 );
		}

		bool IsCurRow ( int row ) const
		{
			return row == currow;
		}

		int  GetCurRowNo() const
		{
			return currow;
		}

		GridEz& SetCursor ( int row )
		{
			if ( row > 0 && row < GetRowCount() )
				currow = row;

			return *this;
		}

		bool	IsCursor() const
		{
			return currow >= 0;
		}

		int  GetCursor() const
		{
			return GetCurRowNo();
		}
		
		void ResizeRow(int index, int delta);
		
		void ResizeCol(int index, int delta);

		void Copy();

		INCLUDE_CELL_FORMAT_METHODS

	private:
		void AddRowHelper ( Vector<Cell>& vc ) {}

		template <typename T, typename...Types>
		void AddRowHelper ( Vector<Cell>& vc, T t, Types&&...args )
		{
			vc.Add ( Cell ( t ) );
			AddRowHelper ( vc, std::forward<Types>(args)... );
		}
		
		int ColWidth(const Column& col)const;
		int RowHeight(const Row& row)const;

	public:
		Gate1<CellPainter&>					WhenPaintingCell;
		Event<Point&, dword>				WhenLeftClick;
		Event<Point&, dword>				WhenLeftDouble;
		Event<Point&, dword>				WhenMouseMove;
		Event<int, Color&, Color&, dword>	WhenRowColor;
		Event<Bar&>							WhenBar;

	protected:
		JoinedCellManager					joinedCells; // size=16
		ScrollBar		scrollx, scrolly; // size = 200each
		//sizeof(GridEz) = 760 as of 2024/10/12
		CellFormat			format; // size = 32

		Vector<Column>		cols;
		Vector<Row>			rows;

		int32 currow;
		int16 curcol;
		uint8 defaultrowheight, defaultcolwidth;
		uint8 fixedtoprows, fixedbtmrows, fixedleftcols, fixedrightcols;

		bool fixedrowwantcursor :1;
		bool fixedcolwantcursor :1;
		bool curcellvisualclue  :1; // not used yet?
		bool autohidesbs		:1;
		bool fitrows			:1;
		bool fitcolumns			:1;
		bool colinnaturalorder  :1;
		bool rowinnaturalorder  :1;
		bool colcanresize		:1;
		bool rowcanresize		:1;


		void DoColAccumulator();
		void DoRowAccumulator();
		void PreparePainter();
		void ConstructJoinedMatrix();
		bool NextRow();
		bool PrevRow();
		bool NextColumn();
		bool PrevColumn();
		bool PageUp();
		bool PageDown();
		void DoAutoShowHideSbs ( Size& sz, int cx, int cy, bool fitwidth );
		void DoColWidthNSbs( Size& sz );
		
		void CheckUpdateCache()const;


static CachedData cached_data;
//		void GetResizingInfo(OUT int32& index, OUT bool& isRow)
//		{
//#ifdef _DEBUG
//			if(this != who)
//				index = -1;
//			else
//#endif
//			{
//				index = resizing_rc;
//				isRow = is_row;
//			}
//		}
//		static void SetResizingInfo(int32 index, bool isRow
//#ifdef _DEBUG
//				, GridEz * _who
//#endif 
//		){
//			resizing_rc = index;
//			is_row = isRow;
//#ifdef _DEBUG
//			who = _who;
//#endif
//		}
//static CAManager map;
//static	int32 resizing_rc; // index of the col/row in the full picture that's be resized or
//						   // can potentially be resized if left mouse button is down.
//static	int32 position; // remember the last position during continous resizing.
//static  bool  is_row;
//#ifdef _DEBUG
//static  GridEz* who;
//#endif

};


inline	int GridEz::ColWidth(const GridEz::Column& col)const
{
	return col.HasDefaultWidth() ? defaultcolwidth : col.FullWidth();
}

inline	int GridEz::RowHeight(const GridEz::Row& row)const
{
	return row.WithDefaultHeight() ? defaultrowheight : row.Height();
}

template <typename...Types>
GridEz::Row& GridEz::InsertHeightedRow ( int index, int height, Types&&...args )
{
	if constexpr (sizeof...(args)==0 )
	{
		return InsertRow(index,height);
	}else{
		Row& r = InsertRow ( index );
		Vector<Cell> vc;
	
		r.info = new Row::RowData;
		r.info->data_type = DT_LOCAL;
		r.info->data_cnt = sizeof... ( args );
		r.info->Height ( height );
		r.info->cells.Reserve ( sizeof... ( args ) );
		
		(..., r.info->cells.Add(std::forward<Types>(args)) );
		return r;
	}
}

template <typename...Types>
GridEz::Row& GridEz::AddHeightedRow ( int height, Types&&...args )
{
	if constexpr (sizeof...(args)==0)
	{
		ASSERT ( height >= DEFAULT_ROW_HEIGHT );
		Row& r = rows.Add();
		lz::AsInt ( r ) = height == DEFAULT_ROW_HEIGHT ? 0 : ( height << 2 ) + 1;
		return r;
	}else{
		Row& r = rows.Add();
		r.info = new Row::RowData;
		r.info->data_type = DT_LOCAL;
		r.info->data_cnt = sizeof... ( args );
		r.info->Height ( height );
		r.info->cells.Reserve ( sizeof... ( args ) );

		( ..., r.info->cells.Add(std::forward<Types>(args)) );
		return r;
	}
}

inline GridEz& GridEz::Remove ( int r )
{
	// todo: update joined cells information
	rows.Remove ( r );
	return *this;
}

inline GridEz& GridEz::Set ( int row, int col )
{
	rows.At ( row ).At ( col );
	return *this;
}

template <typename T>
inline GridEz& GridEz::Set ( int row, int col, T value )
{
	rows.At ( row ).At ( col ) = Cell ( value );
	return *this;
}


END_NAMESPACE

#endif

