#ifndef _PopEasy_PopEasy_h_
#define _PopEasy_PopEasy_h_

#include <CtrlLib/CtrlLib.h>
#include <GridCtrl/GridCtrl.h>

NAMESPACE_UPP


// user or main ask to popup, eg by keyboard/mouse/pseudoKey;
//
// content negotiation
// main communicates its content, caret location, selection etc to PopSlave (how?)
// PopSlave prepare content to be shown accordingly. Many interesting things can happen here
// for example, if there are nothing to be shown, it may asked not to be PopUp'd; if there's
// only one candidate, it may do the auto selection; etc
//
// if both agrees to the PopUP action, then size/location negotiation starts
// 
// PopSlave propose a location and size and communicated to main;
// main authorize or modify the proposal and give it back to popup
// PopSlave do the popup.
//
// or both can be combined?
//
// interface that a PopUp is expected to implement/provide
//
struct PopMainEx;

struct PopSlave
{
	enum HintReason
	{
		HR_SHOW,
		///HR_CLOSE,
		HR_CHANGED		
	};
	
	struct PopUpHint
	{
		int reason;  // HR_SHOW: explicit request to show; 
		             // HR_CHANGED: text/cursor location, etc, changed.
		String text;
		int     sel_start; // if sel_start==sel_end, it's the caret position.
		int     sel_end;
		bool    no_autocompletion; // if last_key==K_BACKSPACE, no auto completion should be performed
		bool operator ==(const PopUpHint& h)const
		{
			return text==h.text && sel_start==h.sel_start && sel_end==h.sel_end;
		}
		bool operator !=(const PopUpHint& h)const
		{
			return !(*this==h);
		}
		
		String ToString()const
		{
			return String().Cat()<<"Text:"<<text<<", sel_start:"<<sel_start<<", sel_end: "<<sel_end;
		}
		// mouse location omitted for now
	};
	
	

	virtual bool DoKey(dword key, int repcount);
	
	virtual int PrepareData(const PopUpHint& hint, String* suffix)=0; //{ return 0; }
	virtual Size GetPreferredSize()const;
	virtual bool Select(){ return true; }
	
	virtual void SetAutoPopUp(bool popup=true){ autopopup=popup;}
	virtual bool GetAutoPopUp()const{ return autopopup; }
	virtual void SetAutoComplete(bool v=true){ autocomplete=v;}
	virtual bool GetAutoComplete()const{ return autocomplete; }
	virtual void SetAutoSelect(bool v=true){ autoselect=v;}
	virtual bool GetAutoSelect()const{ return autoselect; }
	
	PopSlave():main(NULL), _dummy(0){}

	Ctrl* GetCtrl(){
		struct _P: PopSlave, Ctrl
		{
		};
		return static_cast<Ctrl*>(reinterpret_cast<_P*>(this)); 
	}
	const   Ctrl* GetCtrl()const{ 
		return const_cast<const Ctrl*>(
			const_cast<PopSlave&>(*this).GetCtrl()
		);
	}
	void SetMain(PopMainEx* _main){ main=_main; }
	PopMainEx* GetMain(){ return main; }
	const PopMainEx* GetMain()const{ return main; }
	Ctrl* GetMainCtrl();
	bool IsOpen()const{ return GetCtrl()->IsOpen(); }
	
	void NegotiateSizePosition(Rect& rect);

	void PopUp(Rect& r);
	void PopUp();
	
	void Unpop()
	{
		GetCtrl()->Close();
	}

	void AddHint(const PopUpHint& hint, PopMainEx * main);
	void ProcessHint();

	bool IsCurHintObselete()const{ return hint_count>1; }
protected:
	PopMainEx* main;
	//Ctrl* main;
	//Callback1<Rect&> WhenSizeNegotiate;
	PopUpHint hints[2];
	union{
		struct{
			bool  autopopup:1;
			bool  autocomplete:1;
			bool  autoselect:1;
			int8  cur_hint_index;
			volatile int8  hint_count;
		};
		int32 _dummy;
	};
	
};

class PopListBase: public PopSlave, public ParentCtrl
{
public:
	PopListBase();
	
