// internal header
// so that partially visible Ctrl works nicely with GridEz
//
// a Box can limit the visible portion of a contained Ctrl
//
// a Box never OWN its child.
namespace _detail{
class Box: public Ctrl
{
public:
	Box(Ctrl& c, const Rect& rect, const Rect& visible_rect)
	{
		Enter _(false);
		Hold(c, rect, visible_rect);
	}
	
	void Unbox(){
		if(Ctrl* p; contained && (p=GetParent())!=nullptr)
		{
			Ctrl * f = contained->HasFocusDeep() ? Ctrl::GetFocusCtrl() : nullptr;
			p->AddChild(contained, this);
			if(f)
			{
				DUMP(Ctrl::GetFocusCtrl() == f);
				f->SetFocus();
			}
		}
		Remove();
		if(IsOwned(this))
		{
			;
		}
	}
	
	Box& Hold(Ctrl& c, const Rect& rect, const Rect& visible_rect)
	{
		Enter _(false);
		Ctrl * f = c.HasFocusDeep() ? Ctrl::GetFocusCtrl() : nullptr;
		_.Log("1");
		contained = &c;
		if(auto p = c.GetParent(); p != nullptr )
		{
			p->AddChild(this, contained);
		}
		_.Log("2");
		Add(c);
		_.Log("3");
		SetRects(rect, visible_rect);
		if(f)
			f->SetFocus();
		return *this;
	}
	
	void SetRects(const Rect& rect, const Rect& visible_rect)
	{
		ASSERT(contained!=nullptr);
		SetRect(visible_rect);
//		DUMP(rect - visible_rect.TopLeft());
//		DUMP(rect);
//		DUMP(visible_rect);
		
		contained->SetRect(rect - visible_rect.TopLeft());
	}
private:
	Ctrl *contained;
};

}

inline bool IsBoxed(Ctrl& c)
{
	Ctrl * p;
	return (p=c.GetParent())!=nullptr && dynamic_cast<_detail::Box*>(p)!=nullptr;
}

inline void Box(Ctrl& c, const Rect& rect, const Rect& visible_rect)
{
	Enter _(false);
	static int count;
	++count;
	LOG("Box created");
	LOG(count);
//	_detail::Box b(c, rect, visible_rect);
	ASSERT(c.GetParent() && dynamic_cast<GridEz*>(c.GetParent())!=nullptr);
	_.Log("1");
	One<_detail::Box> b;
	b.Create(c, rect, visible_rect);
	_.Log("2");
	MarkAsOwned(b.Detach());
	_.Log("3");
}

inline void Unbox(Ctrl& c)
{
	Enter _(false);

	if(IsBoxed(c))
	{
		_detail::Box* b = (_detail::Box*)c.GetParent();
		b->Unbox();
		if(IsOwned(b)){
			delete b;
		}
	}
}

inline void CheckBox(Ctrl& c, const Rect& rect, const Rect& visible_rect)
{
	Enter _(false);
	if(IsBoxed(c))
	{
		auto& b = *reinterpret_cast<_detail::Box*>(c.GetParent());
		b.SetRects(rect, visible_rect);
	}else
		Box(c, rect, visible_rect);
}