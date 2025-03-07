#include "../DataSet.h"

namespace Upp{
using recordsetIter=lz::RecordSet::Iterator;
template <>
inline void IterSwap<recordsetIter> ( recordsetIter i, recordsetIter j )
{
	lz::RecordSet::DoSwapRecord ( i, j );
}
}

BEGIN_NAMESPACE_LZ

#define RECORDSET_SORT
#ifdef RECORDSET_SORT




void RecordSet::Sort(const Vector<int>& by)
{
	if(by.IsEmpty())
		return;

	Upp::Sort(*this, [&, this](const Record& lhs, const Record& rhs){
		for(int i = 0; i < by.GetCount(); ++i) {
			int col = by[i];
			bool desc = col < 0;
			int comp = 0;
	
			if(desc)
				col = -col;
	
			--col;
	
			FieldDef::FieldValue l = lhs[col], r = rhs[col];
	
			if(l.IsNull()) {
				if(!r.IsNull())
					comp = -1; // null is less than non-null
				else
					comp = 0; // null is same as null
			}
	
			else if(r.IsNull()) {
				comp = 1; // null is less than non-null
			}
	
			else {
				// both are non-null
				comp = compare(l.GetFieldDef().GetFieldType(), l.location(), r.location());
			}
	
			if(desc)
				comp = -comp;
	
			if(comp < 0)
				return true;
			else if(comp > 0)
				return false;
		}
	
		return false;
		
	});
}

Vector<int> RecordSet::ParseSortBy ( const char * sortby)
{
	return Vector<int>();
}


#endif

String FieldDef::FieldValue::ToString() const
{
	return (*this).IsNull() ? String("Null") : Get(String().Cat());
}

String SqlFieldValue(FieldDef::FieldValue v)
{
	if(v.IsNull())
		return "NULL";

	String s;

	return SqlFieldValue1(v.Get(s));
}

// Re: return value:
// for bit, it's a value (0/1),
// for other type it's the address where actual value is stored.
//
const void* FieldDef::FieldValue::DoGet() const
{
	return fldDef->tid == FT_BIT ? (GetBit(base, fldDef->start) ? (void*)1 : (void*)0)
	                             : const_cast<const char*>(location());
}

void FieldDef::FieldValue::SetNull()
{
	if(!IsNull()) {
		switch(fldDef->tid) {

		case FT_STRING:
			reinterpret_cast<String*>(location())->~String();
#ifdef _DEBUG
			memset(location(), 0, sizeof(String));
#endif
		default:;
		}

		MarkAsNotSet();
	}
}

// the case when inTid==FT_PCHAR has been taken care of elsewhere
//
static void* SetToBool(void* dest, int id, const void* data)
{
	bool v;

	switch(id) {

	case FT_BOOL:
		v = *(const bool*)data;
		break; // 1

	case FT_INT8:
	case FT_UINT8:
		v = *(const int8*)data != 0;
		break; // 2 char

	case FT_INT16:
	case FT_UINT16:
		v = *(const int16*)data != 0;
		break;

	case FT_INT32:
	case FT_UINT32:
		v = *(const int32*)data != 0;
		break;

	case FT_INT64:
	case FT_UINT64:
		v = *(const int64*)data != 0;
		break;

	case FT_FLOAT:
		v = *(const float*)data != 0.;
		break; // 10

	case FT_DOUBLE:
		v = *(const double*)data != 0.;
		break;

	case FT_STRING: {
		const String& s = *(const String*)data;

		if(IsNull(s) || s.IsEmpty())
			v = false;
		else if(!ParseBool(s, v))
			v = true;
	}

	break;

	case FT_TIME:
		v = !IsNull(*(const Time*)data);
		break;
		//    case FT_BLOB:
		//    	v=true;
		//		break;

	case FT_PCHAR: {
		const char* p = (const char*)data;

		if(p == NULL || p[0] == '\0')
			v = false;
		else if(!ParseBool(p, v))
			v = true;
	}

	break; // char *

	case FT_VALUE:
		v = (bool)*(Value*)data;
		break; // u++ value

	default:
		throw std::runtime_error("SetToBool from wrong data");
	}

	*(bool*)dest = v;

	return dest;
}

