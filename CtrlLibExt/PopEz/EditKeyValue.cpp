#include "PopEz.h"
namespace lz {

Value EditKeyValue::Format(const Value& q) const
{
	ASSERT(convert == this);
	return IsMatched() ? Value(text) : chained ? chained->Format(q) : Convert::Format(q);
}
// if matched, return key, else do more
Value EditKeyValue::Scan(const Value& text) const
{
	ASSERT(convert == this);
	return IsMatched() ? key : !HasFocus() && chained ? chained->Scan(text) : Null;
}

void EditKeyValue::Updated()
{
	SetMatched(false);
	key = Null;
	EditField::Updated();
}

void EditKeyValue::SetData(const Value& data)
{
	SetMatched(false);
	this->SetKeyValue(data, (WString)Format(data));
}

Value EditKeyValue::GetData() const
{
	return IsMatched() ? key : chained ? chained->Scan(text) : Null;
}

void EditKeyValue::SetKeyValue(const Value& _key, const Value& value)
{
	Update();
	key = _key;
	text = value;
	SetMatched(true);
	SetModify();
	if(HasFocus())
		EditField::SelectAll();
	Refresh();
}

} // namespace lz}//eons lz