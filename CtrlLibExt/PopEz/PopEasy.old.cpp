#include "PopEasy.h"

NAMESPACE_UPP

// a default implementation
Size PopSlave::GetPreferredSize()const
{
	return GetScreenSize()/2;
}

// Pop up or repop up.
//
void PopSlave::PopUp(Rect& r)
{
	Ctrl& c=*GetCtrl();
	c.SetRect(r);
	PopUp();
}

void PopSlave::PopUp()
{
	Ctrl& c=*GetCtrl();
	if(!c.IsOpen() && main!=NULL)
		c.PopUp(&main->GetMain(), true, false, true, true);
}


void PopSlave::AddHint(const PopUpHint& hint, PopMainEx* main)
{
	switch(hint_count)
	{
	case 0:
		hints[0]=hint;
		cur_hint_index=0;
		++hint_count;
		// not currently working on any hints, so start it
		//
		PostCallback([=]{ProcessHint();});
		break;
	case 1:
		++hint_count;
		// deliberate fall-through
	case 2:
		hints[cur_hint_index==0?1:0]=hint;
		break;
	}
	SetMain(main);
}

// single thread approach
// supplied PrepareData() should call Ctrl::ProcessEvents periodically to accept subsequent user inputs
//
// must guarantee only one instance of ProcessHint will actually be running.
// at its finish, if it detects there are more hint(s) waiting to be processed, it should call 
// PostCallback with itself as callback so that another round of ProcessHint starts immediately after the
// current round.
// 
void PopSlave::ProcessHint()
{
	ASSERT( unsigned(cur_hint_index)<2 && unsigned(hint_count-1)<2);
	PopUpHint& hint=hints[cur_hint_index];
	
	if(hint.reason!=HR_SHOW && !IsOpen() && !GetAutoPopUp())
	{
		Ctrl::ProcessEvent(); // AddHint() may be called because of this line.
		if(--hint_count==1)
		{
			cur_hint_index=cur_hint_index==1?0:1;// next hint to be processed
			PostCallback([=]{ProcessHint();});
		}
		return;
	}

	String suffix;
	bool autocomplete=GetAutoComplete() && !hint.no_autocompletion;
	int rcnt=PrepareData(hints[cur_hint_index], autocomplete?&suffix:NULL);
	
	Ctrl::ProcessEvent();
	
	if(--hint_count==1) // means the current one has been cancelled.
	{
		//--hint_count;
		cur_hint_index=cur_hint_index==1?0:1;
		PostCallback([=]{ProcessHint();});
	}else{
#ifdef _DEBUG
		if(hint_count!=0)
		{
			LOG("Error: PopSlave::ProcessHint()---hint_count expected to be 0 but not");
		}
#endif
		switch(rcnt)
		{
		case 0:
			Unpop();
			return;
		case 1:
			if(GetAutoSelect())
			{
				Select();
				Unpop();
				return;
			}
			
		}
		// data preparation is ready.
		// now negotiation location.
		Rect rt;
		NegotiateSizePosition(rt);
		this->PopUp(rt);
		if(autocomplete)
		{
			main->AcceptSuffix(hint, suffix);
		}
	}
}

void PopSlave::NegotiateSizePosition(Rect& rt)
{
	if(!this->GetMain())
	{
		rt=Null;
		return;
	}
	GetMain()->GetSizePosition(rt);
	if(IsNull(rt))
	{
		Size prefer, screen_size;
		prefer=GetPreferredSize();
		screen_size=GetScreenSize();
		// intended future extension. Main will provide a outer rect (not necessarily screen rect)
		// and an inner rect. That's why we have seemingly meaningless outer Rect here.
		//
		Rect outer=RectC(0,0,screen_size.cx, screen_size.cy);
		Rect inner=main->GetMain().GetScreenRect();
		
		++outer.left; ++outer.top; --outer.right; --outer.bottom;
		--inner.left; --inner.top; ++inner.right; ++inner.bottom;
		
		// test if bottom can accomodate the popup at its preferred size
		if(outer.bottom-inner.bottom>=prefer.cy )
		{
			rt.left=inner.left;
			rt.right=rt.left+prefer.cx;
			rt.top=inner.bottom;
			rt.bottom=rt.top+prefer.cy;
			if(rt.right>outer.right)
			{
				rt.right=outer.right;
				rt.left=rt.right-prefer.cx;
				if(rt.left<outer.left)
					rt.left=outer.left;
			}
		}else if(inner.top-outer.top>=prefer.cy)
		{
			rt.left   = inner.left;
			rt.right  = rt.left + prefer.cx;
			rt.bottom = inner.top;
			rt.top    = rt.bottom - prefer.cy;
			if(rt.right>outer.right)
			{
				rt.right=outer.right;
				rt.left=rt.right-prefer.cx;
				if(rt.left<outer.left)
					rt.left=outer.left;
			}
		}else if(outer.right-inner.right>=prefer.cx)
		{
			rt.left=inner.right;
			rt.right=rt.left+prefer.cx;
			rt.top=inner.top;
			rt.bottom=rt.top+prefer.cy;
			if(rt.bottom>outer.bottom)
			{
				rt.bottom=outer.bottom;
				rt.top=rt.bottom-prefer.cy;
				if(rt.top<outer.top)
					rt.top=outer.top;
			}
		}else if(inner.left-outer.left>=prefer.cx)
		{
			rt.right=inner.left;
			rt.left=rt.right-prefer.cx;
			rt.top=inner.top;
			rt.bottom=rt.top+prefer.cy;
			if(rt.bottom>outer.bottom)
			{
				rt.bottom=outer.bottom;
				rt.top=rt.bottom-prefer.cy;
				if(rt.top<outer.top)
					rt.top=outer.top;
			}
		}else{ // take top or bottom whichever has larger area.
			if(inner.top-outer.top>outer.bottom-inner.bottom)
			{
				rt.top=outer.top;
				rt.bottom=inner.top;
			}else{
				rt.top=inner.bottom;
				rt.bottom=outer.bottom;
			}
			rt.left=inner.left;
			rt.right=rt.left+prefer.cx;
			if(rt.right>outer.right)
			{
				rt.right=outer.right;
				rt.left=rt.right-prefer.cx;
				if(rt.left<outer.left)
					rt.left=outer.left;
			}		
		}
	}
}

