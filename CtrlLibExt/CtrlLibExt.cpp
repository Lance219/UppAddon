#include "CtrlLibExt.h"
BEGIN_NAMESPACE_LZ

void   FakeEdit::Paint(Draw& w, Size sz)
{
	int lspace = GetSpaceLeft();
	int rspace = GetSpaceRight();
//	bool enabled = IsShowEnabled();
	Color paper = SColorPaper(); //GetPaper();
	Color textcolor = SColorText(); //GetColorAttr(ATTR_TEXTCOLOR);
	Color ink = textcolor;//enabled ? Nvl(textcolor, style->text) : style->textdisabled;
	int fcy = StdFont().GetCy(); //font.GetCy();
	int yy = GetTy(sz.cy);
	w.DrawRect(sz, paper);
	PaintSpace(w);
	if(!no_internal_margin) {
		lspace += 2;
		rspace += 2;
	}
	if(lspace || rspace)
		w.Clipoff(lspace, no_internal_margin ? 0 : yy, sz.cx - lspace - rspace, no_internal_margin ? 0 : fcy);
	int x = -sc;
	String nulltext = GetTextAttr(ATTR_NULLTEXT);
	Image nullicon = GetAttr<Image>(ATTR_NULLICON);
	if(IsNull(text) && (!IsNull(nulltext) || !IsNull(nullicon))) {
		x = 0;
		WString nt = nulltext.ToWString();
		const wchar *txt = nt;
		if(!IsNull(nullicon)) {
			int icx = nullicon.GetWidth();
			w.DrawRect(x, 0, icx + 4, fcy, paper);
			w.DrawImage(x, (fcy - nullicon.GetHeight()) / 2, nullicon);
			x += icx + 4;
		}
		Paints(w, x, fcy, txt, Nvl(GetColorAttr(ATTR_NULLINK), SColorDisabled()),
		       paper, nt.GetLength(), false, Nvl(GetFontAttr(ATTR_NULLFONT), StdFont().Italic()), Null, false);
	}
	else {
		const wchar *txt = text;
		int len = GetLength();
		Vector<Upp::EditField::Highlight> hl;
		hl.SetCount(len);
		for(int i = 0; i < len; i++) {
			hl[i].ink = ink;
			hl[i].paper = paper;
			hl[i].underline = Null;
		}
		HighlightText(hl);
		len = hl.GetCount();
		int l, h;
		if(GetSelection(l, h)) {
			h = min(h, len);
			for(int i = l; i < h; i++) {
				hl[i].ink = HasFocus() ? EditField::StyleDefault().selectedtext : EditField::StyleDefault().selectedtext0;
				hl[i].paper = HasFocus() ? EditField::StyleDefault().selected : EditField::StyleDefault().selected0;
			}
		}
		int b = 0;
		for(int i = 0; i <= len; i++)
			if((i == len || hl[i] != hl[b]) && b < len) {
				Paints(w, x, fcy, txt, hl[b].ink, hl[b].paper, i - b, password, StdFont(), hl[b].underline, showspaces);
				b = i;
			}
	}
	DrawTiles(w, dropcaret, CtrlImg::checkers());
	if(lspace || rspace)
		w.End();
}

void FakeEdit::Paints(Draw& w, int& x, int fcy, const wchar *&txt,
					   Color ink, Color paper, int n, bool password, Font fnt, Color underline,
					   bool showspaces)
{
	if(n < 0) return;
	int cx = GetTextCx(txt, n, password);
	w.DrawRect(x, 0, cx, fcy, paper);
	if(password) {
		String h;
		h.Cat('*', n);
		w.DrawText(x, 0, ~h, fnt, ink, n);
	}
	else {
		const wchar *txts = txt;
		Buffer<wchar> h;
		const wchar *e = txt + n;
		for(const wchar *q = txt; q < e; q++)
			if(*q < 32) {
				h.Alloc(n);
				wchar *t = ~h;
				for(const wchar *q = txt; q < e; q++)
					*t++ = *q < 32 ? LowChar(*q) : *q;
				txts = ~h;
			}
		if(!IsNull(underline))
			w.DrawRect(x, fnt.GetAscent() + 1, cx, 1, underline);
		w.DrawText(x, 0, txts, fnt, ink, n);
		if(showspaces) {
			int xx = x;
			Size sz = GetTextSize(" ", fnt) / 2;
			e = txts + n;
			for(const wchar *q = txts; q < e; q++) {
				if(*q == ' ')
					w.DrawRect(xx + sz.cx, sz.cy, 2, 2, Blend(SColorHighlight(), SColorPaper()));
				xx += fnt[*q];
			}
		}
	}
	txt += n;
	x += cx;
}

