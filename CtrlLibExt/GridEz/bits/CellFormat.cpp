#include <CtrlLibExt/CtrlLibExt.h>
//#include "../GridEz.h"
//#include <cstring>

BEGIN_NAMESPACE_LZ

CellFormat::CellFormat()
	: font(StdFont())
	, bg(Null)
	, fg(Null)
	, bordercolor(Null)
{
	Ink(SColorText())
		.Paper(SColorPaper())
		.Font(StdFont())
		.BorderTop(BORDER_SINGLE)
		.BorderLeft(BORDER_SINGLE)
		.BorderRight(BORDER_SINGLE)
		.BorderBottom(BORDER_SINGLE)
		.BorderColor(SLtGray())
		.HorzAlign(ALIGN_LEFT)
		.VertAlign(ALIGN_CENTER)
		.Padding(1);
}

const CellFormat& CellFormat::NullCellFormat()
{
	static const CellFormat f(nullptr);
	return f;
}

CellFormat& CellFormat::DefaultCellFormat()
{
	static CellFormat f;
	return f;
}

bool CellFormat::AllPropertiesSet()const
{
	return !IsNull(bg) && !IsNull(fg) && !IsNull(bordercolor) &&
		!IsNull(PaddingLeft()) && !IsNull(PaddingRight()) && !IsNull(PaddingTop()) &&
		!IsNull(PaddingBottom()) && flags.borderLeft != BORDER_NULL &&
		flags.borderRight != BORDER_NULL && flags.borderTop != BORDER_NULL &&
		flags.borderBottom != BORDER_NULL && flags.halign != ALIGN_NULL &&
		flags.valign != ALIGN_NULL && AllFontPropertiesSet();
		
}

bool CellFormat::NoPropertiesSet()const
{
	return IsNull(bg) && IsNull(fg) && IsNull(bordercolor) &&
		*reinterpret_cast<const uint64*>(&paddingLeft) == std::numeric_limits<uint64>::max() &&
			flags.AllPropertiesNull();
}

bool CellFormat::operator == (const Self& rhs)const
{
	return fg == rhs.fg && bg == rhs.bg && bordercolor == rhs.bordercolor &&
		*(int64*)&paddingLeft == *(int64*)&rhs.paddingLeft &&
//		((int32*)&paddingTop)[1] == ((int32*)&rhs.paddingTop)[1] &&
		FaceName() == rhs.FaceName() && FontHeight() == rhs.FontHeight() &&
		FontWidth() == rhs.FontWidth() && Bold() == rhs.Bold() &&
		Italic() == rhs.Italic() && Underline() == rhs.Underline() &&
		Strikeout() == rhs.Strikeout();
}

void CellPainter::Paint()
{
	Rect r(cellrect);

	r.left += PaddingLeft(),		r.right  -= PaddingRight(),
	r.top  += PaddingTop() ,		r.bottom -= PaddingBottom();
	if(r.left >= r.right || r.top >= r.bottom)
		return;

	w.Clip(visible_rect);
	w.DrawRect(visible_rect, Paper());
	DrawBorders();


	if(ctrl != nullptr) {
//		if(cellrect != visible_rect)
//		{
//			CheckBox(*ctrl, cellrect, visible_rect);
//		}else if(IsBoxed(*ctrl))
//			Unbox(*ctrl);

//		ctrl->SetRect(cellrect-cellrect.BottomRight());
//		ctrl->DrawCtrl(w,cellrect.left,cellrect.top);
	}
	else if(!text.IsEmpty()) {
		Size sz = Upp::GetSmartTextSize(text, Font(), r.GetWidth());
		int x = r.left, y = r.top;
		switch(HorzAlign()) {
		case ALIGN_CENTER:
			x += (r.Width() - sz.cx) / 2;
			break;
		case ALIGN_RIGHT:
			x += r.Width() - sz.cx;
			break;
		}
		switch(VertAlign()) {
		case ALIGN_CENTER:
			y += (r.Height() - sz.cy) / 2;
			break;
		case ALIGN_BOTTOM:
			y += r.Height() - sz.cy;
			break;
		}
		Upp::DrawSmartText(w, x, y, r.GetWidth(), text, Font(), Ink());
	}

	w.End();
}

