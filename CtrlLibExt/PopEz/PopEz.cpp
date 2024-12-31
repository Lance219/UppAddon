#include <CtrlLibExt/CtrlLibExt.h>

BEGIN_NAMESPACE_LZ

using namespace Upp;

// a default implementation
Size PopSlave::GetPreferredSize() const { return GetScreenSize() / 5; }

// Pop up or repop up.
//
void PopSlave::PopUp(Rect& r)
{
	Ctrl& c = *GetCtrl();
	c.SetRect(r);
	PopUp();
}

void PopSlave::PopUp()
{
	Ctrl& c = *GetCtrl();
	
	SetPaintAsFocused(c);

	if(!c.IsOpen() && GetMainEx() != NULL) {
		c.PopUp(&GetMainEx()->GetMain(), true, false, true, true);
	}
}

void PopSlave::AddHint(const PopupHint& hint, PopMainEx* ex)
{
	switch(hint_count) {

	case 0:
		hints[0] = hint;
		cur_hint_index = 0;
		hint_count = hint_count + 1;
		// not currently working on any hints, so start it
		//
		PostCallback([=, this] { ProcessHint(); });
		break;

	case 1:
		hint_count = hint_count + 1;
		// deliberate fall-through

	case 2:
		hints[cur_hint_index == 0 ? 1 : 0] = hint;
		break;
	}

	SetMainEx(ex);
}

// single thread approach
// supplied PrepareData() should call Ctrl::ProcessEvents periodically to accept subsequent user
// inputs
//
// must guarantee only one instance of ProcessHint will actually be running.
// at its finish, if it detects there are more hint(s) waiting to be processed, it should call
// PostCallback with itself as callback so that another round of ProcessHint starts immediately
// after the current round.
//
void PopSlave::ProcessHint()
{
	ASSERT(unsigned(cur_hint_index) < 2);
	ASSERT(unsigned(hint_count - 1) < 2);
	PopupHint& hint = hints[cur_hint_index];

	if(hint.reason != HintReason::Show && !IsOpen() && !GetAutoPopUp()) {
		Ctrl::ProcessEvents(); // AddHint() may be called because of this line.

		if(hint_count = hint_count - 1; hint_count == 1) {
			cur_hint_index = cur_hint_index == 1 ? 0 : 1; // next hint to be processed
			PostCallback([=, this] { ProcessHint(); });
		}

		return;
	}

	String suffix;

	bool autocomplete = !hint.no_autocompletion && GetAutoComplete();
	int rcnt = PrepareData(hints[cur_hint_index], autocomplete ? &suffix : NULL);

	Ctrl::ProcessEvents();

	if(hint_count = hint_count - 1;
	   hint_count == 1) // means the current one has been cancelled.
	{
		//--hint_count;
		cur_hint_index = cur_hint_index == 1 ? 0 : 1;
		PostCallback([=, this] { ProcessHint(); });
	}

	else {
		ASSERT_(hint_count == 0, "PopSlave::ProcessHint()---hint_count expected to be 0");

		switch(rcnt) {

		case 0:
			Unpop();
			return;

		case 1:

			if(GetAutoSelect()) {
				Upp::BeepExclamation();
				Select();
				Unpop();
				return;
			}
			// deliberate fall-through
		}

		// data preparation is ready.
		// now negotiation location.
		Rect rt;

		NegotiateSizePosition(rt);

		this->PopUp(rt);

		if(autocomplete) {
			GetMainEx()->AcceptSuffix(hint, suffix);
		}
	}
}

void PopSlave::NegotiateSizePosition(Rect& rt)
{
	if(!GetMainEx()) {
		rt = Null;
		return;
	}

	GetMainEx()->GetSizePosition(rt);

	if(IsNull(rt)) {
		Size prefer, screen_size;
		prefer = GetPreferredSize();
		screen_size = GetScreenSize();
		// intended future extension. Main will provide a outer rect (not necessarily screen
		// rect) and an inner rect. That's why we have seemingly meaningless outer Rect here.
		//
		Rect outer = RectC(0, 0, screen_size.cx, screen_size.cy);
		Rect inner = GetMainEx()->GetMain().GetScreenRect();

		++outer.left;
		++outer.top;
		--outer.right;
		--outer.bottom;
		--inner.left;
		--inner.top;
		++inner.right;
		++inner.bottom;

		// test if bottom can accomodate the popup at its preferred size

		if(outer.bottom - inner.bottom >= prefer.cy) {
			rt.left = inner.left;
			rt.right = rt.left + prefer.cx;
			rt.top = inner.bottom;
			rt.bottom = rt.top + prefer.cy;

			if(rt.right > outer.right) {
				rt.right = outer.right;
				rt.left = rt.right - prefer.cx;

				if(rt.left < outer.left)
					rt.left = outer.left;
			}
		}
		else if(inner.top - outer.top >= prefer.cy) {
			rt.left = inner.left;
			rt.right = rt.left + prefer.cx;
			rt.bottom = inner.top;
			rt.top = rt.bottom - prefer.cy;

			if(rt.right > outer.right) {
				rt.right = outer.right;
				rt.left = rt.right - prefer.cx;

				if(rt.left < outer.left)
					rt.left = outer.left;
			}
		}
		else if(outer.right - inner.right >= prefer.cx) {
			rt.left = inner.right;
			rt.right = rt.left + prefer.cx;
			rt.top = inner.top;
			rt.bottom = rt.top + prefer.cy;

			if(rt.bottom > outer.bottom) {
				rt.bottom = outer.bottom;
				rt.top = rt.bottom - prefer.cy;

				if(rt.top < outer.top)
					rt.top = outer.top;
			}
		}
		else if(inner.left - outer.left >= prefer.cx) {
			rt.right = inner.left;
			rt.left = rt.right - prefer.cx;
			rt.top = inner.top;
			rt.bottom = rt.top + prefer.cy;

			if(rt.bottom > outer.bottom) {
				rt.bottom = outer.bottom;
				rt.top = rt.bottom - prefer.cy;

				if(rt.top < outer.top)
					rt.top = outer.top;
			}
		}
		else {
			// take top or bottom whichever has larger area.

			if(inner.top - outer.top > outer.bottom - inner.bottom) {
				rt.top = outer.top;
				rt.bottom = inner.top;
			}
			else {
				rt.top = inner.bottom;
				rt.bottom = outer.bottom;
			}

			rt.left = inner.left;

			rt.right = rt.left + prefer.cx;

			if(rt.right > outer.right) {
				rt.right = outer.right;
				rt.left = rt.right - prefer.cx;

				if(rt.left < outer.left)
					rt.left = outer.left;
			}
		}
	}
}