	virtual int PrepareData(const PopUpHint& hint, String* suffix); //{ return 0; }
	virtual bool Select();
	// define the storage interface
	//
	virtual int GetRowCount()const=0;
	virtual int GetColumnCount()const=0;
	virtual Value GetCellAt(int row, int col)const=0;
	
	//virtual void   LeftDouble(Point p, dword keyflags);
	
	
	PopListBase& SetKeyColumn(int kcol){ key_col=kcol; return *this;}
	PopListBase& SetValueColumn(int vcol){ value_col=vcol; return *this;}
	
protected:
	GridCtrl list;

	int batch_size;
	int key_col;   // column for key
	int value_col; // column for value
	
private:
	void ListLeftDbl();
};


class PopList: public PopListBase
{
public:
	void Add(const Vector<Value>& v)
	{
		data.Add(v);
	}
	
#define  E__Add(I)      void Add(__List##I(E__Value));
	__Expand(E__Add)
#undef   E__Add

protected:
	virtual int GetRowCount()const
	{
		return data.GetCount();
	}
	virtual int GetColumnCount()const
	{
		return data.GetCount()==0?0:data[0].GetCount();
	}
	virtual Value GetCellAt(int row, int col)const
	{
		if(row>=data.GetCount() || col>=data[row].GetCount())
			return Null;
		return data[row][col];
	}

	Vector<Vector<Value> > data; // provides naive local storage
};

class TreeListPopUp: public PopSlave, public Splitter
{
public:
	enum ViewStyle{VS_TreeOnly, VS_ListOnly, VS_TreeList};

	TreeListPopUp();
	
	TreeListPopUp& SetViewStyle(ViewStyle style);	

	virtual bool DoKey(dword key, int repcount);
	virtual bool Select();
	virtual Size GetPreferredSize()const;
	virtual void RefreshTree(){} // newly added
	virtual void RefreshList(){} // newly added

	typedef PopSlave _Parent;
protected:
	GridCtrl  list;
	TreeCtrl   tree;
	Ctrl*      fake_focus_ctrl;
	String     hint_text;
	ViewStyle  view_style; 
	
	
	void FakeShiftFocus();
	
	void TreeWhenLeft();
	void ListLeftDouble();
	void ListWhenLeft();
};

//#define COMPACT_WITHPOPUP
//#ifndef COMPACT_WITHPOPUP


// it becomes increasingly apparent WithPopUp<T> also need to have some virtual methods or
// even member variables (eg., one can argue AutoPopUp, AutoComplete, AutoSelct should 
// be properties of WithPopUp<T> instead),
// instead of simply add it to the class, we collect them into an interface like thing so that
// they can be accessed more uniformly.
struct PopMainEx
{
	virtual void GetSizePosition(Rect& rt){ rt=Null; }
	virtual void GetPopupHint(PopSlave::PopUpHint& hint);
	virtual void AcceptData(const Value& key, const Value& text){ 
			ValueArray va; va.Add(key); va.Add(text);
			GetMain().SetData(va); 
	}
	virtual void AcceptSuffix(const PopSlave::PopUpHint& hint, String suffix);
	virtual bool UnacceptSuffix();
	virtual void Cancel(){} // when user close popup without make a selection, eg by Esc key.
	
	//virtual ~PopMainEx(); - not required right now. 
	
	Ctrl& GetMain(){ return *reinterpret_cast<Ctrl*>(
	     	reinterpret_cast<char*>(this)-delta
	     );
	}
	const Ctrl& GetMain()const
	{ 
		return const_cast<const Ctrl&>(
			const_cast<PopMainEx&>(*this).GetMain()
		);
	}
	PopMainEx(size_t delta):delta(delta), man(NULL), len_of_suffix_just_accepted(0){}

protected:	                                                           
	PopSlave  *man;	
	const size_t delta; // delta is used to convert this to WithPopUp<T>
	int  len_of_suffix_just_accepted;
	
};