Ctrl* PopSlave::GetMainCtrl()
{
	return main? (&main->GetMain()):NULL;
}


bool PopSlave::DoKey(dword key, int repcount)
{
	switch(key)
	{
	case K_ENTER:
		if(Select())
		{
			Unpop();
		}
		return true;
	case K_ESCAPE:
		if(main){
			main->Cancel();
		}
		Unpop();
		return true;
	case K_UP:
	case K_DOWN:
	case K_PAGEUP:
	case K_PAGEDOWN:
		if(this->GetCtrl()->Key(key, repcount))
			return true;
	}
		
	return false;
}

PopListBase::PopListBase()
	: batch_size(200),key_col(-1),value_col(-1)
{
	NoWantFocus();
	Add(list.SizePos());
	list.NoWantFocus();
	list.EvenColor();
	list.WhenLeftDouble=[=]{ListLeftDbl();};
}

void   PopListBase::ListLeftDbl()
{
	DoKey(K_ENTER, 1);
}

// utf-8 support needed!
static int common_prefix_length_i(const char * s1, const char * s2)
{
	int i;
	for(i=0; s1[i]!='\0' && ToUpper(s1[i])==ToUpper(s2[i]); ++i)
		;
	return i;
}

int PopListBase::PrepareData(const PopUpHint& hint, String * suffix)
{
	int rcnt=GetRowCount();
	int ccnt=GetColumnCount();
	String htext=ToUpper(hint.text);
	
	bool nosuffix=suffix==NULL;
	String tmp_suffix;
	list.Clear();
	
	for(int i=0; i<rcnt; ++i)
	{
		for(int j=0; j<ccnt; ++j)
		{
			String s=String().Cat()<<GetCellAt(i,j);
			int p=ToUpper(s).Find(htext);
			if(p!=-1)
			{
				Vector<Value> v;
				for(j=0; j<ccnt; ++j)
					v.Add(GetCellAt(i,j)); // more complicated logic needed to cope with multiple occurances in single line
				list.Add(v);
				
				// calculate common suffix;
				if(!nosuffix)
				{
					
					if(tmp_suffix.IsEmpty())
					{
						tmp_suffix=s.Mid(p+htext.GetLength());
					}else
					{
						
						int len=common_prefix_length_i(tmp_suffix, ((const char*)s)+p+htext.GetLength());
						if(len==0)
							nosuffix=true;
						else
							tmp_suffix=tmp_suffix.Left(len);
					}
				}
				
				break;
			}
		}
		if(i%batch_size==0)
		{
			Ctrl::ProcessEvent();
			if(IsCurHintObselete())
				break;
		}
	}
	if(rcnt!=0)
		SetCursor(0);
	if(!nosuffix)
		*suffix=tmp_suffix;
	return list.GetCount();
}

bool PopListBase::Select()
{
	if(list.IsCursor())
	{
		int vcol=value_col,kcol=key_col;
		if(vcol<0 || vcol>=list.GetColumnCount())
		{
			vcol=(key_col<0||key_col>=list.GetColumnCount())?0:key_col;
		}
		if(kcol<0 || kcol>=list.GetColumnCount())
		{
			kcol=vcol;
		}
		main->AcceptData(list.Get(kcol), list.Get(vcol));
	}
	return true;
}


#define E__Addv(I)    v.At(I-1)=p##I
#define E__AddF(I) \
void PopList::Add(__List##I(E__Value)) { \
	Vector<Value>& v=data.Add();\
	__List##I(E__Addv); \
}
__Expand(E__AddF)


TreeListPopUp::TreeListPopUp()
	:fake_focus_ctrl(&list)
{
	Horz(tree, list);
	tree.NoWantFocus();
	list.NoWantFocus();
	list.AutoHideSb();
	
	RefreshTree();
	
	list.EvenColor();
	list.WhenLeftDouble=[=]ListLeftDouble();};
	list.WhenLeftClick=[=]ListWhenLeft();};
	
	tree.WhenLeftClick=[=]{TreeWhenLeft();};
	
	SetViewStyle(VS_TreeList);
}

