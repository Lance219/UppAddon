// internal header
class Parser {
public:
	class Input {
	public:
		int skip_spaces(){
			int c;
			while( isspace( (c=peek())))
				advance();
			return c;
		}
		
		int skip_blanks(){
			int c;
			while( isblank(c=peek()))
				advance();
			return c;
		}
		virtual int peek() const = 0;
		virtual int read() = 0;
		virtual void advance() = 0;
		virtual long tellg()const=0;
		virtual void seekg(long pos)=0;
		//virtual void unget(int ch)=0;
		virtual ~Input(){}
	};

	// not a very descriptive name, but for now
	struct Info{
		DataSet ds;
		std::vector<int> tag_stack;

		template <typename... Args>
		void Error(std::format_string<Args...> fmt, Args&&...args);
		
		bool HasError()const{
			return ds.Contains("errors") && ds["errors"].RecordCount()!=0;
		}
		
		auto& error_table()
		{
			if(!ds.Contains("errors")){
				auto& e = ds.Add("errors");
				e.AddField("msg", FT_STRING).PhysicalLayout();
				return e;
			}
			return	ds["errors"];
		}
		
		auto& stmt_hdr()
		{
			if(!ds.Contains("stmt_hdr")){
				auto & m = ds.Add("stmt_hdr");
				m.AddField("CURDEF", FT_STRING)
					.AddField("ACCTID", FT_STRING)
					.AddField("LEDGERBALAMT", FT_DOUBLE)
					.AddField("LEDGERBALDTASOF", FT_TIME)
					.AddField("AVAILBALAMT", FT_DOUBLE)
					.AddField("AVAILBALDTASOF", FT_TIME)
					.PhysicalLayout();
				return m;
			}
			return ds["stmt_hdr"];
		}
		auto& stmt_trans()
		{
			if(!ds.Contains("stmt_trans")){
				auto & m = ds.Add("stmt_trans");
				m.AddField("TRNTYPE", FT_STRING)
					.AddField("DTPOSTED", FT_TIME)
					.AddField("TRNAMT", FT_DOUBLE)
					.AddField("FITID", FT_STRING)
					.AddField("NAME", FT_STRING)
					.AddField("MEMO", FT_STRING)
					.PhysicalLayout();
				return m;
			}
			return ds["stmt_trans"];
		}
	};
public:
	bool Parse(Input& in, Info& info);
	bool Parse(const char * file, Info& info);
	
	// note if passed in 'fp' is not stdin, it will be
	// closed after the call
	//
	bool Parse(FILE * fp, Info& info);


private:
//	Token HeaderReadKeyValue(Input& in, std::string& value);
	bool ReadHeader(Input& in, Info& info);
	bool ReadKeyValue(Input& in, int key_index, std::string& value);
	bool ParseSGML(Input& in, Info& info);
	std::string ReadValue(Input& in, bool oneline=false);
	//Time	ReadTimeValue(Input& in);
	std::string ReadOneLineValue(Input& in){
		return ReadValue(in, true);
	}
	bool ReadTag(Input& in, Info& info, int tag_index)
	{
		assert( tag_index>=0 && tag_index < TagsCount());
		return ReadTag(in, info, tags[tag_index]);
	}
	bool ReadTag(Input& in, Info& info, const char * tag);

	int  ReadTag(Input& in, Info& info);
	bool ReadSignOnMsgSRSV1(Input& in, Info& info);
	bool ReadMsgSRSV1(Input& in, Info& info, const char * tag);
//	bool ReadBANKMSGSRSV1(Input& in, Info& info);
	bool ReadBANKTRANLIST(Input& in, Info& info);
	bool SkipToTag(Input& in, const char * tag, bool close=false);
	bool ReadSTMTTRN(Input& in, Info& info);
	
	constexpr static const char * keys[]={// in ascending order
		"CHARSET",
		"COMPRESSION",
		"DATA",
		"ENCODING",
		"NEWFILEUID",
		"OFXHEADER",
		"OLDFILEUID",
		"SECURITY",
		"VERSION"
	};
	constexpr static const char * tags[]={
#include "tags.inc"
	};
	
	FRIEND_TEST(ParserTest, TagsAndKeys);
	FRIEND_TEST(ParserTest, ReadKeyValue);
	FRIEND_TEST(ParserTest, ReadHeader);
	FRIEND_TEST(ParserTest, ReadTag);
	FRIEND_TEST(ParserTest,ReadSignOnMsgSRSV1);
	FRIEND_TEST(ParserTest, ReadSTMTTRN);

	
public:
	constexpr static int TagsCount(){ return sizeof(tags)/sizeof(tags[0]);}
	constexpr static int KeysCount(){ return sizeof(keys)/sizeof(keys[0]);}
	constexpr static int IndexOfKey(const char * key)
	{
		auto ndex=[](const char * p)constexpr-> int{
			auto i = std::lower_bound(keys,
				keys+KeysCount(), std::string_view(p)
			) - keys;
			return i==KeysCount() ||
				 keys[i]!=std::string_view(p) ? -1 : i;
			
		};
		return ndex(key);
	}
	constexpr static int IndexOfTag(std::string_view tag)
	{
		auto i = std::lower_bound(tags, tags+TagsCount(),
			tag) - tags;
		return i!=TagsCount() &&
			tag == tags[i] ? i : -1;
	}
	
	constexpr static int IndexOfCloseTag(std::string_view tag)
	{
		auto i = IndexOfTag(tag);
//		ASSERT(i!=-1);
		return -i-2;
	}
};

class FileInput : public Parser::Input {
public:
	FileInput(FILE* _fp)
		: fp(_fp)
	{
	}
	FileInput(const char* file);

	int peek() const override { return ungetc(fgetc(fp), fp); }
	int read() override{ return fgetc(fp); }
	void advance() override{ (void)fgetc(fp); }
	long tellg()const override{
		return ftell(fp);
	}
	void seekg(long pos)override{
		fseek(fp, pos, SEEK_SET);
	}
	~FileInput() override{
		if(fp!=stdin)
			fclose(fp);
	}
	
	//void unget(int ch)override{ ungetc(ch, fp); }

public:
	FILE* fp;
};

// the text to be parsed is in a contiguous memory location
//
class BuffInput : public Parser::Input {
public:
	BuffInput(char * buff, unsigned length)
		: beg(buff), end(buff+length), ptr(buff)
	{
	}

	int peek() const override { return ptr>=end ? EOF : *ptr; }
	int read() override{ return ptr<end ? *ptr++ : EOF; }
	void advance() override{ if(ptr<end)++ptr; }
	long tellg()const override{
		return ptr-beg;
	}
	void seekg(long pos)override{
		assert(pos>=0 && pos<=end-beg);
		ptr = beg + pos;
	}
	//void unget(int ch)override{ *--ptr=ch; }
	~BuffInput() override{}
	

public:
	char *beg, *end, *ptr;
};


inline bool Parser::Parse(const char * file, Info& info)
{
	FileInput in(file);
	return Parse(in, info);
}

inline bool Parser::Parse(FILE* fp, Info& info)
{
	FileInput in(fp);
	return Parse(in, info);
}

template <typename... Args>
void Parser::Info::Error(std::format_string<Args...> fmt, Args&&...args)
{
	std::string s=std::format(fmt, std::forward<Args&&>(args)...);

	auto& error = error_table();
	error.Append();
	error(0)=s.c_str();
}
