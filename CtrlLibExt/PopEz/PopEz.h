#ifndef _PopEz_PopEz_h_
#define _PopEz_PopEz_h_

#include <CtrlLibExt/CtrlLibExt.h>
//#include <GridEz/GridEz.h>
#include "bits/EditKeyValue.h"

BEGIN_NAMESPACE_LZ

struct PopMainEx;

struct PopSlave
{
	PopSlave() = default;

	enum class HintReason : IntOfSize<sizeof(bool)>
	{
		Show,
		Changed
	};

	struct PopupHint
	{
		String text;
		int     sel_start; // if sel_start==sel_end, it's the caret position.
		int     sel_end;

		HintReason reason;
		bool    no_autocompletion; // if last_key==K_BACKSPACE, no auto completion should be performed

		bool operator == ( const PopupHint& h ) const
		{
			return text == h.text && sel_start == h.sel_start && sel_end == h.sel_end;
		}

		bool operator != ( const PopupHint& h ) const
		{
			return ! ( *this == h );
		}

		String ToString() const
		{
			return String().Cat() << "Text:" << text << ", sel_start:" << sel_start << ", sel_end: " << sel_end;
		}

		// mouse location omitted for now
	};



	virtual bool DoKey ( dword key, int repcount );

	virtual int PrepareData ( const PopupHint& hint, String* suffix ) = 0;
	virtual Size GetPreferredSize() const;
	virtual bool Select()
	{
		return true;
	}


	Ctrl* GetCtrl()
	{

		struct _P: PopSlave, Ctrl
		{
		};

		return reinterpret_cast<_P*> ( this );
	}

	const   Ctrl* GetCtrl() const
	{
		return const_cast<const Ctrl*> (
				   const_cast<PopSlave&> ( *this ).GetCtrl()
			   );
	}

	void SetMainEx ( PopMainEx* _main )
	{
		main = _main;
	}

	void ClearMainEx()
	{
		SetMainEx ( nullptr );
		Unpop();
	}

	PopMainEx* GetMainEx()
	{
		return main;
	}

	const PopMainEx* GetMainEx() const
	{
		return main;
	}

	Ctrl* GetMainCtrl();
	const Ctrl* GetMainCtrl() const
	{
		return const_cast<const Ctrl*> (
				   const_cast<PopSlave&> ( *this ).GetMainCtrl()
			   );
	}

	bool IsOpen() const
	{
		return GetCtrl()->IsOpen();
	}

	void NegotiateSizePosition ( Rect& rect );

	void PopUp ( Rect& r );
	void PopUp();

	void Unpop()
	{
		GetCtrl()->Close();
	}

	inline bool GetAutoPopUp() const;
	inline bool GetAutoComplete() const;
	inline bool GetAutoSelect() const;

	void AddHint ( const PopupHint& hint, PopMainEx * main );
	void ProcessHint();

	bool IsCurHintObselete() const
	{
		return hint_count > 1;
	}

	private:
	PopMainEx* main = nullptr;

protected:
	PopupHint hints[2];
	union
	{

		struct
		{
			int16  cur_hint_index;
			volatile int16  hint_count;
		};

		int32 _dummy = 0;
	};

};

class ListPopBase: public PopSlave, public lz::GridEz
{
public:
	ListPopBase();

	int PrepareData ( const PopupHint& hint, String* suffix ) override;
	bool Select() override;
	void LeftDouble ( Point p, dword keyflags ) override;
	// define the storage interface
	//
	virtual int GetRowCount() const = 0;
	virtual int GetColumnCount() const = 0;
	virtual Value GetCellAt ( int row, int col ) const = 0;

protected:
	int batch_size;
};


class ListPopUp: public ListPopBase
{

	public:
		void Add ( const Vector<Value>& v )
		{
			data.Add ( Vector<Value> ( v, 1 ) );
		}
		