void FakeEdit::LeftDown(Point p, dword flags)
{
	int c = GetCursor(p.x + sc);
	if(!HasFocus()) {
		SetFocus();
		if(clickselect) {
			SetSelection();
			Finish();
			return;
		}
		sc = 0;
		Move(c);
	}
	int l, h;
	selclick = false;
	if(GetSelection(l, h) && c >= l && c < h) {
		selclick = true;
		return;
	}
	//SetCapture();
	//EditCapture();
	Move(c, flags & K_SHIFT);
	Finish();
}

int FakeEdit::GetCursor(int posx)
{
	posx -= GetSpaceLeft();
	if(!no_internal_margin)
		posx -= 2;
	if(posx <= 0)
		return 0;

	int count = text.GetLength();
	if(password)
		return min((posx + StdFont()['*'] / 2) / StdFont()['*'], count);

	int x = 0;
	const wchar* s = text;
	int i = 0;
	while(i < count) {
		int witdh = GetCharWidth(*s);
		if(posx < x + witdh / 2)
			break;
		x += witdh;
		s++;
		i++;
	}
	return i;
}

int FakeEdit::GetTextCx(const wchar *txt, int n, bool password) const
{
	if(password)
		return n * StdFont()['*'];
	const wchar *s = txt;
	int x = 0;
	while(n--)
		x += GetCharWidth(*s++);
	return x;
}

void FakeEdit::Move(int newpos, bool select)
{
	bool refresh = anchor >= 0;
	if(select) {
		if(anchor < 0)
			anchor = cursor;
		refresh = true;
	}
	else
		anchor = -1;
	cursor = newpos;
	Finish(refresh);
	SelSource();
}


void FakeEdit::SetSelection(int l, int h)
{
	if(l < h) {
		anchor = max(l, 0);
		cursor = min(h, text.GetLength());
	}
	else {
		cursor = l;
		anchor = -1;
	}
	SelSource();
	Finish();
}

bool FakeEdit::GetSelection(int& l, int& h) const
{
	if(anchor < 0 || anchor == cursor) {
		l = h = cursor;
		return false;
	}
	if(anchor < cursor) {
		l = anchor;
		h = cursor;
	}
	else {
		l = cursor;
		h = anchor;
	}
	return true;
}

void FakeEdit::Finish(bool refresh)
{
	if(anchor > text.GetLength()) anchor = text.GetLength();
	if(cursor > text.GetLength()) cursor = text.GetLength();
	if(cursor < 0) cursor = 0;
	Size sz {0,0};// GetSize();
//	if(autosize) {
//		Rect r = GetRect();
//		int mw = min(r.Width(), Draw::GetStdFontSize().cx);
//		sz.cx = GetCaret(text.GetLength()) + 4;
//		sz = AddFrameSize(sz);
//		if(GetParent())
//			sz.cx = min(sz.cx, GetParent()->GetSize().cx - r.left);
//		sz.cx = minmax(sz.cx, mw, autosize);
//		if(sz.cx != r.Width())
//			LeftPos(r.left, sz.cx);
//		sz = GetSize();
//	}
	if(!no_internal_margin)
		sz.cx -= 2;
	sz.cx -= GetSpaceLeft() + GetSpaceRight();
	if(sz.cx <= 0) return;
	int x = GetCaret(cursor);
	int rspc = max(StdFont().GetRightSpace('o'), StdFont().GetCy() / 5); // sometimes RightSpace is not implemented (0)
	int wx = x + rspc;
	if(alignright) {
		int cx = GetCaret(text.GetCount());
		sc = cx - sz.cx + rspc + 2;
		//RefreshAll();
	}
	if(wx > sz.cx + sc - 1) {
		sc = wx - sz.cx + 1;
		//RefreshAll();
	}
	if(x < sc) {
		sc = x;
		//RefreshAll();
	}
	if(refresh)
		;//RefreshAll();
}

void FakeEdit::SelSource()
{
	if(GetSelection(fsell, fselh))
		;//SetSelectionSource(ClipFmtsText());
	else
		fsell = fselh = -1;
	CancelMyPreedit();
}

int  FakeEdit::GetCaret(int cursor) const
{
	return GetTextCx(text, cursor, password);
}

int  FakeEdit::GetTy(int cy) const
{
	return (cy+1 -StdFont().GetCy())/2;//(GetSize().cy + 1 - font.GetCy()) / 2;
}

END_NAMESPACE