Ctrl* PopSlave::GetMainCtrl() { return GetMainEx() ? (&GetMainEx()->GetMain()) : nullptr; }

bool PopSlave::DoKey(dword key, int repcount)
{
	switch(key) {

	case K_ENTER:

		if(Select()) {
			Unpop();
		}

		return true;

	case K_ESCAPE:
		if(GetMainEx()) {
			GetMainEx()->Cancel();
		}

		Unpop();

		return true;

	case K_UP:

	case K_DOWN:

	case K_PAGEUP:

	case K_PAGEDOWN:
		return GetCtrl()->Key(key, repcount);
	}

	return false;
}

ListPopBase::ListPopBase()
	: batch_size(200)
{
	NoWantFocus();
}

void ListPopBase::LeftDouble(Point p, dword keyflags)
{
	GridEz::LeftDouble(p, keyflags);
	;
	DoKey(K_ENTER, 1);
}

// utf-8 support needed!
static int common_prefix_length_i(const char* s1, const char* s2)
{
	int i;

	for(i = 0; s1[i] != '\0' && ToUpper(s1[i]) == ToUpper(s2[i]); ++i)
		;

	return i;
}

int ListPopBase::PrepareData(const PopupHint& hint, String* suffix)
{
	int rcnt = GetRowCount();
	int ccnt = GetColumnCount();
	String htext = ToUpper(hint.text);

	bool nosuffix = suffix == NULL;
	String tmp_suffix;
	Clear();

	for(int i = 0; i < rcnt; ++i) {
		for(int j = 0; j < ccnt; ++j) {
			String s = String().Cat() << GetCellAt(i, j);
			int p = ToUpper(s).Find(htext);

			if(p != -1) {
				Row& r = AddRow();
				Vector<Value> v;

				for(j = 0; j < ccnt; ++j)
					r.At(j) = Cell(String().Cat() << GetCellAt(i, j));

				// calculate common suffix;
				if(!nosuffix) {

					if(tmp_suffix.IsEmpty()) {
						tmp_suffix = s.Mid(p + htext.GetLength());
					}

					else {

						int len = common_prefix_length_i(tmp_suffix, ((const char*)s) + p +
						                                                 htext.GetLength());

						if(len == 0)
							nosuffix = true;
						else
							tmp_suffix = tmp_suffix.Left(len);
					}
				}

				break;
			}
		}

		if(i % batch_size == 0) {
			Ctrl::ProcessEvents();

			if(IsCurHintObselete())
				break;
		}
	}

	if(rcnt != 0)
		SetCursor(0);

	if(!nosuffix)
		*suffix = tmp_suffix;

	return GetCount();
}

bool ListPopBase::Select()
{
	if(IsCursor())
		GetMainEx()->AcceptData(Get(0), GetColumnCount() == 1 ? Get(0) : Get(1));

	return true;
}

TreeListPopUp::TreeListPopUp()
	: fake_focus_ctrl(&list)
	, key_col(0)
{
	Horz(tree, list);
	tree.NoWantFocus();
	list.NoWantFocus();
	list.WhenRowColor = [](int r, Color& ink, Color& paper, int) {
		if(r % 4 == 3)
			paper = Gray();//Blend(Cyan(), White());
	};

	RefreshTree();

	list.WhenLeftDouble = [=, this](Point, dword) { ListLeftDouble(); };

	list.WhenLeftClick = [=, this](Point, dword) { ListWhenLeft(); };

	tree.WhenLeftClick = [=, this] { TreeWhenLeft(); };

	SetViewStyle(VS_TreeList);
	SetPaintAsFocused(list);
}