static void* SetToInt1(void* dest, int id, const void* data)
{
	int8 v;

	switch(id) {

	case FT_BOOL:
		v = *(const bool*)data;
		break; // 1

	case FT_INT8:

	case FT_UINT8:
		v = *(const int8*)data;
		break; // 2 char

	case FT_INT16:

	case FT_UINT16:
		v = (int8) * (const int16*)data;
		break;

	case FT_INT32:

	case FT_UINT32:
		v = (int8) * (const int32*)data;
		break;

	case FT_INT64:

	case FT_UINT64:
		v = (int8) * (const int64*)data;
		break;
		break; // unsigned char

	case FT_FLOAT:
		v = (int8) * (const float*)data;
		break; // 10

	case FT_DOUBLE:
		v = (int8) * (const double*)data;
		break;
		//	case FT_STRING:
		//		v=!IsNull(*(String*)data) && !(*(String*)data).IsEmpty();
		//		break;
		//    case FT_TIME:
		//    	v=!IsNull(*(Time*)data);
		//		break;
		//    case FT_BLOB:
		//    	v=true;
		//		break;

	case FT_PCHAR:
		v = (int8)atoi((const char*)data);
		break; // char *

	case FT_VALUE:
		v = (int8)(int64) * (const Value*)data;
		break; // u++ value

	default:
		throw std::runtime_error("SetToInt1 from wrong data");
	}

	*(int8*)dest = v;

	return dest;
}

static void* SetToInt2(void* dest, int id, const void* data)
{
	int16 v;

	switch(id) {

	case FT_BOOL:
		v = (int16) * (const bool*)data;
		break; // 1

	case FT_INT8:

	case FT_UINT8:
		v = (int16)(int64) * (const int8*)data;
		break; // 2 char

	case FT_INT16:

	case FT_UINT16:
		v = *(const int16*)data;
		break;

	case FT_INT32:

	case FT_UINT32:
		v = (int16) * (const int32*)data;
		break;

	case FT_INT64:

	case FT_UINT64:
		v = (int16) * (const int64*)data;
		break;
		break; // unsigned char

	case FT_FLOAT:
		v = (int16) * (const float*)data;
		break; // 10

	case FT_DOUBLE:
		v = (int16) * (const double*)data;
		break;
		//	case FT_STRING:
		//		v=!IsNull(*(String*)data) && !(*(String*)data).IsEmpty();
		//		break;
		//    case FT_TIME:
		//    	v=!IsNull(*(Time*)data);
		//		break;
		//    case FT_BLOB:
		//    	v=true;
		//		break;

	case FT_PCHAR:
		v = (int16)atoi((const char*)data);
		break; // char *

	case FT_VALUE:
		v = (int16)(int64) * (const Value*)data;
		break; // u++ value

	default:
		throw std::runtime_error("SetToInt2 from wrong data");
	}

	*(int16*)dest = v;

	return dest;
}

static void* SetToInt32(void* dest, int id, const void* data)
{
	int32 v;

	switch(id) {

	case FT_BOOL:
		v = (int32) * (const bool*)data;
		break; // 1

	case FT_INT8:

	case FT_UINT8:
		v = (int32) * (const int8*)data;
		break; // 2 char

	case FT_INT16:

	case FT_UINT16:
		v = (int32) * (const int16*)data;
		break;

	case FT_INT32:

	case FT_UINT32:
		v = *(const int32*)data;
		break;

	case FT_INT64:

	case FT_UINT64:
		v = (int32) * (const int64*)data;
		break;
		break; // unsigned char

	case FT_FLOAT:
		v = (int32) * (const float*)data;
		break; // 10

	case FT_DOUBLE:
		v = (int32) * (const double*)data;
		break;
		//	case FT_STRING:
		//		v=!IsNull(*(String*)data) && !(*(String*)data).IsEmpty();
		//		break;
		//    case FT_TIME:
		//    	v=!IsNull(*(Time*)data);
		//		break;
		//    case FT_BLOB:
		//    	v=true;
		//		break;

	case FT_PCHAR:
		v = atoi((const char*)data);
		break; // char *

	case FT_VALUE:
		v = (int32)(int64) * (Value*)data;
		break; // u++ value

	default:
		throw std::runtime_error("SetToInt32 from wrong data");
	}

	*(int32*)dest = v;

	return dest;
}

