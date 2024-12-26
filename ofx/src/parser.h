// internal header

class Parser {
public:
	class Input {
	public:
		virtual int peek() const = 0;
		virtual int read() = 0;
		virtual void advance() = 0;
		virtual ~Input(){}
	};

public:
	void Parse(Input& in);
	void Parse(const char * file);
	
	// note if passed in 'fp' is not stdin, it will be
	// closed after the call
	//
	void Parse(FILE * fp);


private:
	int get_id(Input& in);
	
	constexpr static const char * keys[]={
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
#include "tags.h"
	};
	
	FRIEND_TEST(ParserTest, TagsAndKeys);

	
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

public:
	FILE* fp;
};

inline void Parser::Parse(const char * file)
{
	FileInput in(file);
	Parse(in);
}

inline void Parser::Parse(FILE* fp)
{
	FileInput in(fp);
	Parse(in);
}