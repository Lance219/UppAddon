#include <Core/Core.h>
#include <DataSet/DataSet.h>
#include <plugin/gtest/gtest.h>
#include "DataSetTest.h"

using namespace lz;
using namespace Upp;

void DataSetTest::SetUp()
{
	rs.AddField<char>("char") // 1
		.AddField<bool>("bool") // 2
		.AddField<bit>("bit") // 3
		.AddField<int>("int") //4
		.AddField<unsigned>("unsigned") //5
		.AddField<double>("double") //6
		.AddField<float>("float")//7
		.AddField<uint16>("uint16") //8
		.AddField<int32>("int32")//9
		.AddField<uint64>("uint64") //10
		.AddField<String>("String") //11
		.AddField<Date>("Date")
		.AddField<Time>("Time")
		.PhysicalLayout();
//	RDUMP(rs.GetRecordDef());
}

TEST_F(DataSetTest, ReadWrite)
{
	EXPECT_EQ(0, rs.RecordCount());
	rs.Append();
	EXPECT_EQ(1, rs.RecordCount());
	EXPECT_EQ(0, rs.RecordNo());
	rs["char"] = 'A';
	EXPECT_EQ(rs["char"].Get<char>(), 'A');
	rs["bool"].Get<bool>() = false;
	EXPECT_EQ(rs["bool"].Get<bool>(), false);
	rs["bool"].Get<bool>() = true;
	EXPECT_EQ(rs["bool"].Get<bool>(), true);
	rs["bit"].Set(false);
	EXPECT_EQ(rs["bit"].Get<bit>(), false);
	rs["bit"] = true;
	EXPECT_EQ(rs["bit"].Get<bit>(), true);
	rs["int"] = -13579;
	EXPECT_EQ(rs["int"].Get<int>(), -13579);
	rs["unsigned"] = 7654321u;
	EXPECT_EQ(rs["unsigned"].Get<unsigned>(), 7654321u);
	rs["double"] = 3.1415926;
	EXPECT_DOUBLE_EQ(rs["double"].Get<double>(), 3.1415926);
	rs["float"] = 1.1712f;
	EXPECT_FLOAT_EQ(rs["float"].Get<float>(), 1.1712f);
	rs["uint16"]=0xF37Eu;
	EXPECT_EQ(rs["uint16"].Get<unsigned short>(), 0xF37E);
	rs["int32"]=-31415629;
	EXPECT_EQ(rs["int32"].Get<int>(), -31415629);
	rs["uint64"]=0xFEEF'107F'3E48'77ED;
	EXPECT_EQ(rs["uint64"].Get<unsigned long long>(), 0xFEEF'107F'3E48'77ED);
	rs["uint64"]=183987365.4897;
	EXPECT_EQ(rs["uint64"].Get<unsigned long long>(), 183987365);
	{
		String s;
		s << "1qweroiquwelasdfnz,mxvcnz;sdjg[oaiwesav;ksadl;gkaopsdg[p';dfskasd'f";
		rs["String"] = s;
		EXPECT_EQ(rs["String"].Get<String>(), s);
	}
	{
		Time t = GetSysTime();
		Date d = t;
		rs["Date"] = d;
		EXPECT_EQ(rs["date"].Get<Date>(), d);
		rs["time"] = t;
		EXPECT_EQ(rs["time"].Get<Time>(), t);
	}
}

TEST_F(DataSetTest, SetNull)
{
	rs.Append();
	EXPECT_EQ(rs["char"].IsNull(), true);
	rs["char"] = 'A';
	EXPECT_EQ(rs["char"].IsNull(), false);
	rs["char"].SetNull();
	EXPECT_EQ(rs["char"].IsNull(), true);
}


TEST_F(DataSetTest, Sort)
{
	rs.Append();
	rs["char"]='C'; // 1st field
	rs["unsigned"]=700; // 5th field
	rs["string"]="Hell0"; // 11th field
	rs.Append();
	rs["char"]='B';
	rs["unsigned"]=1700;
	rs["string"]="World";
	rs.Append();
	rs["char"]='F';
	rs["unsigned"]=900;
	rs["string"]="DataSet";
	rs.Append();
	rs["char"]='Z';
	rs["unsigned"]=30;
	rs["string"]="Sorting";
	rs.Sort({1,-5,11});
	EXPECT_EQ(rs[0]["char"].Get<char>(),'B');
	
}

bool compare_record(const RecordSet::Record& lhs, const RecordSet::Record& rhs)
{
	const auto l=lhs["name"];
	const auto r=rhs["name"];
	if(l.IsNull() && r.IsNull())
		return (&l)<(&r);
	else if(l.IsNull())
		return true;
	else if(r.IsNull())
		return false;
	return l.Get<String>() > r.Get<String>();
	
}

void Dump(RecordSet& rset)
{
	int n=0;
	for(auto i=rset.Begin(); i!=rset.End(); ++i)
	{
		++n;
		RecordSet::Record& r=*i;
		if( !r["name"].IsNull())
		{
			
			String s;
			s << n << ':' << r["name"].Get<String>() << ","
			 << r["score"].Get<int16>()<<","
			 << r["sex"].Get<bit>()<<","
			 << r["uid"].Get<int64>()<<","
			 << r["address"].Get<String>()<<","
			 << r["happy"].Get<bool>()<<","
			 << r["birthday"].Get<Time>();
			LOG(s);
		}
	}
}