static void* SetToInt8(void* dest, int id, const void* data)
{
	int64 v;

	switch(id) {

	case FT_BOOL:
		v = (int64) * (const bool*)data;
		break; // 1

	case FT_INT8:

	case FT_UINT8:
		v = (int64) * (const int8*)data;
		break; // 2 char

	case FT_INT16:

	case FT_UINT16:
		v = (int64) * (const int16*)data;
		break;

	case FT_INT32:

	case FT_UINT32:
		v = (int64) * (const int32*)data;
		break;

	case FT_INT64:

	case FT_UINT64:
		v = (int64) * (const int64*)data;
		break;
		break; // unsigned char

	case FT_FLOAT:
		v = (int64) * (const float*)data;
		break; // 10

	case FT_DOUBLE:
		v = (int64) * (const double*)data;
		break;
		//	case FT_STRING:
		//		v=!IsNull(*(String*)data) && !(*(String*)data).IsEmpty();
		//		break;
		//    case FT_TIME:
		//    	v=!IsNull(*(Time*)data);
		//		break;
		//    case FT_BLOB:
		//    	v=true;
		//		break;

	case FT_PCHAR:
		v = Upp::ScanInt64((const char*)data);
		break; // char *

	case FT_VALUE:
		v = (int64) * (Value*)data;
		break; // u++ value

	default:
		throw std::runtime_error("SetToInt8 from wrong data");
	}

	*(int64*)dest = v;

	return dest;
}

static void* SetToFloat(void* dest, int id, const void* data)
{
	float v;

	switch(id) {

	case FT_BOOL:
		v = (float)*(const bool*)data;
		break; // 1

	case FT_INT8:

	case FT_UINT8:
		v = (float)*(const int8*)data;
		break; // 2 char

	case FT_INT16:

	case FT_UINT16:
		v = (float)*(const int16*)data;
		break;

	case FT_INT32:

	case FT_UINT32:
		v = (float)*(const int32*)data;
		break;

	case FT_INT64:

	case FT_UINT64:
		v = (float)*(const int64*)data;
		break;
		break; // unsigned char

	case FT_FLOAT:
		v = *(const float*)data;
		break; // 10

	case FT_DOUBLE:
		v = (float)*(const double*)data;
		break;
		//	case FT_STRING:
		//		v=!IsNull(*(String*)data) && !(*(String*)data).IsEmpty();
		//		break;
		//    case FT_TIME:
		//    	v=!IsNull(*(Time*)data);
		//		break;
		//    case FT_BLOB:
		//    	v=true;
		//		break;

	case FT_PCHAR:
		v = (float)atof((const char*)data);
		break;

	case FT_VALUE:
		v = (float)(double)*(Value*)data;
		break; // u++ value

	default:
		throw std::runtime_error("SetToInt2 from wrong data");
	}

	*(float*)dest = v;

	return dest;
}

