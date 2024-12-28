#include "../ofx.h"

namespace lz{

bool Parser::Parse(Parser::Input& in)
{
	Info info;
	if(!ReadHeader(in, info))
	{
		std::string s("Error reading ofx header:...");

		int c;
		int count=10;
		while( --count!=0 && (c=in.peek())!=EOF )
		{
			s += (char)c;
		}
		info.Error(s.c_str());
		return false;
	}
	if(info.ds.Contains("header"))
		return ParseSGML(in, info);
	
	return true;
}

bool Parser::ParseSGML(Input& in, Info& info)
{
	std::vector<int> tags;
	int c;
	
	if(in.skip_spaces()!='<')
	{
		info.Error("parsing input failure: expecting <ofx> tag");
		return false;
	}
	in.advance(); // read pass '<'
	
	return true;
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