#include "../ofx.h"
#include <cassert>

namespace lz{

bool Parser::Parse(Parser::Input& in, Parser::Info& info)
{
	if(!ReadHeader(in, info))
	{
		std::string s;

		int c;
		int count=10;
		while( --count!=0 && (c=in.read())!=EOF )
		{
			s += (char)c;
		}
		info.Error("Error reading ofx header:...'{}'", s);
		return false;
	}
	if(info.ds.Contains("header"))
		return ParseSGML(in, info);
	
	return false;
}

// attempt to read pass the expected tag 'tag', if input has
// something else, the Input 'in' will be significantly restored to
// the state before the failed read.
//
// for example, if input 
// '\n  <OFX>...'
// is passed to this function with 'tag'="STMTTRN"
// false will be returned, input will be
// '<OFX>...'
// i.e. the skipped whitespace is not restored, otherwise the input
// can be used to test for another tag, etc.
//
bool Parser::ReadTag(Input& in, Info& info, const char * tag)
{
	assert(IndexOfTag(tag)!=-1);
	if(in.skip_spaces()!='<')
	{
		return false;
	}
	auto pos = in.tellg();
	in.advance(); // read pass '<'
	int i=0, c;
	auto taglen = std::string_view(tag).length();
	while( i<taglen && (c=in.peek())==tag[i])
	{
		++i;
		in.advance();
	}
	if(i==taglen && in.read() == '>')
		return true;
	
	in.seekg(pos);
	return false;
}

// return -1 on invalid tag
//        [0, tag_count), an open tag with return value as the index
//        other negative value 'r', an close tag with '-r+2' as the index
int  Parser::ReadTag(Input& in, Info& info)
{
	std::string tag;
	bool close = false;
	
	if(in.skip_spaces()!='<')
	{
		return -1;
	}
	in.advance(); // read pass '<'
	if(in.peek()=='/'){
		close = true;
		in.advance();
	}
	
	int c;
	while( (c=in.read())!=EOF && c!='>' && !isspace(c) )
	{
		tag += (char)c;
	}
	if(c!='>')
		return -1;
	
	int index = IndexOfTag(tag.c_str());
	return index == -1 ? -1 : close ? -index-2 : index;
}

// for now, we skip this segment. 
// DTSERVER, FID may be interesting to have
//
bool Parser::ReadSignOnMsgSRSV1(Input& in, Info& info)
{
	return SkipToTag(in, "SIGNONMSGSRSV1", true);
}

std::string Parser::ReadValue(Input& in, bool oneline)
{
	std::string s;
	int c;
	while( (c=in.peek())!=EOF && c!='<' )
	{
		in.advance();
		if(oneline && c=='\n')
			break;
		s += (char)c;
	}
	return s;
}

// we only take information that we are interested in
//
bool Parser::ReadMsgSRSV1(Input& in, Info& info, const char * tag)
{
	int c;
	Enter _(false);
	if( !SkipToTag(in, "CURDEF") )
	{
		info.Error("Expecting <{}>", "CURDEF");
		return false;
	}
	_.Log("1");
	auto& r = info.stmt_hdr().Append();
	r("CURDEF")=ReadOneLineValue(in).c_str();
	_.Log("2");
	if( !SkipToTag(in, "ACCTID") )
	{
		info.Error("Expecting <{}>", "ACCTID");
		return false;
	}
	
	r("ACCTID")=ReadOneLineValue(in).c_str();
	_.Log("------");
	_.Log("{}", (const char*)r("ACCTID").Get<String>());
	_.Log("3");
	if( !ReadBANKTRANLIST(in, info) )
	{
		info.Error("Error while parsing <{0}>...</{0}>.", "BANKTRANLIST");
		return false;
	}
	_.Log("4");
	if(ReadTag(in, info, "LEDGERBAL") && ReadTag(in, info, "BALAMT") )
	{
		r("LEDGERBALAMT") = ReadOneLineValue(in).c_str();
		if( ReadTag(in, info, "DTASOF") )
		{
			r("LEDGERBALDTASOF") = ReadTimeValue(in);
		}
		
	}
	_.Log("5");
	if(ReadTag(in, info, "AVAILBAL") && ReadTag(in, info, "BALAMT") )
	{
		r("AVAILBALAMT") = ReadOneLineValue(in).c_str();
		if( ReadTag(in, info, "DTASOF") )
		{
			r("AVAILDTASOF") = ReadTimeValue(in);
		}
	}
	// DO WE WANT TO VALIDATE THE INPUT?
	_.Log("6");
	
	return SkipToTag(in, tag, true);
}

// actually, skip input until the desired tag is read or EOF
//
bool Parser::SkipToTag(Input& in, const char * tag, bool close)
{
	int c;
	assert(IndexOfTag(tag)!=-1);
	while( (c=in.read())!=EOF)
	{
		if(c=='<')
		{
			if(close && (c=in.read()) != '/')
			{
				continue;
			}
			int i = 0;
			while( i<std::string_view(tag).length() && (c=in.read())==tag[i] )
			{
				++i;
			}
			if(i==std::string_view(tag).length() && in.read()=='>' )
				return true;
		}
	}
	return false;
}

// it seems we don't need tag_stack with this way of parsing
// we will skip this one. Note we skip to the open tag <BANKTRANLIST>
//
bool Parser::ReadBANKTRANLIST(Input& in, Info& info)
{
	Enter _(false);
	if(! SkipToTag(in, "BANKTRANLIST"))
	{
		info.Error("Expecting <{}>","BANKTRANLIST");
	}
	_.Log("1");
	if(SkipToTag(in, "STMTTRN"))
	{
		int tag;
		do{
			_.Log("2");
			if(!ReadSTMTTRN(in, info))
				return false;
		}while( (tag=ReadTag(in, info))== IndexOfTag("STMTTRN"));
		return tag == IndexOfCloseTag("BANKTRANLIST");
	}
	return false;
}

// we just past the open tag <STMTTRN>
// 
bool Parser::ReadSTMTTRN(Input& in, Info& info)
{
	int tag;
	Enter _(false);;
	auto& r = info.stmt_trans().Append();
	_.Dump(IndexOfCloseTag("STMTTRN"));
	while( (tag = ReadTag(in, info))!=-1)
	{
		if(tag>=0)
			_.Log("{}", tags[tag]);
		else
			_.Log("{}close {}", tag, tags[-tag-2]);
		
		switch(tag)
		{
		case IndexOfTag("TRNTYPE"):
		case IndexOfTag("DTPOSTED"):
		case IndexOfTag("TRNAMT"):
		case IndexOfTag("FITID"):
		case IndexOfTag("NAME"):
		case IndexOfTag("MEMO"):
			r(tags[tag])=ReadOneLineValue(in).c_str();
			break;
		case IndexOfCloseTag("STMTTRN"):
			return true;
		}
	}
	_.Log("ReadSTMTTRN false");
	return false;
}


bool Parser::ParseSGML(Input& in, Info& info)
{
	int c;
	Enter _(true);
	
	if(!ReadTag(in, info, "OFX")){
		info.Error("Expecting openning tag <OFX>");
		return false;
	}
	info.tag_stack.push_back(IndexOfTag("OFX"));
	
	int tag;
	while( (tag = ReadTag(in, info))!=-1)
	{
		switch(tag)
		{
		case IndexOfTag("SIGNONMSGSRSV1"):
			info.tag_stack.push_back(IndexOfTag("SIGNONMSGSRSV1"));
			if(!ReadSignOnMsgSRSV1(in, info))
			{
				info.Error("Error parsing <{}>.", tags[IndexOfTag("SIGNONMSGSRSV1")]);
				return false;
			}
			break;
		case IndexOfTag("CREDITCARDMSGSRSV1"):
		case IndexOfTag("BANKMSGSRSV1"):
//			LOG("ParseSGML - xxx MSGSRSV1");
			_.Log("ParseSGML - xxx MSGSRSV1");
			//info.tag_stack.push_back(tag);
			if(!ReadMsgSRSV1(in, info,tags[tag]))
			{
				info.Error("Error parsing <{}.>", tags[tag]);
				return false;
			}
			_.Log("ParseSGML - xxx MSGSRSV1 OK");
//			LOG("ParseSGML - xxx MSGSRSV1 OK");
			break;
//			info.tag_stack.push_back(IndexOfTag("BANKMSGSRSV1"));
//			if(!ReadBANKMSGSRSV1(in, info))
//			{
//				info.Error("Error parsing <{}.>", tags[IndexOfTag("BANKMSGSRSV1")]);
//				return false;
//			}
//			break;
		case IndexOfCloseTag("OFX"):
			return true;
		}
	}
	return false; // we ignore contents after </ofx> if any
}

// return true on successfully reading the key:value pair
//
// this func doesn't know and care if value is empty or valid
//
bool Parser::ReadKeyValue(Input& in, int key_index, std::string& value)
{
	unsigned i = 0;
	int c;
	std::string_view key(keys[key_index]);
	while(i<key.length() && (c = in.peek()) == key[i]){
		++i; in.advance();
	}
	
	if( i==key.length() && in.peek()==':') // successfully read key,
	{
		in.advance();
		value.clear();
		while( (c=in.read()) != EOF && c!='\n' )
			value += (char)c;
		return true;
	}
	return false;
}

// return true if no errors are encountered, including no header at all
//
// return false on error
//
bool Parser::ReadHeader(Input& in, Info& info)
{
	unsigned i=0;
	char c;
	
	// we first check if the input is headerless, in which case
	// the first nonblank character is a '<'
	while( (c=in.peek())!=EOF && isspace(c) )
	{
		++i;
		in.advance();
	}
	
	if(c=='<')
		return true;
	if(i!=0) // we expect the input starts with OFXHEADER:, not space
		return false;

	std::string value;
	// actually reading header
	int indexes[]={IndexOfKey("OFXHEADER"), IndexOfKey("DATA"),
		IndexOfKey("VERSION"), IndexOfKey("SECURITY"), IndexOfKey("ENCODING"),
		IndexOfKey("CHARSET"),IndexOfKey("COMPRESSION"),IndexOfKey("OLDFILEUID")
		,IndexOfKey("NEWFILEUID")
	};
	
	RecordSet hdr;
	for(int i = 0; i<sizeof indexes/sizeof indexes[0]; ++i)
		hdr.AddField(keys[indexes[i]], FT_STRING);
	hdr.PhysicalLayout().Append();
	
	for(int i = 0; i<sizeof indexes/sizeof indexes[0]; ++i){
		if(!ReadKeyValue(in, indexes[i], value))
			return false;
		hdr(i).Set(value.c_str());
	}
	info.ds.Add(std::move(hdr),"header");
	return true;
}

//
//int get_id(Parser::Input& in)
//{
//	return -1;
//}


FileInput::FileInput(const char * file)
{
	fp = fopen(file, "r");
	if( fp == nullptr ){
		char buff[100];
		snprintf(buff, sizeof(buff), "Unable to open file '%s': %s\n",
			file, strerror(errno));
		throw std::runtime_error(buff);
	}
}
	
}