static void* SetToDouble(void* dest, int id, const void* data)
{
	double v;

	switch(id) {

	case FT_BOOL:
		v = (double)*(const bool*)data;
		break; // 1

	case FT_INT8:
		v = *(const int8*)data;
		break;

	case FT_UINT8:
		v = *(const uint8*)data;
		break; // 2 char

	case FT_INT16:
		v = *(const int16*)data;
		break;

	case FT_UINT16:
		v = *(const uint16*)data;
		break;

	case FT_INT32:
		v = *(const int32*)data;
		break;

	case FT_UINT32:
		v = *(const uint32*)data;
		break;

	case FT_INT64:
		v = (double)*(const int64*)data;
		break;

	case FT_UINT64:
		v = (double)*(const uint64*)data;
		break; // unsigned char

	case FT_FLOAT:
		v = *(const float*)data;
		break; // 10

	case FT_DOUBLE:
		v = *(const double*)data;
		break;

	case FT_STRING: {
		const String& s = *(const String*)data;

		if(IsNull(s))
			v = Null;
		else
			v = atof(s);
	}

	break;
		//    case FT_TIME:
		//    	v=!IsNull(*(Time*)data);
		//		break;
		//    case FT_BLOB:
		//    	v=true;
		//		break;

	case FT_PCHAR:
		v = atof((const char*)data);
		break; // char *

	case FT_VALUE:
		v = (double)*(Value*)data;
		break; // u++ value

	default:
		throw std::runtime_error("SetToDouble from wrong data");
	}

	*(double*)dest = v;

	return dest;
}

static void* SetToString(void* dest, int id, const void* data)
{
	String& v = *(String*)dest;
	v.Clear();

	switch(id) {

	case FT_BOOL:
		v << *(const bool*)data;
		break; // 1

	case FT_INT8:
		v << *(const int8*)data;
		break;

	case FT_UINT8:
		v << *(const uint8*)data;
		break; // 2 char

	case FT_INT16:
		v << *(const int16*)data;
		break;

	case FT_UINT16:
		v << *(const uint16*)data;
		break;

	case FT_INT32:
		v << *(const int32*)data;
		break;

	case FT_UINT32:
		v << *(const uint32*)data;
		break;

	case FT_INT64:
		v << *(const int64*)data;
		break;

	case FT_UINT64:
		v << *(const uint64*)data;
		break;

	case FT_FLOAT:
		v << *(const float*)data;
		break; // 10

	case FT_DOUBLE:
		v << *(const double*)data;
		break;

	case FT_PCHAR:
		v << (const char*)data;
		break;

	case FT_VALUE:
		v << *(const Value*)data;
		break;

	case FT_TIME:
		v << *(const Time*)data;
		break;

	case FT_STRING:
		v << *(const String*)data;
		break;

	default:
		throw std::runtime_error("SetToInt16 from wrong data");
	}

	return dest;
}

// 20241101000000.000[-5:EDT]
// 20241012065958
//
// milliseconds and timezone ignored
//
static Time	ReadOfxTimeValue(const char * ofxtime)
{
	auto readint=[](const char *& beg, int len, int& v)
	{
		auto r = std::from_chars(beg, beg+len, v).ec != std::errc::invalid_argument;
		beg += len;
		return r;
	};
	std::string_view s{ofxtime};
	
	if(auto v = s.find_first_not_of("\t \r\n"); v!=s.npos)
		s.remove_prefix(v);
	if(s.empty())
		return Null;

	if(auto v = s.find_first_of(". [\n"); v!=s.npos)
		s.remove_suffix(s.size()-v);
	if(s.length()!=14)
		return Null;
	
	int y,m,d,h,min,sec;
	const char * p= s.data();
	return readint(p,4,y) && readint(p,2,m) && readint(p,2,d) &&
		readint(p,2,h) && readint(p,2,min) && readint(p,2,sec) ?
		Time(y,m,d,h,min,sec) : Null;
	
}


static void* SetToTime(void* dest, int id, const void* data)
{
	Time& t = *(Time*)dest;

	switch(id) {

	case FT_PCHAR:
		{
			const char * p = (const char*)data;
			if (StrToTime(t, p ) == nullptr )
				t = ReadOfxTimeValue(p);
		// throw std::runtime_error("Invalid Time String");
		}
		break;

	case FT_VALUE:
		t = *(const Value*)data;

		break;

	case FT_TIME:
		t = *(const Time*)data;

		break;

	case FT_STRING:
		if(StrToTime(t, (*(const String*)data)) == NULL)
			throw Upp::Exc("Invalid Time String");

		break;

	default:
		throw Upp::Exc("SetToTime from wrong data");
	}

	return dest;
}