template <class T>class WithPopUp : public T, public PopMainEx
{
public:
	typedef WithPopUp<T> _Self;
	typedef T			 _Parent;
	
	WithPopUp():PopMainEx(sizeof(T)){		
	}
	

	virtual bool Key(dword key, int count);
	virtual void SetData(const Value& v);

	PopSlave* SetPopSlave(PopSlave* _new);
	
	bool IsPopUpOpen(){ return man && man->GetCtrl()->IsOpen(); }
protected:
	void Unpop()
	{
		if(man && man->GetMainCtrl()==this)
			man->Unpop();
	}		
		
	virtual void LostFocus(){
	 	Unpop();
	 	this->_Parent::LostFocus(); 
	}	
	 
	virtual void State(int reason)
	{
	 if(reason==static_cast<int>(Ctrl::POSITION) && IsPopUpOpen())
	 {
	    Rect rt;
	    man->NegotiateSizePosition(rt);
	    man->PopUp(rt); 
	 }
	 else if(!this->IsVisible())
	     Unpop();
	 this->_Parent::State(reason);
	}
	
//protected:
//		
};

template <class T>
bool WithPopUp<T>::Key(dword key, int count)
{
	//LOG(String().Cat()<<"WithPopUp<T>::Key: (key,asc)=("<<char(key)<<','<<key<<')');
	if(man)
	{
		PopSlave::PopUpHint hint;
		switch(key)
		{
		case K_ALT_DOWN:
		case K_ALT_UP:
			if(IsPopUpOpen())
			{
				Unpop();

			}else
			{
				hint.reason=PopSlave::HR_SHOW;
				this->GetPopupHint(hint);
				man->AddHint(hint, this);
				return true;
			}
			break;
		default:
			if( IsPopUpOpen() && man->DoKey(key, count))
				return true;
		}
		if( (key==K_BACKSPACE && len_of_suffix_just_accepted!=0 && UnacceptSuffix() ) || 
				this->T::Key(key, count)
		){
			hint.reason=PopSlave::HR_CHANGED;
			this->GetPopupHint(hint);
			hint.no_autocompletion=key==K_BACKSPACE;
			if(IsPopUpOpen() || man->GetAutoPopUp())
			{
				man->AddHint(hint, this);
			}
			len_of_suffix_just_accepted=0;
			return true;
		}
		return false;
	}else
		return this->T::Key(key, count);
}



template <class T>
PopSlave* WithPopUp<T>::SetPopSlave(PopSlave* _new)
{
	PopSlave *old=man;
	if(old)
	{
		old->SetMain(NULL);
		Unpop();
	}
	man=_new;
	man->SetMain(this);
	return old;
}
class EditKeyValue;

template <class T>
void WithPopUp<T>::SetData(const Value& v)
{
//	LOG("WithPopUp<T>::SetData()"<<v);
	EditKeyValue* e=dynamic_cast<EditKeyValue*>(this);
	Value key;
	bool  formated=false;
	if(e!=NULL)
	{	
		e->MarkAsDirty();	
		if(IsValueArray(v))
		{
			ValueArray va=v;
			if(va.GetCount()==2)
			{
				e->SetKeyValue(va[0],va[1]);
				formated=true;
			}else
				key=va.GetCount()>0?va[0]:Null; 
		}else
			key=v;
		if(!formated)
			e->SetKeyValue(key, e->Format(key));
	}else
		T::SetData(v);
	
	Refresh();
}

// PopEasy doesn't work easily with stock EditField derivatives.
// EditKeyValue is designed to work with PopEasy in a typical situation
// to retrieve Key:Value pair.
// 'Key' is the internal representation; 'Value' is what's shown to 
// the world.
//
// eg, in database application, 'key' may be the primary key, 
//  while 'value' is some combination of attributes that make sense
// to users
//
class EditKeyValue : public EditField, public Convert
{
public:
	EditKeyValue(){ EditField::SetConvert(*this); }

	virtual Value  Format(const Value& q) const;
	virtual Value  Scan(const Value& text) const;
	
	virtual void Updated();

	void SetKeyValue(const Value&key, const Value& value);
	EditKeyValue& MarkAsDirty(){ matched=false; return *this; }

	EditKeyValue& SetConvert(const Convert& conv){ chained=&conv; return *this; }
protected:
	Value key;
	bool  matched;
	const Convert* chained;
};

END_UPP_NAMESPACE

#endif