		template <typename... Args>
		void Add (Args&&... params)
		{
			auto& vv = data.Add();
			if constexpr(sizeof...(params)!=0){
				vv.Reserve(sizeof...(params));
				(vv.Add(std::forward<Args>(params)),...);
			}
		}

	protected:
		virtual int GetRowCount() const
		{
			return data.GetCount();
		}

		virtual int GetColumnCount() const
		{
			return data.GetCount() == 0 ? 0 : data[0].GetCount();
		}

		virtual Value GetCellAt ( int row, int col ) const
		{
			if ( row >= data.GetCount() || col >= data[row].GetCount() )
				return Null;

			return data[row][col];
		}

		Vector<Vector<Value> > data; // provides naive local storage
};

class TreePopup: public PopSlave, public Upp::TreeCtrl
{

	public:
		TreePopup() {}
};

class TreeListPopUp: public PopSlave, public Upp::Splitter
{

	public:
		enum ViewStyle{VS_TreeOnly, VS_ListOnly, VS_TreeList};

		TreeListPopUp();

		TreeListPopUp& SetViewStyle ( ViewStyle style );
		TreeListPopUp& SetKeyColumn ( int col );
		int	GetKeyColumn() const;

		bool DoKey ( dword key, int repcount ) override;
		bool Select() override;
		Size GetPreferredSize() const override;
		virtual void RefreshTree() {} // newly added

		virtual void RefreshList() {} // newly added

		typedef PopSlave _Parent;

	protected:
		GridEz		list;
		TreeCtrl   tree;
		Ctrl*      fake_focus_ctrl;
		String     hint_text;
		ViewStyle  view_style;
		int		   key_col;


		void FakeShiftFocus();

		void TreeWhenLeft();
		void ListLeftDouble();
		void ListWhenLeft();
};

struct PopMainEx
{
	PopMainEx (uint16 _delta) : delta ( _delta ),
		man ( nullptr ), dummy ( 0 )
	{
	}

	virtual void GetSizePosition ( Rect& rt )
	{
		rt = Null;
	}

	virtual void GetPopupHint ( PopSlave::PopupHint& hint );
	virtual void AcceptData ( const Value& key, const Value& text )
	{
		GetMain().SetData ( key );
		WhenSelect();
	}

	virtual void AcceptSuffix ( const PopSlave::PopupHint& hint, String suffix );
	virtual bool UnacceptSuffix();
	virtual void Cancel() {} // when user closes popup without make a selection, eg by Esc key.

	virtual PopMainEx& SetAutoPopUp ( bool popup = true )
	{
		noautopopup = !popup;
		return *this;
	}

	virtual bool GetAutoPopup() const
	{
		return !noautopopup;
	}

	virtual PopMainEx& SetAutoComplete ( bool v = true )
	{
		autocomplete = v;
		return *this;
	}

	virtual bool GetAutoComplete() const
	{
		return autocomplete;
	}

	virtual PopMainEx& SetAutoSelect ( bool v = true )
	{
		noautoselect = !v;
		return *this;
	}

	virtual bool GetAutoSelect() const
	{
		return !noautoselect;
	}


	Ctrl& GetMain()
	{
		return *reinterpret_cast<Ctrl*> (
				   reinterpret_cast<char*> ( this ) - delta
			   );
	}

	const Ctrl& GetMain() const
	{
		return const_cast<const Ctrl&> (
				   const_cast<PopMainEx&> ( *this ).GetMain()
			   );
	}


	PopSlave* GetSlave()
	{
		return man;
	}

	const PopSlave* GetSlave() const
	{
		return man;
	}

	void SetPopSlave ( PopSlave* _new )
	{
		if ( man )
			man->ClearMainEx();

		man = _new;

		man->SetMainEx ( this );
	}

	Event<>              WhenSelect;

protected:
	PopSlave  *man;
	union
	{

		struct
		{

			bool noautopopup:1;
			bool autocomplete:1;
			bool noautoselect:1;
			bool bit3:1; // make available to derived class
			bool bit4:1;
			bool bit5:1;
			bool bit6:1;
			bool bit7:1;
			uint8 len_of_suffix_just_accepted;
		};

