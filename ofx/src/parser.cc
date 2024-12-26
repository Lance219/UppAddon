#include "../ofx.h"

namespace lz{

void Parser::Parse(Parser::Input& in)
{
	DataSet ds;
	auto& hdr = ds.Add("header");
	hdr.AddField("OFXHEADER", )
	while(in.peek()!=EOF)
	{
		in.advance();
	}
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