Size TreeListPopUp::GetPreferredSize()const
{
	//GridCtrl& g=const_cast<GridCtrl&>(list);
	return Size(GetScreenSize()/2);
}

bool TreeListPopUp::DoKey(dword key, int repcount)
{
	switch(key)
	{
	case K_TAB:
		if(view_style==VS_TreeList)
		{
			FakeShiftFocus();
			return true;
		}
		return false;
	case K_UP:
	case K_DOWN:
	case K_PAGEUP:
	case K_PAGEDOWN:
		if(fake_focus_ctrl==&list && list.GetCursor()<0)
		{
			list.SetCursor(key==K_UP||key==K_PAGEUP?list.GetCount()-1:0);
			if(key!=K_PAGEUP && key!=K_PAGEDOWN)
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
	view_style=style;
	fake_focus_ctrl=&list;
	switch(style)
	{
	case VS_TreeOnly:
		Zoom(0);
		fake_focus_ctrl=&tree;
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

bool TreeListPopUp::Select(){ 
	if(main && list.GetCount()!=0)
	{
		if(!list.IsCursor())
			list.SetCursor(0);
		main->AcceptData(list.Get(0), list.Get(1));
		return true;
	}
	return false;
}


void TreeListPopUp::FakeShiftFocus()
{
	fake_focus_ctrl= fake_focus_ctrl==&tree?(Ctrl*)&list:(Ctrl*)&tree;
}

void TreeListPopUp::TreeWhenLeft()
{
	fake_focus_ctrl=&tree;
}

void TreeListPopUp::ListWhenLeft()
{
	fake_focus_ctrl=&list;
}

void TreeListPopUp::ListLeftDouble()
{
	fake_focus_ctrl=&list;
	DoKey(K_ENTER, 1);
}

// a default implementation, for EditField derivatives, may add a default for LineEdit etc later.
//
void PopMainEx::AcceptSuffix(const PopSlave::PopUpHint& hint, String suffix)
{
	PopSlave::PopUpHint h;
	GetPopupHint(h);
	if(h!=hint) // don't accept suffix if cursor/text, etc is different from the original hint this suffix is generate from.
		return;
	EditField * d=dynamic_cast<EditField*>(&GetMain());
	if(d==NULL)
		return;
	
	len_of_suffix_just_accepted=suffix.GetLength();
	d->Insert(h.sel_end, suffix);
	d->SetSelection(h.sel_start+len_of_suffix_just_accepted);
	d->Refresh();
}

bool PopMainEx::UnacceptSuffix()
{
	EditField * e=dynamic_cast<EditField*>(&GetMain());
	if(e==NULL)
		return false;
	e->Remove(e->GetLength()-len_of_suffix_just_accepted,len_of_suffix_just_accepted);
	e->CancelSelection();
	//e->Undo();
	e->Refresh();
	len_of_suffix_just_accepted=0;
	return true;
}

void PopMainEx::GetPopupHint(PopSlave::PopUpHint& hint)
{
	hint.text.Clear();
	EditField* e=dynamic_cast<EditField*>(this);
	if(e)
	{
		hint.text<<e->GetText();
		e->GetSelection(hint.sel_start, hint.sel_end);
		hint.no_autocompletion=false;
	}
}

Value  EditKeyValue::Format(const Value& q) const
{
	//return chained?chained->Format(q):Convert::Format(q);
	
	LOG(String().Cat()<<"EditKeyValue::Format()"<<q);
	ASSERT(convert==this);
	if(!matched && chained){
		const_cast<EditKeyValue&>(*this).SetKeyValue(key,chained->Format(q));
	}
	
	return matched?Value(text):Convert::Format(q);
}
// if matched, return key, else do more
Value  EditKeyValue::Scan(const Value& text) const
{
	//return chained?chained->Scan(text):Convert::Scan(text);
	//LOG(String().Cat()<<"EditKeyValue::Scan():("<<key<<','<<text<<','<<matched<<')');
	ASSERT(convert==this);
	if(!matched)
	{
		Value key, value;
		
		if(chained){
			key=chained->Scan(text);
			value=Format(key);
		}else{
			key=text;
			value=text;
		}
		//LOG("EditKeyValue::Scan() When !matched");
		EditKeyValue& t=const_cast<EditKeyValue&>(*this);
		t.SetKeyValue(key, value);
	}
	return key;
}

void EditKeyValue::Updated()
{
//	LOG("EditKeyValue::Updated()");
//	matched=false;
//	key=Null;
	EditField::Updated();
}

void
EditKeyValue::SetKeyValue(const Value&key, const Value& value)
{
//	LOG(String().Cat()<<"EditKeyValue::SetKeyValue-("<<key<<','<<value<<')');
	this->key=key;
	text=value;
	matched=true;
	Refresh();
}



END_UPP_NAMESPACE
