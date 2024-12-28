#include "../ofx.h"

namespace lz{

bool Parser::Parse(Parser::Input& in)
{
	DataSet ds;
	if(!ReadHeader(in, ds))
	{
		auto& error = ds.Add("errors");
		error.AddField("msg", FT_STRING).PhysicalLayout();
		error.Append();
		String s;
		s<<"Error reading ofx header:...";
		int c;
		int count=10;
		while( --count!=0 && (c=in.peek())!=EOF )
		{
			s << (char)c;
		}
		error(0)=s;
		return false;
	}
	
	
	
	return true;
}
//
//enum {SPACE, NL, ALPHA, NUM, LESS, COLON, OTHER};
//static auto chargrp=[](int ch)
//{
//	switch(ch)
//	{
//	case ' ':
//	case '\t':
//		return SPACE;
//	case '\n':
//		return NL;
//	case '<':
//		return LESS;
//	case ':':
//		return COLON;
//	default:
//		if(isalpha(ch))
//			return ALPHA;
//		if(isdigit(ch))
//			return NUM;
//		
//	}
//	return OTHER;
//};
// eg: OFXHEADER:100...
// will consume upto ':' inclusive
// 
// state  on			to
//    0   space			0
//    0		\n			0
//    0   ALPHA			1
//    0    <			4
//    1   ALPHA			1
//    1   NUM			1
//    1   space			2
//    1		:			3
//    2   space         2
//	  2		:			3
//
//    5 is error state, 3,4,5 are accepting states
//Parser::Token Parser::HeaderReadKeyValue(Input& in, std::string& value)
//{
//	/* known Keys and the sequence they appears
//		OFXHEADER,	DATA, VERSION, SECURITY, ENCODING, CHARSET,
//		COMPRESSION, OLDFILEUID, NEWFILEUID
//	*/
//	static char table[][7]={
//		//SPACE	NL	ALPHA,	NUM, LESS, COLON, OTHER
//		// state 0
//		{	0,	0,	1,		5,		4,		5,	5},
//		// state 1
//		{	2,	5,	1,		1,		5,		3,	5},
//		// state 2
//		{	2,	5,	5,		5,		5,		3,	5}
//	};
//	int c;
//	Parser::Token t;
//	t.type = TokenType::Error;
//	int state = 0;
//	bool done = false;
//	std::string text;
//	while( !done && (c=in.peek())!=EOF)
//	{
//		switch(c)
//		{
//		case ':':
//			if(state != 0)
//			{
//				int i = IndexOfKey(text.c_str());
//				if(i!=-1)
//				{
//					t.type = TokenType::HeaderKey;
//					t.id = i;
//				}
//			}
//		case '\n':
//			in.advance();
//			[[fallthrough]];
//		case '<':
//			done = true;
//			continue;
//		}
//		if( c=='<')
//			break;
//		else if(c=='\n')
//		{
//			in.advance();
//			break;
//		}
//		
//		switch(state)
//		{
//		case 0:
//			//state0();
//			break;
//		case 1:
//			//state1();
//			break;
//		case 2:
//		}
//		switch(c)
//		{
//		case ' ':
//		case '\t':
//			continue;
//		case '<':
//			
//		}
//	}
//	return t;
//}

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
bool Parser::ReadHeader(Input& in, DataSet& ds)
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
	ds.Add(std::move(hdr),"header");
	return true;
}


int get_id(Parser::Input& in)
{
	return -1;
}


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


//int Parse::Input::peek()const
//{
//	if(nullptr == fp)
//	{
//		return ptr==end ? EOF : *ptr;
//	}else{
//		char c = getc(fp);
//		return ungetc(c, fp);
//	}
//}
//
//int Parse::Input::read()
//{
//	if(nullptr == fp)
//	{
//		int c = ptr==end ? EOF : *ptr;
//		++ptr;
//		return c;
//	}else{
//		return getc(fp);
//	}
//}
//		int advance();
//	private:
//		FILE * fp;
//		char *ptr, *beg, *end;
//	};

	
}