		int16 dummy;
	};
	const uint16 delta; // delta is used to convert this to WithPopUp<T>
};

bool PopSlave::GetAutoPopUp() const
{
	return GetMainEx() && GetMainEx()->GetAutoPopup();
}

bool PopSlave::GetAutoComplete() const
{
	return GetMainEx() && GetMainEx()->GetAutoComplete();
}

bool PopSlave::GetAutoSelect() const
{
	return GetMainEx() && GetMainEx()->GetAutoSelect();
}

void WithPopupSetup ( Ctrl& c, MultiButtonFrame& frame, PopMainEx& ex );

template <CtrlClass T>class WithPopup : public T, public PopMainEx
{

	public:
	typedef WithPopup<T> _Self;
	typedef T			 _Parent;

	WithPopup() : PopMainEx ( static_cast<uint16>(sizeof ( T ) ) )
	{
		static_assert( sizeof(T) <= std::numeric_limits<uint16>::max(),
			"Ctrl size exceeds the designed limitation of 65535"
		);
		
		WithPopupSetup ( *this, frame, *this );
	}


	_Self& SetButtonStyle ( const MultiButton::Style& style )
	{
		frame.SetStyle ( style );
		return *this;
	}
	
	bool Key ( dword key, int count ) override;
	
	bool IsPopupOpen()
	{
		return man && man->GetCtrl()->IsOpen();
	}

	bool LeafOnly() const
	{
		return bit7;
	}

	_Self& LeafOnly ( bool b )
	{
		bit7 = b;
		return *this;
	}

	GateN<dword, int> OnMainKey; //(dword key, int rep);

protected:
	void Unpop()
	{
		if ( man && man->GetMainCtrl() == this )
			man->Unpop();
	}
	
	void GotFocus() override
	{
		_Parent::GotFocus();
		frame.RefreshFrame();
	}
	void LostFocus() override
	{
		Unpop();
		_Parent::LostFocus();
		frame.RefreshFrame();
	}
	
	void State ( int reason ) override
	{
	
		if ( reason == static_cast<int> ( Ctrl::POSITION ) && IsPopupOpen() )
		{
			Rect rt;
			man->NegotiateSizePosition ( rt );
			man->PopUp ( rt );
		}
	
		else
			if ( !this->IsVisible() )
				Unpop();
	
		this->_Parent::State ( reason );
	}

protected:
	MultiButtonFrame frame;


};

template <CtrlClass T>
bool WithPopup<T>::Key ( dword key, int count )
{
	if ( man )
	{
		PopSlave::PopupHint hint;

		switch ( key )
		{

			case K_ALT_DOWN:

			case K_ALT_UP:

				if ( IsPopupOpen() )
				{
					Unpop();

				}

				else
				{
					hint.reason = PopSlave::HintReason::Show;
					this->GetPopupHint ( hint );
					hint.text.Clear();
					man->AddHint ( hint, this );
				}

				return true;

			default:

				if ( IsPopupOpen() && man->DoKey ( key, count ) )
					return true;
		}

		if ( OnMainKey && OnMainKey ( key, count ) )
			return true;

		if ( ( key == K_BACKSPACE && len_of_suffix_just_accepted != 0 && UnacceptSuffix() )
			 ||	this->T::Key ( key, count )
		   )
		{
			hint.reason = PopSlave::HintReason::Changed;
			this->GetPopupHint ( hint );
			hint.no_autocompletion = key == K_BACKSPACE;

			if ( IsPopupOpen() || GetAutoPopup() )
			{
				man->AddHint ( hint, this );
			}

			len_of_suffix_just_accepted = 0;

			return true;
		}
	}

	return ( OnMainKey && OnMainKey ( key, count ) ) || this->T::Key ( key, count );
}

using EditEz = WithPopup<EditKeyValue>;

END_NAMESPACE

#endif
