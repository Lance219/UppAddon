#ifndef _PopEasy_EditKeyValue_h_
#define _PopEasy_EditKeyValue_h_

#include <CtrlLibExt/CtrlLibExt.h>

BEGIN_NAMESPACE_LZ
//
//using Upp::EditField, Upp::Convert, Upp::Value, Upp::Point;
//using Upp::dword, Upp::WString;

class EditKeyValue : public EditField, public Upp::Convert
{
public:

	EditKeyValue() : chained(nullptr){ EditField::SetConvert(*this); }

	Value  Format(const Value& key) const override;
	Value  Scan(const Value& text) const override;
	void SetData(const Value& value)override;
	Value GetData()const override;
	void Updated()override;
	void LeftDouble(Point p, dword keyflags)override{ WhenLeftDouble(); }

	void SetKeyValue(const Value&key, const Value& value);

	EditKeyValue& SetConvert(const Convert& conv){ chained=&conv; return *this; }

private:
	Value key;
	const Convert* chained;

#ifndef CPU_64
	bool  matched;
#endif
	
	bool IsMatched()const{
#ifdef CPU_64
		return GetReserved(this)==1;
#else
		return matched;
#endif
	}
	void SetMatched(bool v){
#ifdef CPU_64
		SetReserved(this, (int32)(v?1:0) );
#else
		matched = v;
#endif
	}

public:
	Upp::Event<> WhenLeftDouble;
};

END_NAMESPACE
#endif