// work horse for data-copying of data types other than bit
//
static void SetData0(void* dest, FieldTypeId did, const void* src, FieldTypeId inid)
{
	switch(did) {

	case FT_BOOL:
		SetToBool(dest, inid, src);
		break;

	case FT_INT8:
		SetToInt1(dest, inid, src);
		break; // 2 char

	case FT_INT16:
		SetToInt2(dest, inid, src);
		break; // 3

	case FT_INT32:
		SetToInt32(dest, inid, src);
		break; // 4

	case FT_INT64:
		SetToInt8(dest, inid, src);
		break;

	case FT_UINT8:
		SetToInt1(dest, inid, src);
		break; // unsigned char

	case FT_UINT16:
		SetToInt2(dest, inid, src);
		break; // 7

	case FT_UINT32:
		SetToInt32(dest, inid, src);
		break;

	case FT_UINT64:
		SetToInt8(dest, inid, src);
		break;

	case FT_FLOAT:
		SetToFloat(dest, inid, src);
		break; // 10

	case FT_DOUBLE:
		SetToDouble(dest, inid, src);
		break;

	case FT_STRING:
		SetToString(dest, inid, src);
		break;

	case FT_TIME:
		SetToTime(dest, inid, src);
		break;

	default:
		// error;
		String s;
		s << "Error in SetData0, Source Field: " << TypeName(inid)
		  << ", Destination Field:" << TypeName(did);
		LOG(s);
	}
}

void* FieldDef::FieldValue::DoGet(void* dest, FieldTypeId did) const
{
	FieldTypeId id = fldDef->tid;
	const void* data = DoGet();
	bool dummy;
	String empty;
	// when id is FT_BIT, special treatment required

	if(id == FT_BIT) {
		dummy = data != NULL;
		data = &dummy;
		id = FT_BOOL; // basically from now on bit is exactly same as bool, and id of FT_BIT
					  // will not be encounter by subsequent branches.
	}

	else if(id == FT_STRING && IsNull()) {
		data = &empty;
	}

	SetData0(dest, did, data, id);

	return dest;
}

void FieldDef::FieldValue::PODSet(const void* data)
{
	FieldTypeId id = fldDef->tid;
	memcpy(location(), data, lz::TypeSize(id));
	MarkAsSet();
}

// we can make use of SetData0 except when current field type id is FT_BIT
//
void FieldDef::FieldValue::DoSet(const void* data, FieldTypeId inTid)
{
	bool dummy, isbit = false;

	FieldTypeId tid = fldDef->tid;
	void* dest;

	if(tid == FT_BIT) {
		isbit = true;
		tid = FT_BOOL;
		dest = &dummy;
	}

	else {
		dest = const_cast<void*>(DoGet());

		if(tid == FT_STRING && IsNull())
			new (dest) String();
	}

	SetData0(dest, tid, data, inTid);

	// special treatment for field of type FT_BIT

	if(isbit)
		lz::SetBit(const_cast<void*>(base), fldDef->start, dummy);

	MarkAsSet();
}

String& operator<<(String& lhs, const FieldDef::FieldValue& rhs)
{
	return lhs << rhs.Get(String().Cat());
}

void FieldDef::FieldValue::dumpflags()
{
#ifdef _DEBUG
//		String s;
//		s<<"BASE: "<<(void*)rset->GetRowBase(row)<<" CIndex: "<<col<<"SET FLAG: ";
//		for(uint16 i=0; i<rset->GetRecordDef().GetFieldCount(); ++i)
//		{
//			s<< (((*(int*)rset->GetRowBase(row)) & (1<<i))?'1':'0');
//		}
//		LOG(s);
#endif
}

