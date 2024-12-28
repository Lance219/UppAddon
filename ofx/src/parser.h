// internal header

class Parser {
public:
	class Input {
	public:
		virtual int peek() const = 0;
		virtual int read() = 0;
		virtual void advance() = 0;
		//virtual void unget(int ch)=0;
		virtual ~Input(){}
	};
	
//	enum class TokenType: char{
//		LessSign, // when reading header, got a <
//		Error,	// unknown or unexpected keys/tags
//		HeaderKey,
//		OpenTag,
//		CloseTag,
//	};
//	struct Token{
//		TokenType type;
//		int		   id;
//	};

public:
	bool Parse(Input& in);
	bool Parse(const char * file);
	
	// note if passed in 'fp' is not stdin, it will be
	// closed after the call
	//
	bool Parse(FILE * fp);


private:
//	Token HeaderReadKeyValue(Input& in, std::string& value);
	bool ReadHeader(Input& in, DataSet& ds);
	bool ReadKeyValue(Input& in, int key_index, std::string& value);
	
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

	
public:
	constexpr int IndexOfKey(const char * key)
	{
		auto ndex=[](const char * p)constexpr-> int{
			auto i = std::lower_bound(keys,
				keys+sizeof(keys)/sizeof(keys[0]), std::string_view(p)
			) - keys;
			return i==sizeof(keys)/sizeof(keys[0]) ||
				 keys[i]!=std::string_view(p) ? -1 : i;
			
		};
		return ndex(key);
	}
	constexpr int IndexOfTag(std::string_view tag)
	{
		auto i = std::lower_bound(tags, tags+sizeof(tags)/sizeof(tags[0]),
			tag) - tags;
		return i!=sizeof(tags)/sizeof(tags[0]) &&
			tag == tags[i] ? i : -1;
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
	//void unget(int ch)override{ *--ptr=ch; }
	~BuffInput() override{}
	

public:
	char *beg, *end, *ptr;
};


inline bool Parser::Parse(const char * file)
{
	FileInput in(file);
	return Parse(in);
}

inline bool Parser::Parse(FILE* fp)
{
	FileInput in(fp);
	return Parse(in);
}