void CellPainter::DrawBorders()
{
	int delta, border;
	delta = col == 0 ? 1 : 0;
	switch(border = BorderLeft()) {
	case BORDER_DOUBLE: // double
		w.DrawLine(cellrect.left + 2 + delta, cellrect.top, cellrect.left + 2 + delta,
		           cellrect.bottom, 1, BorderColor());
		[[fallthrough]];
	case BORDER_SINGLE:
	case BORDER_THICK: // single
		w.DrawLine(cellrect.left + delta, cellrect.top, cellrect.left + delta, cellrect.bottom,
		           border == BORDER_THICK ? 2 : 1, BorderColor());
		break;
	}
	delta = row == 0 ? 1 : 0;
	switch(border = BorderTop()) {

	case BORDER_DOUBLE: // double
		w.DrawLine(cellrect.left, cellrect.top + 2, cellrect.right, cellrect.top + 2, 1,
		           BorderColor());
		[[fallthrough]];
	case BORDER_SINGLE:
	case BORDER_THICK: // single
		w.DrawLine(cellrect.left, cellrect.top, cellrect.right, cellrect.top,
		           border == BORDER_THICK ? 2 : 1, BorderColor());
		break;
	}
	// delta=col==?1:0;
	delta = -1; //???
	switch(border = BorderRight()) {
	case BORDER_DOUBLE: // 4
		w.DrawLine(cellrect.right - 2 + delta, cellrect.top, cellrect.right - 2 + delta,
		           cellrect.bottom, 1, BorderColor());
		[[fallthrough]];
	case BORDER_SINGLE:
	case BORDER_THICK: // single
		w.DrawLine(cellrect.right + delta, cellrect.top, cellrect.right + delta,
		           cellrect.bottom, border == BORDER_THICK ? 2 : 1, BorderColor());
		break;
	}
	// delta=row==0?1:0;
	switch(border = BorderBottom()) {

	case BORDER_DOUBLE: // double

		w.DrawLine(cellrect.left, cellrect.bottom - 2 + delta, cellrect.right,
		           cellrect.bottom - 2 + delta, 1, BorderColor());
		[[fallthrough]];
	case BORDER_SINGLE:
	case BORDER_THICK: // single
		w.DrawLine(cellrect.left, cellrect.bottom + delta, cellrect.right,
		           cellrect.bottom + delta, border == BORDER_THICK ? 2 : 1, BorderColor());
		break;
	}
}

#define CELL_FORMAT_COMPOSE(prop_name, r, c, g)                                                \
	(!IsNull(r.prop_name())   ? r.prop_name()                                                  \
	 : !IsNull(c.prop_name()) ? c.prop_name()                                                  \
	                          : g.prop_name())
#define CELL_FORMAT_FLAGGED_COMPOSE(flag, prop_name, r, c, g)\
	prop_name(r.flag ? r.prop_name() : c.flag ? c.prop_name() : g.prop_name())

void CellFormat::Compose(const CellFormat& row, const CellFormat& col, const CellFormat& grid)
{
	Ink(CELL_FORMAT_COMPOSE(Ink, row, col, grid));
	Paper(CELL_FORMAT_COMPOSE(Paper, row, col, grid));
	BorderColor(CELL_FORMAT_COMPOSE(BorderColor, row, col, grid));

	Bold(CELL_FORMAT_COMPOSE(Bold, row, col, grid));

	int b = row.BorderLeft();
	if(b == BORDER_NULL) {
		b = col.BorderLeft();
		if(b == BORDER_NULL)
			b = grid.BorderLeft();
	}
	BorderLeft(b);

	b = row.BorderRight();
	if(b == BORDER_NULL) {
		b = col.BorderRight();
		if(b == BORDER_NULL)
			b = grid.BorderRight();
	}
	BorderRight(b);

	b = row.BorderTop();
	if(b == BORDER_NULL) {
		b = col.BorderTop();
		if(b == BORDER_NULL)
			b = grid.BorderTop();
	}
	BorderTop(b);

	b = row.BorderBottom();
	if(b == BORDER_NULL) {
		b = col.BorderBottom();
		if(b == BORDER_NULL)
			b = grid.BorderBottom();
	}
	BorderBottom(b);

	b = row.HorzAlign();
	if(b == ALIGN_NULL) {
		b = col.HorzAlign();
		if(b == ALIGN_NULL)
			b = grid.HorzAlign();
	}
	HorzAlign(b);

	b = row.VertAlign();
	if(b == ALIGN_NULL) {
		b = col.VertAlign();
		if(b == ALIGN_NULL)
			b = grid.VertAlign();
	}
	VertAlign(b);

	PaddingLeft(CELL_FORMAT_COMPOSE(PaddingLeft, row, col, grid));
	PaddingRight(CELL_FORMAT_COMPOSE(PaddingRight, row, col, grid));
	PaddingTop(CELL_FORMAT_COMPOSE(PaddingTop, row, col, grid));
	PaddingBottom(CELL_FORMAT_COMPOSE(PaddingBottom, row, col, grid));
	CELL_FORMAT_FLAGGED_COMPOSE(flags.faceNotNull, FaceName, row, col, grid);
	CELL_FORMAT_FLAGGED_COMPOSE(flags.boldNotNull, Bold, row, col, grid);
	CELL_FORMAT_FLAGGED_COMPOSE(flags.heightNotNull, FontHeight, row, col, grid);
	CELL_FORMAT_FLAGGED_COMPOSE(flags.widthNotNull, FontWidth, row, col, grid);
	CELL_FORMAT_FLAGGED_COMPOSE(flags.underlineNotNull, Underline, row, col, grid);
	CELL_FORMAT_FLAGGED_COMPOSE(flags.italicNotNull, Italic, row, col, grid);
	CELL_FORMAT_FLAGGED_COMPOSE(flags.strikeoutNotNull, Strikeout, row, col, grid);
}
#undef CELL_FORMAT_FLAGGED_COMPOSE
#undef CELL_FORMAT_COMPOSE


END_NAMESPACE