RecordSet& RecordSet::AppendRecord()
{
	int rec_size = recordDef.GetRecordSize();
	//	RLOG("In AppendRecord()");
	//	RDUMP(recordDef.GetRecordSize());

	if(recordCount >= capacity) {
		int new_cap = capacity * 2;

		if(new_cap < 10)
			new_cap = 10;

		//		RLOG("1");
		char* p = new char[new_cap * rec_size];
//		RLOG("Newly alocated void*data");
//		RDUMP((void*)p);

//		RDUMP(new_cap*rec_size);
#ifdef _DEBUG
		memset(p, 0, new_cap * rec_size);

#endif
		//		RLOG("2");
		//
		memcpy(p, data, capacity * rec_size);

		//		RLOG("3");
		delete[] data;
		//		RLOG("4");
		data = p;

		capacity = new_cap;
	}

	//	RLOG("5");
	// initialize newly appended record
	memset(data + recordCount * rec_size, 0, rec_size);

	curRecord = recordCount++;

	return *this;
}

RecordSet& RecordSet::Erase(int i)
{
	FieldValue fv(nullptr, nullptr);

	for(int j = 0; j < GetFieldCount(); ++j) {
		fv.fldDef = &GetRecordDef().GetFieldDef(j);

		switch(fv.fldDef->tid) {

		default:
			continue;

		case FT_STRING:

		case FT_BLOB:
			fv.base = const_cast<char*>(GetRowBase(i));
			fv.SetNull();
		}

		//			{
		//
		//				for(auto r=Begin(); !(r==End()); ++r)
		//				{
		//					fv.base=r.base;
		//					fv.SetNull();
		//				}
		//				break;
		//			}
		//
		//		}
	}

	size_t recordSize = recordDef.GetRecordSize();

	memmove(const_cast<char*>(GetRowBase(i)), GetRowBase(i + 1),
	        (recordCount - i - 1) * recordSize);
	memset(const_cast<char*>(GetRowBase(recordCount - 1)), 0, recordSize);
	--recordCount;

	if(curRecord >= recordCount && curRecord != 0)
		--curRecord;

	return *this;
}

RecordSet& RecordSet::Erase(int i, int j)
{
	if(i < 0)
		i = 0;

	if(j >= (int)recordCount)
		j = (int)recordCount - 1;

	if(j < i)
		return *this;

	FieldValue fv(nullptr, nullptr);

	for(int k = 0; k < GetFieldCount(); ++k) {
		fv.fldDef = &GetRecordDef().GetFieldDef(k);

		switch(fv.fldDef->tid) {

		default:
			continue;

		case FT_STRING:

		case FT_BLOB:

			for(int r = i; r <= j; ++r) {
				fv.base = const_cast<char*>(GetRowBase(r));
				fv.SetNull();
			}
		}
	}

	size_t recordSize = recordDef.GetRecordSize();

	int cnt = j - i + 1;

	if(j != recordCount - 1)
		memmove(const_cast<char*>(GetRowBase(i)), GetRowBase(j + 1),
		        (recordCount - j - 1) * recordSize);

	memset(const_cast<char*>(GetRowBase(recordCount - cnt)), 0, recordSize * cnt);

	recordCount -= cnt;

	if(curRecord >= recordCount && curRecord != 0)
		curRecord = recordCount - 1;

	return *this;
}

