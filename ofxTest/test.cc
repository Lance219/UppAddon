#include "test.h"

namespace lz{

TEST(ParserTest, TagsAndKeys)
{
	lz::Parser _;
	for(auto k : _.keys)
		EXPECT_TRUE(_.IndexOfKey(k)!=-1);
	for(auto t : _.tags)
		EXPECT_TRUE(_.IndexOfTag(t)!=-1);
	EXPECT_EQ(_.IndexOfKey("OFX"), -1);
	EXPECT_EQ(_.IndexOfTag("OFXHEADER"), -1);
}

TEST(ParserTest, ReadKeyValue)
{
	lz::Parser _;
	char buff[]=R"(OFXHEADER:100
DATA:OFXSGML
VERSION:102
SECURITY:NONE
ENCODING:USASCII
CHARSET:1252
COMPRESSION:NONE
OLDFILEUID:NONE
NEWFILEUID:NONE)";
	std::string value;
	BuffInput in(buff, sizeof(buff)-1);
	_.ReadKeyValue(in, _.IndexOfKey("OFXHEADER"), value);
	EXPECT_EQ(value, "100");
	_.ReadKeyValue(in, _.IndexOfKey("DATA"), value);
	EXPECT_EQ(value, "OFXSGML");
	_.ReadKeyValue(in, _.IndexOfKey("VERSION"), value);
	EXPECT_EQ(value, "102");
	_.ReadKeyValue(in, _.IndexOfKey("SECURITY"), value);
	EXPECT_EQ(value, "NONE");
	_.ReadKeyValue(in, _.IndexOfKey("ENCODING"), value);
	EXPECT_EQ(value, "USASCII");
	_.ReadKeyValue(in, _.IndexOfKey("CHARSET"), value);
	EXPECT_EQ(value, "1252");
	_.ReadKeyValue(in, _.IndexOfKey("COMPRESSION"), value);
	EXPECT_EQ(value, "NONE");
	_.ReadKeyValue(in, _.IndexOfKey("OLDFILEUID"), value);
	EXPECT_EQ(value, "NONE");
	_.ReadKeyValue(in, _.IndexOfKey("NEWFILEUID"), value);
	EXPECT_EQ(value, "NONE");
	EXPECT_TRUE(in.peek()==EOF);
}

TEST(ParserTest, ReadHeader)
{
	lz::Parser _;
	char buff[]=R"(OFXHEADER:100
DATA:OFXSGML
VERSION:102
SECURITY:NONE
ENCODING:USASCII
CHARSET:1252
COMPRESSION:NONE
OLDFILEUID:NONE
NEWFILEUID:NONE)";
	std::string value;
	BuffInput in(buff, sizeof(buff)-1);
	DataSet ds;
	EXPECT_TRUE(_.ReadHeader(in, ds));
	EXPECT_TRUE(ds.Contains("header"));
	auto& hdr = ds["header"];
	EXPECT_EQ(hdr["OFXHEADER"].Get<Upp::String>(), "100");
	EXPECT_EQ(hdr["DATA"].Get<Upp::String>(), "OFXSGML");
	EXPECT_EQ(hdr["VERSION"].Get<Upp::String>(), "102");
	EXPECT_EQ(hdr["SECURITY"].Get<Upp::String>(),"NONE");
	EXPECT_EQ(hdr["ENCODING"].Get<Upp::String>(),"USASCII");
	EXPECT_EQ(hdr["CHARSET"].Get<Upp::String>(),"1252");
	EXPECT_EQ(hdr["COMPRESSION"].Get<Upp::String>(),"NONE");
	EXPECT_EQ(hdr["OLDFILEUID"].Get<Upp::String>(),"NONE");
	EXPECT_EQ(hdr["NEWFILEUID"].Get<Upp::String>(),"NONE");
}



}