Size TreeListPopUp::GetPreferredSize() const
{
	int cxm, cx = list.GetTotalCx();
	const Ctrl* c = GetMainCtrl();

	if(c) {
		cxm = c->GetRect().Width();

		if(cxm > cx)
			cx = cxm;
	}

	return Size(cx, list.GetTotalCy());
}

bool TreeListPopUp::DoKey(dword key, int repcount)
{
	switch(key) {

	case K_TAB:

		if(view_style == VS_TreeList) {
			FakeShiftFocus();
			return true;
		}

		return false;

	case K_UP:

	case K_DOWN:

	case K_PAGEUP:

	case K_PAGEDOWN:

		if(fake_focus_ctrl == &list && list.GetCursor() < 0) {
			//			list.SetCursor(key==K_UP||key==K_PAGEUP?list.GetCount()-1:0);
			list.SetCursor((key == K_UP || key == K_PAGEUP)
			                   ? list.GetCount() - list.GetFixedBottomRowCount() - 1
			                   : list.GetFixedTopRowCount());

			if(key != K_PAGEUP && key != K_PAGEDOWN)
				return true;
		}

		if(fake_focus_ctrl->Key(key, repcount))
			return true;

		break;
	}

	return _Parent::DoKey(key, repcount);
}

TreeListPopUp& TreeListPopUp::SetViewStyle(TreeListPopUp::ViewStyle style)
{
	view_style = style;
	fake_focus_ctrl = &list;

	switch(style) {

	case VS_TreeOnly:
		Zoom(0);
		fake_focus_ctrl = &tree;
		break;

	case VS_ListOnly:
		Zoom(1);
		break;

	default:
		Zoom(-1);
		SetPos(3000);
	}

	return *this;
}

bool TreeListPopUp::Select()
{
	if(GetMainEx() && list.GetCount() != 0) {
		if(!list.IsCursor())
			list.SetCursor(list.GetFixedTopRowCount());

		GetMainEx()->AcceptData(list.Get(this->GetKeyColumn()), Null);

		return true;
	}

	return false;
}

TreeListPopUp& TreeListPopUp::SetKeyColumn(int col)
{
	if(col < 0)
		col = 0;

	key_col = col;

	return *this;
}

int TreeListPopUp::GetKeyColumn() const { return key_col; }

void TreeListPopUp::FakeShiftFocus()
{
	if( fake_focus_ctrl == &tree)
	{
		fake_focus_ctrl = &list;
		SetPaintAsFocused(list);
	}else{
		fake_focus_ctrl = &tree;
		SetPaintAsFocused(tree);
	}
//		fake_focus_ctrl = fake_focus_ctrl == &tree ? (Ctrl*)&list : (Ctrl*)&tree;
	
}

void TreeListPopUp::TreeWhenLeft() { fake_focus_ctrl = &tree; }

void TreeListPopUp::ListWhenLeft() { fake_focus_ctrl = &list; }

void TreeListPopUp::ListLeftDouble() { DoKey(K_ENTER, 1); }

// a default implementation, for EditField derivatives, may add a default for LineEdit etc
// later.
//
void PopMainEx::AcceptSuffix(const PopSlave::PopupHint& hint, String suffix)
{
	PopSlave::PopupHint h;
	GetPopupHint(h);

	if(h != hint) // don't accept suffix if cursor/text, etc is different from the original hint
	              // this suffix is generate from.
		return;

	EditField* d = dynamic_cast<EditField*>(&GetMain());

	if(d == NULL)
		return;

	len_of_suffix_just_accepted = suffix.GetLength();

	d->Insert(h.sel_end, suffix);

	d->SetSelection(h.sel_start + len_of_suffix_just_accepted);

	d->Refresh();
}

bool PopMainEx::UnacceptSuffix()
{
	EditField* e = dynamic_cast<EditField*>(&GetMain());

	if(e == NULL)
		return false;

	e->Remove(e->GetLength() - len_of_suffix_just_accepted, len_of_suffix_just_accepted);

	e->CancelSelection();

	e->Refresh();

	len_of_suffix_just_accepted = 0;

	return true;
}

void PopMainEx::GetPopupHint(PopSlave::PopupHint& hint)
{
	hint.text.Clear();
	EditField* e = dynamic_cast<EditField*>(this);

	if(e) {
		hint.text << e->GetText();
		e->GetSelection(hint.sel_start, hint.sel_end);
		hint.no_autocompletion = false;
	}
}

void WithPopupSetup(Ctrl& c, MultiButtonFrame& frame, PopMainEx& ex)
{
	frame.AddTo(c);
	frame.AddButton().Main() <<= [&] {
		c.SetFocus();

		if(ex.GetSlave()) {
			if(ex.GetSlave()->GetMainEx() != &ex)
				ex.GetSlave()->Unpop();

			c.Key(K_ALT_DOWN, 1);
		}
	};
	frame.NoDisplay()
		.SetStyle(frame.StyleFrame())
		.GetButton(0)
		.SetMonoImage(Grayscale(CtrlsImg::DA()));
}

END_NAMESPACE
