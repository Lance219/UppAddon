#ifndef _CtrLibExt_CtrlLibExt_h_
#define _CtrLibExt_CtrlLibExt_h_

#include <CtrlLib/CtrlLib.h>
#include <CtrlCoreExt/CtrlCoreExt.h>

BEGIN_NAMESPACE_LZ
#include "bits/using_upp_namespace.h"

class FakeEdit : public FakeCtrl, private Upp::TextArrayOps{
public:
	void   Paint(Draw& w, Size sz)override;
	void LeftDown(Point p, dword keyflags)override;
	
	
	void SetText(const WString& text){ this->text = text; }

protected:
	int GetTy(int cy) const;
protected:
	virtual void HighlightText(Upp::Vector<Upp::EditField::Highlight>& hl){};
	int64 GetTotal()const override				{ return text.GetLength();	}
	int	  GetCharAt(int64 pos)const	override	{ return text[(int)pos];	}

	int	  GetCursor(int posx);

	virtual void PaintSpace(Draw& w){}
	virtual int GetSpaceLeft()const				{ return 0;					}
	virtual int GetSpaceRight()const			{ return 0;					}
	
	WString		text, undotext;
	Rect16		dropcaret;
	
	String	nulltext;
	Image	nullicon;
	
protected:
	enum {
		ATTR_TEXTCOLOR = FakeCtrl::ATTR_LAST,
		ATTR_INACTIVE_CONVERT,
		ATTR_CHARFILTER,
		ATTR_NULLICON,
		ATTR_NULLTEXT,
		ATTR_NULLINK,
		ATTR_NULLFONT,
		ATTR_BACKGROUND,
		ATTR_LAST,
	};

	int sc;
	int cursor, anchor;
	
	int undocursor, undoanchor;
	
	int maxlen;
	int autosize;
	int fsell, fselh;
	
	int dropcursor;
	
	byte charset;
	
	bool selclick : 1;
	bool password : 1;
	bool autoformat : 1;
	bool keep_selection : 1;
	bool clickselect : 1;
	bool nobg : 1;
	bool alignright : 1;
	bool errorbg: 1;
	bool showspaces : 1;
	bool no_internal_margin : 1;
	
	int LowChar(int c) const { return 0x25af /*c + 0x2400*/; }
	int GetCharWidth(int c) const { return StdFont()[c < 32 ? LowChar(c) : c]; }
	int     GetTextCx(const wchar *text, int n, bool password/*, Font fnt*/) const;
	void Paints(Draw& w, int& x, int fcy, const wchar*& txt, Color ink, Color paper, int n,
	            bool pwd, Font fnt, Color underline, bool showspaces);
	int GetCaret(int cursor) const;
	void Finish(bool refresh = true);
	void SelSource();


public:
	void Move(int newpos, bool select = false);

	void SetSelection(int l = 0, int h = INT_MAX);
	bool GetSelection(int& l, int& h) const;
	int GetLength() const { return text.GetLength(); }
	
};

END_NAMESPACE

#include "GridEz/GridEz.h"
#include "PopEz/PopEz.h"

#endif