RecordSet& RecordSet::Erase(const Vector<int>& v)
{
	if(v.IsEmpty())
		return *this;

	// Sort(v.Begin(), v.End());

	FieldValue fv(nullptr, nullptr);

	for(int k = 0; k < GetFieldCount(); ++k) {
		fv.fldDef = &GetRecordDef().GetFieldDef(k);

		switch(fv.fldDef->tid) {

		default:
			continue;

		case FT_STRING:

		case FT_BLOB:

			for(int i = 0; i < v.GetCount(); ++i) {
				fv.base = const_cast<char*>(GetRowBase(v[i]));
				fv.SetNull();
			}
		}
	}

	size_t recordSize = recordDef.GetRecordSize();

	int i = v[0];
	char* dest = const_cast<char*>(GetRowBase(i));
	// const char * src;
	++i;

	for(int r = 1; r < v.GetCount(); ++r) {
		ASSERT(i <= v[r] && v[r] < (int)recordCount);

		if(i != v[r]) {
			int cnt = int(GetRowBase(v[r]) - GetRowBase(i));
			memmove(dest, GetRowBase(i), cnt);
			dest += cnt;
			i = v[r];
		}

		++i;
	}

	if(i < (int)recordCount) {
		int cnt = int((recordCount - i + 1) * recordSize);
		memmove(dest, GetRowBase(i), cnt);
		dest += cnt;
	}

	memset(dest, 0, const_cast<char*>(GetRowBase(recordCount)) - dest);

	recordCount -= v.GetCount();

	if(curRecord >= recordCount && curRecord != 0)
		curRecord = recordCount - 1;

	return *this;
}

RecordSet& RecordSet::EraseAll()
{
	Erase(0, int(recordCount - 1));
	return *this;
}

RecordSet& RecordSet::Reserve(size_t count)
{
	throw "Not yet implemented";
	return *this;
	//	size_t rec_size=recordDef.GetRecordSize();
	//	if(count>capacity)
	//	{
	//		//size_t new_cap=static_cast<size_t>(capacity*2);
	//		//if(new_cap<10)
	//		//	new_cap=10;
	//		char * p=new char[count*rec_size];
	// #ifdef _DEBUG
	//		memset(p, 0, new_cap*rec_size);
	// #endif
	//
	//		memcpy(p, data, capacity*rec_size);
	//		delete [] data;
	//		data=p;
	//		capacity=new_cap;
	//	}
	//	// initialize newly appended record
	//	memset(data+recordCount*rec_size,0, rec_size);
	//
	//	curRecord=recordCount++;
}

void RecordSet::Clear()
{
	this->~RecordSet();
	new (this) RecordSet();
}

RecordSet::~RecordSet()
{
	FieldValue fv(NULL, NULL);

	for(int i = 0; i < GetFieldCount(); ++i) {
		fv.fldDef = &GetRecordDef().GetFieldDef(i);

		switch(fv.fldDef->tid) {

		default:
			continue;

		case FT_STRING:

		case FT_BLOB: {

			for(auto r = Begin(); r != End(); ++r) {
				fv.base = r.base;
				fv.SetNull();
			}

			break;
		}
		}
	}

	delete[] data;
}

RecordSet& RecordSet::SetRecordDef(const RecordDef& rdf)
{
	if(this->RecordCount() != 0) {
		LOG("RecordSet::SetRecordDef()-Modifying the RecordDef for a populated RecordSet "
		    "ignored!");
	}

	else {
		recordDef = rdf;
	}

	return *this;
}

RecordSet& RecordSet::SetRecordDef(RecordDef&& rdf)
{
	if(this->RecordCount() != 0) {
		LOG("RecordSet::SetRecordDef()-Modifying the RecordDef for a populated RecordSet "
		    "ignored!");
	}

	else {
		// LOG("RecordSet::SetRecordDef(RecordDef&&)");
		recordDef = std::move(rdf);
	}

	return *this;
}

RecordSet& RecordSet::AppendRecordDef(const RecordDef& rdf)
{
	if(this->RecordCount() != 0) {
		LOG("RecordSet::AppendRecordDef()-Modifying the RecordDef for a populated RecordSet "
		    "ignored!");
	}

	else {
		recordDef.AppendRecordDef(rdf);
	}

	return *this;
}

void RecordSet::DoSwapRecord(RecordSet::Iterator i, RecordSet::Iterator j)
{
	ASSERT(i.rdef == j.rdef && i.rdef != NULL);
	char* p = (char*)i.base;
	char* q = (char*)j.base;

	for(; p < (char*)i.base + i.rdef->GetRecordSize(); ++p, ++q)
		Swap(*p, *q);
}

// void RecordSet::Sort(Vector<int> fields)
//{
//	//Upp::Sort
// }
//

END_NAMESPACE


