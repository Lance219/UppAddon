class FakeCtrl;

FakeCtrl* FocusFakeCtrl();

class FakeCtrl
{
friend FakeCtrl* FocusFakeCtrl();

public:
	virtual void Paint(Draw& w, Size sz){}
	virtual void LeftDown(Point p, dword keyflags){}
	virtual ~FakeCtrl();
	
	void   CancelMyPreedit()                   { /*if(HasFocus()) CancelPreedit();*/ }

	bool    SetFocus();
	bool    HasFocus() const                   { return FocusFakeCtrl() == this; }

protected:
	enum {
		ATTR_LAYOUT_ID,
		ATTR_TIP,
		ATTR_HELPLINE,
		ATTR_DESCRIPTION,
		ATTR_HELPTOPIC,
		ATTR_MEGARECT_X,
		ATTR_MEGARECT_Y,
		ATTR_LAST
	};
	
	void   SetTextAttr(int ii, const char *s);
	void   SetTextAttr(int ii, const String& s);
	String GetTextAttr(int ii) const;
	
	void   SetColorAttr(int ii, Color c);
	Color  GetColorAttr(int ii) const;
	
	void   SetFontAttr(int ii, Font fnt);
	Font   GetFontAttr(int ii) const;
	
	void   SetIntAttr(int ii, int val);
	int    GetIntAttr(int ii, int def = Null) const;

	void   SetInt64Attr(int ii, int64 val);
	int64  GetInt64Attr(int ii, int64 def = Null) const;
	
	void   SetVoidPtrAttr(int ii, const void *ptr);
	void  *GetVoidPtrAttr(int ii) const;
	
	template <class T>
	void  DeleteAttr(int ii)      { void *p = GetVoidPtrAttr(ii); if(p) { delete (T *)p; SetVoidPtrAttr(ii, nullptr); }; }

	template <class T>
	T&    CreateAttr(int ii)      { DeleteAttr<T>(ii); T *q = new T; SetVoidPtrAttr(ii, q); return *q; }
	
	template <class T>
	T     GetAttr(int ii) const   { void *p = GetVoidPtrAttr(ii); return p ? *(T *)p : T(); }
	

	
private:
	Upp::PackedData attrs;
	
	Upp::PackedData& Attrs();

static FakeCtrl* focus_fctrl;
};

inline FakeCtrl* FocusFakeCtrl()
{
	return FakeCtrl::focus_fctrl;
}