#include "../CtrlCoreExt.h"

BEGIN_NAMESPACE_LZ

FakeCtrl* FakeCtrl::focus_fctrl;

FakeCtrl::~FakeCtrl()
{
}


String FakeCtrl::GetTextAttr(int ii) const
{
	return /*layout_id_literal ? String() :*/ attrs.GetString(ii);
}

void FakeCtrl::SetTextAttr(int ii, const String& s)
{
	Attrs().SetString(ii, s);
}

Upp::PackedData& FakeCtrl::Attrs()
{
//	if(layout_id_literal) {
//		String layout_id((const char *)attrs.GetRawPtr());
//		attrs.SetRawPtr(nullptr);
//		attrs.SetString(ATTR_LAYOUT_ID, layout_id);
//		layout_id_literal = false;
//	}
	return attrs;
}

void FakeCtrl::SetColorAttr(int ii, Color c)
{
	Attrs();
	if(IsNull(c))
		attrs.SetNull(ii);
	else
		attrs.SetDword(ii, c.GetRaw());
}

Color FakeCtrl::GetColorAttr(int ii) const
{
//	if(layout_id_literal)
//		return Null;
	static dword nullval = Color(Null).GetRaw();
	return Color::FromRaw(attrs.GetDword(ii, nullval));
}

void FakeCtrl::SetFontAttr(int ii, Font fnt)
{
	Attrs();
	if(IsNull(fnt))
		attrs.SetNull(ii);
	else
		attrs.SetInt64(ii, fnt.AsInt64());
}

Font FakeCtrl::GetFontAttr(int ii) const
{
//	if(layout_id_literal)
//		return Null;
	static int64 nullval = Font(Null).AsInt64();
	return Font::FromInt64(attrs.GetInt64(ii, nullval));
}

void FakeCtrl::SetVoidPtrAttr(int ii, const void *ptr)
{
	if(ptr)
		Attrs().SetPtr(ii, (void *)ptr);
	else
		Attrs().SetNull(ii);
}

void *FakeCtrl::GetVoidPtrAttr(int ii) const
{
//	if(layout_id_literal)
//		return NULL;
	return attrs.GetPtr(ii);
}

bool FakeCtrl::SetFocus()
{
	Upp::GuiLock __;
	//LLOG("FakeCtrl::SetFocus(" << Name() << ")");
	focus_fctrl = this;
	return true;
}


END_NAMESPACE