#include "../CtrlLibExt.h"
#define LTIMING RTIMING

#include <algorithm>

namespace lz {
GridEz::CachedData GridEz::cached_data;

GridEz::GridEz()
	: fixedtoprows(0)
	, fixedbtmrows(0)
	, fixedleftcols(0)
	, fixedrightcols(0)
{
	scrollx.Horz().WhenScroll = [this] {
		cached_data.scrollx_changed = true;
		Refresh();
	};
	scrolly.WhenScroll = [this] {
		cached_data.scrolly_changed = true;
		Refresh();
	};
	scrollx.SetLine(5);
	scrolly.SetLine(10);
	scrollx.NoAutoHide();
	scrolly.NoAutoHide();
	AddFrame(scrolly);
	AddFrame(scrollx);
	currow = -1;
	curcol = -1;
	fixedrowwantcursor = false;
	fixedcolwantcursor = false;
	fitcolumns = false;
	fitrows = false;
	highlightcurrow = true;
	rowcanresize = true; // to default to false
	colcanresize = true;
	autohidesbs = true;
	
	DefaultRowHeightFromFont().DefaultColWidthFromFont();
}

bool GridEz::DetermineCellPosition(int screenr, int screenc, int region_index, int r,
                                   CellPainter& di)
{
	int c = cached_data.map.col.visibles[screenc];

	switch(region_index) {
	case -2: // part of a region but not leading Cell, skip drawing
		return false;

	case -1: // non-joined cell
		di.cellrect.left = cached_data.map.Left(c);
		di.cellrect.right = cached_data.map.Right(c);
		di.cellrect.top = cached_data.map.Top(r);
		di.cellrect.bottom = cached_data.map.Bottom(r);
		di.row = r;
		di.col = c;
		cached_data.map.translateRect(di.cellrect, di.visible_rect,
		                              IsFixedTopRow(di.row)   ? 0
		                              : IsFixedBtmRow(di.row) ? 2
		                                                      : 1,
		                              IsFixedLeftCol(di.col)    ? 0
		                              : IsFixedRightCol(di.col) ? 2
		                                                        : 1);

		break;

	default: // first visible Cell in a region.
		ASSERT(region_index >= 0 && region_index < joinedCells.size());
		{
			Rect region = joinedCells[region_index];

			di.cellrect.left = cached_data.map.Left(region.left);
			di.cellrect.top = cached_data.map.Top(region.top);
			di.cellrect.right = cached_data.map.Right(region.right - 1);
			di.cellrect.bottom = cached_data.map.Bottom(region.bottom - 1);

			di.row = region.top;
			di.col = region.left;

			cached_data.map.translateRect(di.cellrect, di.visible_rect,
			                              IsFixedTopRow(region.top)   ? 0
			                              : IsFixedBtmRow(region.top) ? 2
			                                                          : 1,
			                              IsFixedLeftCol(region.left)    ? 0
			                              : IsFixedRightCol(region.left) ? 2
			                                                             : 1,
			                              IsFixedTopRow(region.bottom - 1)   ? 0
			                              : IsFixedBtmRow(region.bottom - 1) ? 2
			                                                                 : 1,
			                              IsFixedLeftCol(region.right - 1)    ? 0
			                              : IsFixedRightCol(region.right - 1) ? 2
			                                                                  : 1);

			// mark cells in the region painted
			for(int k = screenr; k < cached_data.map.row.visibles.size() &&
			                     cached_data.map.row.visibles[k] < region.bottom;
			    ++k)
				for(int m = screenc; m < cached_data.map.col.visibles.size() &&
				                     cached_data.map.col.visibles[m] < region.right;
				    ++m)
					cached_data.map.joined[k][m] = -2; // mark the region as painted
		}
	}
	return true;
}

void GridEz::DetermineCellFormat(const CellFormat& rowf, const CellFormat& colf,
                                 const CellFormat& gridf, CellPainter& di)
{
	di.Compose(rowf, colf, gridf);

	if(IsCurRow(di.row) && HighligthCurRow()) {
		di.Ink(HasFocus() || IsPaintAsFocused(*this) ? SColorHighlightText() : SBlack());
		di.Paper(HasFocus() || IsPaintAsFocused(*this) ? SColorHighlight()
		                                               : Blend(SColorDisabled, SColorPaper));
	}
	else if(IsFixedRow(di.row) || IsFixedCol(di.col))
		di.Paper(SColorFace());

	if(WhenRowColor && IsRowInE(di.row) && !IsCurRow(di.row)) {
		Color ink = di.Ink();
		Color paper = di.Paper();
		WhenRowColor(di.row - GetFixedTopRowCount(), ink, paper, 0);
		di.Ink(ink);
		di.Paper(paper);
	}
}

void GridEz::Paint(Draw& w)
{
	LTIMING("GridEz::Paint");
	Rect rect(GetSize());
	w.DrawRect(rect, SColorPaper());

	if(GetColCount() <= 0 || GetRowCount() == 0 || GetRowCount() < GetFixedRowCount())
		return;

	PreparePainter();

	CellPainter di(w, this);

	// we will need another iteration to remove or hide old, visible Ctrls that are
	// scrolled out of the screen or otherwise become invisible
	// screenr: screen row index; r: full picture row index. screenc, c similar to these.
	for(int screenr = 0; screenr < cached_data.map.row.visibles.GetCount(); ++screenr) {
		auto& vr = cached_data.map.joined[screenr];
		int r = cached_data.map.row.visibles[screenr];
		for(int screenc = 0; screenc < cached_data.map.col.visibles.GetCount(); ++screenc) {
			di.Clear();

			if(!DetermineCellPosition(screenr, screenc, vr[screenc], r, di))
				continue;

			Row& row = rows[di.row];
			Column& col = cols[di.col];

			DetermineCellFormat(row.GetFormat(di.col), col, format, di);

			di.datacol = col.DataColumn();

			di.datarow = -1;

			switch(row.DataStorage()) {

			case DT_EXTERNAL:
				di.datarow = row.RowDataId();

			case DT_NODATA: // now action
				break;

			default: {
				di.ctrl = row.GetCell(di.col).GetCtrl();

				if(di.ctrl) {
					Ctrl& c = *di.ctrl;

					if(!c.HasFocusDeep() || c.GetParent() != this) {
						AddChild(&c);
					}

					c.SetRect(di.cellrect);
				}
				else
					di.text = row.GetText(di.col);
			}
			}
			if(!WhenPaintingCell(di))
				di.Paint();
		}
	}
}

void GridEz::CheckUpdateCache() const
{
	// to be optimized
	const_cast<GridEz*>(this)->PreparePainter();
}

void GridEz::PreparePainter()
{
	Size sz = GetSize();
	//	DUMP(cached_data.who);
	//	DUMP(sz);
	//	DUMP(cached_data.orig_size);

	if(cached_data.who == this && cached_data.orig_size == sz && cached_data.NoFlagIsSet()) {
		LOG(Upp::Format("GridEz::PreparePainter(), cache hit! Who = %p",
		                (void*)cached_data.who));
		return;
	}
	else {
		cached_data.who = this;
		cached_data.orig_size = sz;
		cached_data.FlagMarkAll();
	}

	//	if(cached_data.x_dimension_recalculate || cached_data.y_dimension_recalculate)
	{
		DoColWidthNSbs(sz);
		//		if(cached_data.x_dimension_recalculate)
		DoColAccumulator();
		//		if(cached_data.y_dimension_recalculate)
		DoRowAccumulator();
	}

	cached_data.map.Set(sz, scrollx, scrolly, fixedleftcols, fixedrightcols, fixedtoprows,
	                    fixedbtmrows);

	cached_data.map.col.UpdateVisibles(fixedleftcols, fixedrightcols, sz.cx);

	cached_data.map.row.UpdateVisibles(fixedtoprows, fixedbtmrows, sz.cy);

	if(scrollx.GetSize() != Size{0, 0}) {
		scrollx.SetTotal(cached_data.map.WidthOfEInFullPicture());

		scrollx.SetPage(cached_data.map.WidthOfEInGrid());
	}

	if(scrolly.GetSize() != Size{0, 0}) {
		scrolly.SetTotal(cached_data.map.HeightOfEInFullPicture());

		scrolly.SetPage(cached_data.map.HeightOfEInGrid());
	}

	// construct the cached_data.map.joined matrix. see help file for
	// the meaning of the member var 'joined'.
	ConstructJoinedMatrix();
	//	cached_data.FlagClearAll();
}

void GridEz::ConstructJoinedMatrix()
{
	cached_data.map.joined.SetCount(cached_data.map.row.visibles.size());
	for(auto& r : cached_data.map.joined) {
		r.Clear();
		r.SetCount(cached_data.map.col.visibles.size(), -1);
	}

	for(int i = 0; i < joinedCells.size(); ++i) {
		Rect region = joinedCells[i];
		--region.bottom;
		--region.right;

		for(int j = 0; j < cached_data.map.row.visibles.size(); ++j) {
			if(cached_data.map.row.visibles[j] < region.top)
				continue;

			if(cached_data.map.row.visibles[j] > region.bottom)
				break;

			for(int k = 0; k < cached_data.map.col.visibles.size(); ++k) {
				if(cached_data.map.col.visibles[k] < region.left)
					continue;

				if(cached_data.map.col.visibles[k] > region.right)
					break;

				cached_data.map.joined[j][k] = i; // i is the index of a region
			}
		}
	}
}

void GridEz::DoColAccumulator()
{
	int t = 0, w;
	CAManager::Accum& _colacc = cached_data.map.col;

	_colacc.ends.SetCount(GetColCount());

	for(int i = 0; i < GetColCount(); ++i) {
		w = cols[i].Width();
		t += w;
		_colacc.ends[i] = t;
	}
}

void GridEz::DoRowAccumulator()
{
	int t = 0, w;
	CAManager::Accum& _rowacc = cached_data.map.row;
	t = 0;

	_rowacc.ends.SetCount(GetRowCount());

	for(int i = 0; i < GetRowCount(); ++i) {
		w = rows[i].Height() == DEFAULT_ROW_HEIGHT ? defaultrowheight : rows[i].Height();
		t += w;
		_rowacc.ends[i] = t;
	}
}

GridEz& GridEz::SetRowCount(int n)
{
	if(n != GetRowCount() && n >= fixedtoprows + fixedbtmrows) {
		rows.SetCount(n);
		Refresh();
	}

	return *this;
}

GridEz& GridEz::SetColCount(int n)
{
	if(n != GetColCount() && n >= fixedleftcols + fixedrightcols) {
		cols.SetCount(n);
		Refresh();
	}

	return *this;
}

GridEz& GridEz::SetFixedLeftColCount(int n)
{
	ASSERT(n >= 0);

	if(n != fixedleftcols) {
		if(n + fixedrightcols > this->GetColCount())
			SetColCount(n + fixedrightcols);

		fixedleftcols = n;

		Refresh();
	}

	return *this;
}

GridEz& GridEz::SetFixedRightColCount(int n)
{
	ASSERT(n >= 0);

	if(n != fixedrightcols) {
		if(n + fixedleftcols > this->GetColCount())
			SetColCount(n + fixedleftcols);

		fixedrightcols = n;

		Refresh();
	}

	return *this;
}

GridEz& GridEz::SetFixedTopRowCount(int n)
{
	ASSERT(n >= 0);

	if(n != fixedtoprows) {
		fixedtoprows = n;
		Refresh();
	}

	return *this;
}

GridEz& GridEz::SetFixedBottomRowCount(int n)
{
	ASSERT(n >= 0);

	if(n != fixedbtmrows) {
		fixedbtmrows = n;
		Refresh();
	}

	return *this;
}

GridEz::Column& GridEz::AddColumn(int width, int dataid)
{
	Column& c = cols.Add();
	c.DataColumn(dataid);
	c.Width(width);
	return c;
}

GridEz::Column& GridEz::AddColumn(String name, int width, int dataid)
{
	Column& c = cols.Add();
	c.DataColumn(dataid);
	c.Width(width);

	if(GetFixedTopRowCount() < 1)
		SetFixedTopRowCount(1);

	if(GetRowCount() == 0) {
		AddRow(name).GetRowFormat().Bold(tribool::True);
	}

	else
		rows[0].At(cols.GetCount() - 1) = name;

	return c;
}

GridEz::Column& GridEz::AddColumn(StrLiteral name, int width, int dataid)
{
	Column& c = cols.Add();
	c.DataColumn(dataid);
	c.Width(width);

	if(GetFixedTopRowCount() < 1)
		SetFixedTopRowCount(1);

	if(GetRowCount() == 0) {
		AddRow(name).GetRowFormat().Bold(tribool::True);
	}

	else
		rows[0].At(cols.GetCount() - 1) = name;

	return c;
}

// GridEz::Row& GridEz::AddRow ( int height )
//{
// }

GridEz::Row& GridEz::AddExternalRow(uint64 rowdataid, int height)
{
	ASSERT(height >= DEFAULT_ROW_HEIGHT);
	ASSERT(rowdataid != -1);
	return rows.Add().StoreDataIDAndHeight(rowdataid, height);
}

GridEz::Row& GridEz::InsertRow(int index, int height)
{
	ASSERT(height >= DEFAULT_ROW_HEIGHT);
	Row* p = index >= GetCount() ? &rows.At(index) : &rows.Insert(index);
	lz::AsInt(*p) = height == DEFAULT_ROW_HEIGHT ? 0 : (height << 2) + 1;
	return *p;
}

GridEz::Row& GridEz::InsertExternalRow(int index, uint64 rowid, int height)
{
	ASSERT(height >= DEFAULT_ROW_HEIGHT);
	ASSERT(rowid != -1);
	Row* p = index >= GetCount() ? &rows.At(index) : &rows.Insert(index);
	return (*p).StoreDataIDAndHeight(rowid, height);
}

// joined regions are ordered by
// top, left (One cell cannot belong to more than 1 block of joined cells)
//
// user to guarantee joined regions don't overlap
//
GridEz& GridEz::JoinCells(int startcol, int startrow, int countcol, int countrow)
{
	joinedCells.Add(startcol, startrow, startcol + countcol, startrow + countrow);

	return *this;
}

GridEz& GridEz::UnjoinCells(int col, int row)
{
	joinedCells.RemoveRegionContainsCell(col, row);
	return *this;
}

Value GridEz::Get0(int row, int col) const
{
	Value v;

	if(row < 0) {
		if(currow >= 0)
			row = currow;
		else
			return Null;
	}

	const Row& r = rows[row];

	switch(r.DataStorage()) {

	case DT_EXTERNAL:
		// di.datarow = row.RowDataId();
		return Null; // for now

	case DT_NODATA:  // now action
		return Null; // fow now
		break;

	default:
		v = r.GetData(col);
	}

	return v;
}

GridEz& GridEz::Clear(bool columns)
{
	rows.Clear();
	joinedCells.Clear();
	scrollx.Set(0);
	scrolly.Set(0);
	scrolly.SetTotal(0);
	scrollx.SetTotal(0);
	// cached_data.map.Clear();

	if(columns)
		cols.Clear();

	return *this;
}

GridEz& GridEz::ClearNonfixedRows()
{
	// to do: update joined cells info

	rows.Remove(fixedtoprows, GetCount() - GetFixedRowCount());

	scrolly.Set(0);
	scrolly.SetTotal(0);
	return *this;
}

void GridEz::State(int reason)
{
	if(reason == FOCUS)
		Refresh();
}

void GridEz::MouseWheel(Point, int zdelta, dword) { scrolly.Wheel(zdelta); }

void GridEz::LeftDown(Point p, dword flags)
{
	if(cached_data.resizing_rc != -1) {
		SetCapture();
		if(cached_data.is_row) {
			cached_data.orig_position = p.y;
			cached_data.orig_dimension = GetRowHeight(cached_data.resizing_rc);
		}
		else {
			cached_data.orig_position = p.x;
			cached_data.orig_dimension = GetColFullWidth(cached_data.resizing_rc);
			if(IsFitColumns()) {
				cached_data.orig_dimensions.SetCount(GetColCount() - cached_data.resizing_rc -
				                                     1);
				for(int i = cached_data.resizing_rc + 1, j = 0; i < GetColCount(); ++i, ++j)
					cached_data.orig_dimensions[j] = GetColFullWidth(i);
			}
		}
		cached_data.orig_position = cached_data.is_row ? p.y : p.x;
	}
	auto cp = GridToCell(p);

	if(cp.row != -1 && cp.row != currow && (IsFixedRowWantCursor() || !IsFixedRow(cp.row))) {
		currow = cp.row;
		Refresh();
	}

	if(cp.col != -1 && cp.col != curcol && (IsFixedColWantCursor() || !IsFixedCol(cp.col))) {
		curcol = cp.col;
		Refresh();
	}

	if(IsWantFocus())
		SetFocus();

	if(WhenLeftClick) {
		WhenLeftClick(p, flags);
	}
}

void GridEz::LeftDouble(Point p, dword flags)
{
	if(WhenLeftDouble) {
		WhenLeftDouble(p, flags);
	}
}

void GridEz::RightDown(Point p, dword flags)
{
	if(WhenBar)
		MenuBar::Execute(WhenBar);
}

void GridEz::MouseMove(Point p, dword flags)
{
	if(HasCapture()) {
		if(cached_data.resizing_rc != -1) {
			int new_position;

			if(cached_data.is_row) {
				new_position = p.y;
				if(IsFixedBtmRow(cached_data.resizing_rc)) {
					ResizeRow(cached_data.resizing_rc, cached_data.orig_dimension -
					                                       cached_data.orig_position +
					                                       new_position);
				}
				else {
					ResizeRow(cached_data.resizing_rc, cached_data.orig_dimension +
					                                       new_position -
					                                       cached_data.orig_position);
				}
			}
			else {
				new_position = p.x;
				if(IsFixedRightCol(cached_data.resizing_rc)) {
					ResizeCol(cached_data.resizing_rc,
					          cached_data.orig_position - new_position);
				}
				else {
					ResizeCol(cached_data.resizing_rc,
					          new_position - cached_data.orig_position);
				}
			}
		}
	}
	else if(WhenMouseMove) {
		WhenMouseMove(p, flags);
	}
}

Image GridEz::CursorImage(Point p, dword)
{
	if(!HasCapture()) {
		if(int col; colcanresize && (col = cached_data.map.AtRightBoundOfColumn(p.x)) != -1 &&
		            IsColResizable(col)) {
			if(IsFixedRightCol(col) && col < GetColCount() - 1)
				cached_data.resizing_rc = col + 1;
			else
				cached_data.resizing_rc = col;
			cached_data.is_row = false;
		}
		else if(int row;
		        rowcanresize && (row = cached_data.map.AtBottomBoundOfRow(p.y)) != -1) {
			cached_data.resizing_rc = row;
			cached_data.is_row = true;
		}
		else [[likely]]
			cached_data.resizing_rc = -1;
	}
	return cached_data.resizing_rc == -1 ? Image::Arrow()
	       : cached_data.is_row          ? Image::SizeVert()
	                                     : Image::SizeHorz();
}

GridEz::CellPoint& GridEz::GridToCell(Point p, GridEz::CellPoint& cp)
{
	cp.row = GetRowIndexAt(p.y);
	cp.col = GetColumnIndexAt(p.x);

	if(cp.row != -1 && cp.col != -1) {
		p = GridToFP(p);
		Point tl(cached_data.map.col.start(cp.col), cached_data.map.row.start(cp.row));

		cp.point = p - tl;
	}

	else
		cp.point = Null;

	return cp;
}

bool GridEz::Key(dword key, int flags)
{
	switch(key) {

	case K_DOWN:
		return NextRow();

	case K_UP:
		return PrevRow();

	case K_LEFT:
		return PrevColumn();

	case K_RIGHT:
		return NextColumn();

	case K_PAGEUP:
		return PageUp();

	case K_PAGEDOWN:
		return PageDown();
	}

	return Ctrl::Key(key, flags);
}

static bool NextRC(int rc_cnt, int fixedTop, int fixedBtm, int32& curRow, bool fixedWantCursor)
{
	bool set = false;
	int fixed = fixedTop + fixedBtm;

	if(curRow < 0) {
		if(!fixedWantCursor) {
			if(rc_cnt > fixed) {
				curRow = fixedTop;
				set = true;
			}
		}

		else if(rc_cnt > 0) {
			curRow = 0;
			set = true;
		}
	}

	else {
		if((!fixedWantCursor && curRow < rc_cnt - fixedBtm - 1) ||
		   (fixedWantCursor && curRow < rc_cnt - 1)) {
			++curRow;
			set = true;
		}
	}

	return set;
}

static bool PrevRC(int rc_cnt, int fixedTop, int fixedBtm, int32& curRow, bool fixedWantCursor)
{
	bool set = false;
	int fixed = fixedTop + fixedBtm;

	if(curRow < 0) {
		if(!fixedWantCursor) {
			if(rc_cnt > fixed) {
				curRow = rc_cnt - fixedBtm - 1;
				set = true;
			}
		}

		else if(rc_cnt > 0) {
			curRow = rc_cnt - 1;
			set = true;
		}
	}

	else {
		if((!fixedWantCursor && curRow > fixedTop) || (fixedWantCursor && curRow > 0)) {
			--curRow;
			set = true;
		}
	}

	return set;
}

bool GridEz::NextRow()
{
	if(NextRC(GetRowCount(), GetFixedTopRowCount(), GetFixedBottomRowCount(), currow,
	          IsFixedRowWantCursor())) {
		scrolly.Set(cached_data.map.row.MakeVisible(currow));
		Refresh();
		return true;
	}

	BeepError();
	return true; // do not allow arrow navigate to next Ctrl for now!

	//	return false;
}

bool GridEz::PrevRow()
{
	if(PrevRC(GetRowCount(), GetFixedTopRowCount(), GetFixedBottomRowCount(), currow,
	          IsFixedRowWantCursor())) {
		scrolly.Set(cached_data.map.row.MakeVisible(currow));
		Refresh();
		return true;
	}

	BeepError();
	return true; // disallow arrow navigate to previous Ctrl for now!
	             //	return false;
}

bool GridEz::NextColumn()
{
	int32 col = curcol;

	if(NextRC(GetColCount(), GetFixedLeftColCount(), GetFixedRightColCount(), col,
	          IsFixedColWantCursor())) {
		curcol = (int16)col;
		scrollx.Set(cached_data.map.col.MakeVisible(col));
		Refresh();
		return true;
	}

	return false;
}

bool GridEz::PrevColumn()
{
	int32 col = curcol;

	if(PrevRC(GetColCount(), GetFixedLeftColCount(), GetFixedRightColCount(), col,
	          IsFixedColWantCursor())) {
		curcol = (int16)col;
		scrollx.Set(cached_data.map.col.MakeVisible(col));
		Refresh();
		return true;
	}

	return false;
}

bool GridEz::PageUp()
{
	int ycursor;
	//	int section;
	int y;
	int newscroll;
	// for now, ignore the case when fixed rows will accept cursor.

	if(cached_data.map.row.dimE == 0)
		return false;

	if(currow == -1)
		currow = cached_data.map.row.visibles[cached_data.map.row.visibles.GetCount() -
		                                      GetFixedBottomRowCount() - 1];

	ycursor = cached_data.map.Top(currow);

	while(--currow >= GetFixedTopRowCount()) {
		y = cached_data.map.Top(currow);

		if(ycursor - y > cached_data.map.row.dimE) {
			++currow;
			break;
		}
	}

	if(currow < GetFixedTopRowCount())
		++currow;

	newscroll = cached_data.map.row.scroll + cached_data.map.Top(currow) - ycursor;

	scrolly.Set(newscroll);

	cached_data.map.row.scroll = newscroll;

	Refresh();

	return true;
}

bool GridEz::PageDown()
{
	int ycursor;
	//	int section;
	int y;
	int newscroll;
	// for now, ignore the case when fixed rows will accept cursor.

	if(cached_data.map.row.dimE == 0)
		return false;

	if(currow == -1)
		currow = cached_data.map.row.visibles[GetFixedTopRowCount()];

	ycursor = cached_data.map.Bottom(currow);

	while(++currow < GetRowCount() - GetFixedBottomRowCount()) {
		y = cached_data.map.Bottom(currow);

		if(y - ycursor > cached_data.map.row.dimE) {
			--currow;
			break;
		}
	}

	if(currow >= GetRowCount() - GetFixedBottomRowCount())
		--currow;

	newscroll = cached_data.map.row.scroll + cached_data.map.Bottom(currow) - ycursor;

	scrolly.Set(newscroll);

	cached_data.map.row.scroll = newscroll;

	Refresh();

	return true;
}

void GridEz::Copy()
{
	String s;

	for(int i = GetFixedRowCount(); i < GetRowCount() - GetFixedBottomRowCount(); ++i) {
		if(i != 0)
			s << '\n';

		Row& r = (*this)[i];

		if(r.IsEmpty())
			continue;

		for(int j = 0; j < GetColCount(); ++j) {
			if(j != 0)
				s << '\t';

			s << r.GetText(j);
		}
	}

	WriteClipboardText(s);
}

void GridEz::DoColWidthNSbs(Size& sz)
{
	int tcx = 0, tcy = 0, total_weight = 0;
	bool fw = IsFitColumns();
	bool ahs = autohidesbs;

	if(ahs) {
		sz.cx += scrolly.GetSize().cx; // add back dimensions possibly taken by scrollbars
		sz.cy += scrollx.GetSize().cy;
	}

	for(const auto& col : cols) // sum up total width and total weight
	{
		if(!col.IsHidden()) {
			tcx += ColWidth(col);
			if(fw)
				total_weight += col.ScaleWeight();
		}
	}

	if(ahs) {
		for(const auto& row : rows)
			tcy += RowHeight(row);
		DoAutoShowHideSbs(sz, tcx, tcy, fw);
	}

	for(int delta = sz.cx - tcx; auto& col : cols) {
		if(!fw || total_weight == 0)
			col.SetWidthInternal(ColWidth(col));
		else {
			auto w = ColWidth(col) + static_cast<int>(static_cast<double>(col.ScaleWeight()) /
			                                          total_weight * delta);
			if(w < 0)
				w = 0;
			col.SetWidthInternal(w);
		}
	}
	cached_data.map.col.total_weight = total_weight;
	// cached_data.width_changed = false;
	// cached_data.height_changed = false;
}

void GridEz::DoAutoShowHideSbs(Size& sz, int tcx, int tcy, bool fitwidth)
{
	bool showscrlx = false, showscrly = false;

	const int barsize = ScrollBarSize(); // scrollx.GetStyle().barsize;

	if(fitwidth) {
		if(tcy > sz.cy)
			showscrly = true;
	}
	else if(tcx > sz.cx && tcy <= sz.cy - barsize)
		showscrlx = true;
	else if(tcy > sz.cy && tcx <= sz.cx - barsize)
		showscrly = true;
	else if(tcx > sz.cx || tcy > sz.cy)
		showscrlx = showscrly = true;

	if(showscrlx)
		sz.cy -= barsize;

	if(showscrly)
		sz.cx -= barsize;

	scrollx.Show(showscrlx);
	scrolly.Show(showscrly);
}

void GridEz::ResizeRow(int index, int new_height)
{
	int h = GetRowHeight(index);

	if(new_height < 0)
		new_height = 0;

	if(index >= rows.GetCount() || index < 0 || new_height == h)
		return;

	if(!fitrows) {
		rows[index].Height(new_height);
	}
	else {
	}
	cached_data.y_dimension_recalculate = true;
	Refresh();
}

void GridEz::ResizeCol(int index, int change_in_new_width)
{
	int new_width = cached_data.orig_dimension + change_in_new_width;
	if(new_width < 0)
		new_width = 0;
	int h = GetColFullWidth(index);

	if(index >= cols.GetCount() || index < 0 || h == new_width)
		return;

	bool needrefresh = false;
	if(!fitcolumns || index != cols.GetCount() - 1) {
		cols[index].Width(new_width);

		needrefresh = true;
	}

	if(fitcolumns &&
	   index != cols.GetCount() - 1) // the very last column cannot be adjusted this way
	{
		int old_fullwidth = cached_data.orig_dimension;
		if(new_width < 0)
			new_width = 0;
		int delta = change_in_new_width;

		int total_weight = 0; // total weight of scaleweight of columns following this column;
		int total_width = 0;  // total naturalwidth of columns following this column
		int accum_weight = 0; // increase when each affected column is process;
		int accum_delta = 0;  //
		for(int i = index + 1; i < cols.GetCount(); ++i) {
			if(int sw = cols[i].ScaleWeight(); sw != 0) {
				total_weight += sw;
				total_width += GetColFullWidth(i);
			}
		}

		if(total_weight == 0) // none of following columns allows resizing
		{
			// change_in_new_width = 0;
			// request to change size vetoed
			return;
		}
		else if(delta > total_width) {
			delta = total_width; // maximum allowed
		}

		cols[index].FullWidth(old_fullwidth + delta);
		for(int i = index + 1; i < cols.GetCount(); ++i) {
			accum_weight += cols[i].ScaleWeight();
			int this_delta = delta * accum_weight / total_weight - accum_delta;
			new_width = cached_data.orig_dimensions[i - index - 1] - this_delta;
			if(new_width < 0) {
				this_delta -= new_width;
				new_width = 0;
			}
			cols[i].FullWidth(new_width);
			accum_delta += this_delta;
		}
		needrefresh = true;
	}
	if(needrefresh) {
		cached_data.x_dimension_recalculate = true;
		Refresh();
	}
}

END_NAMESPACE
