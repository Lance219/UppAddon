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
	char buff[]=
R"(OFXHEADER:100
DATA:OFXSGML
VERSION:102
SECURITY:NONE
ENCODING:USASCII
CHARSET:1252
COMPRESSION:NONE
OLDFILEUID:NONE
NEWFILEUID:NONE)";

	lz::Parser _;
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
	char buff[]=
R"(OFXHEADER:100
DATA:OFXSGML
VERSION:102
SECURITY:NONE
ENCODING:USASCII
CHARSET:1252
COMPRESSION:NONE
OLDFILEUID:NONE
NEWFILEUID:NONE
)";

	lz::Parser _;
	std::string value;
	BuffInput in(buff, sizeof(buff)-1);
	Parser::Info info;
	//DataSet ds;
	EXPECT_TRUE(_.ReadHeader(in, info));
	EXPECT_TRUE(info.ds.Contains("header"));
	auto& hdr = info.ds["header"];
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

TEST(ParserTest, ReadTag) // also tested is SkipToTag
{
	char buff[]="<OFX><SIGNONMSGSRSV1><SONRS><STATUS></OFX><GAME><CODE>0<SEVERITY>";

	Parser _;
	BuffInput in(buff, sizeof(buff)-1);
	Parser::Info info;
	
	EXPECT_FALSE(_.ReadTag(in,info, _.IndexOfTag("SIGNONMSGSRSV1")));
	EXPECT_TRUE(_.ReadTag(in,info, _.IndexOfTag("OFX")));
	EXPECT_FALSE(_.ReadTag(in,info, _.IndexOfTag("OFX")));
	EXPECT_TRUE(_.ReadTag(in,info, _.IndexOfTag("SIGNONMSGSRSV1")));
	EXPECT_TRUE(_.ReadTag(in,info)==_.IndexOfTag("SONRS"));
	EXPECT_TRUE(_.ReadTag(in,info)==_.IndexOfTag("STATUS"));
	EXPECT_TRUE(_.ReadTag(in,info)==-_.IndexOfTag("OFX")-2); // close tag
	EXPECT_TRUE(_.ReadTag(in,info)== -1); // invalid tag
	EXPECT_TRUE(_.SkipToTag(in,"SEVERITY"));
}

TEST(ParserTest,ReadSignOnMsgSRSV1)
{
	char buff[]="<DTACCTUP>20241012065958<INTU.BID>00005</SONRS></SIGNONMSGSRSV1>";
	
	Parser _;
	BuffInput in(buff, sizeof(buff)-1);
	Parser::Info info;
	info.tag_stack.push_back(0);
	EXPECT_TRUE(_.ReadSignOnMsgSRSV1(in,info));
}


TEST(ParserTest, ReadSTMTTRN)
{
	char buff[]=
R"(
<TRNTYPE>CREDIT
<DTPOSTED>20241101000000.000[-5:EDT]
<TRNAMT>26.48
<FITID>5000230212850011202411012040245
<NAME>PAYMENT RECEIVED - THANK YOU
</STMTTRN>
)";
	Enter e(false);
	Parser _;
	BuffInput in(buff, sizeof(buff)-1);
	Parser::Info info;
	e.Log("-=-=-=-=-=-=");
	EXPECT_TRUE(_.ReadSTMTTRN(in, info));
	e.Log("-=-=-=-==-=****");
	auto& r = info.stmt_trans();
	EXPECT_TRUE(r.RecordCount() == 1);
	EXPECT_TRUE(r("TRNTYPE").Get<String>()=="CREDIT");
	EXPECT_EQ(r("DTPOSTED").Get<Time>(), Time(2024,11,1,0,0,0));
	EXPECT_TRUE(r("TRNAMT").Get<double>()==26.48);
	EXPECT_TRUE(r("FITID").Get<String>()=="5000230212850011202411012040245");
	EXPECT_TRUE(r("NAME").Get<String>()=="PAYMENT RECEIVED - THANK YOU");
}

TEST(ParserTest, Parse)
{
	char buff[]=
R"(OFXHEADER:100
DATA:OFXSGML
VERSION:102
SECURITY:NONE
ENCODING:USASCII
CHARSET:1252
COMPRESSION:NONE
OLDFILEUID:NONE
NEWFILEUID:NONE

<OFX><SIGNONMSGSRSV1><SONRS><STATUS><CODE>0<SEVERITY>INFO<MESSAGE>OK</STATUS><DTSERVER>20241012065958<LANGUAGE>ENG<DTPROFUP>20241012065958<DTACCTUP>20241012065958<INTU.BID>00005</SONRS></SIGNONMSGSRSV1>
<BANKMSGSRSV1><STMTTRNRS><TRNUID>20241012065958<STATUS><CODE>0<SEVERITY>INFO<MESSAGE>OK</STATUS>
<STMTRS><CURDEF>CAD<BANKACCTFROM><BANKID>600000100<ACCTID>00422 83-42032<ACCTTYPE>SAVINGS</BANKACCTFROM>
<BANKTRANLIST><DTSTART>20211206120000<DTEND>20241011120000
<STMTTRN><TRNTYPE>CREDIT<DTPOSTED>20241011120000.000[-5:EST]<TRNAMT>1000.00<FITID>24285285123204178266000002<MEMO>INTERNET TRANSFER 000000203995;Internet Banking</STMTTRN>
<STMTTRN><TRNTYPE>DEBIT<DTPOSTED>20241010120000.000[-5:EST]<TRNAMT>-120.74<FITID>24284284061323869625000001<MEMO>INTERNET TRANSFER 000000130077;Internet Banking</STMTTRN>
<STMTTRN><TRNTYPE>CREDIT<DTPOSTED>20241010120000.000[-5:EST]<TRNAMT>61.45<FITID>24284284040800698532940000<MEMO>DEPOSIT;CANADA;Electronic Funds Transfer</STMTTRN>
</BANKTRANLIST><LEDGERBAL><BALAMT>4000.99<DTASOF>20241012065958</LEDGERBAL>
<AVAILBAL><BALAMT>3837.24<DTASOF>20241012065958</AVAILBAL>
</STMTRS></STMTTRNRS></BANKMSGSRSV1></OFX>)";

	Parser _;
	BuffInput in(buff, sizeof(buff)-1);
	Parser::Info info;
	EXPECT_TRUE(_.Parse(in, info));
	EXPECT_EQ(info.stmt_hdr()("LEDGERBALAMT").Get<double>(), 4000.99);
	EXPECT_EQ(info.stmt_hdr()("LEDGERBALDTASOF").Get<Time>(),Time(2024,10,12,6,59,58));
}

}