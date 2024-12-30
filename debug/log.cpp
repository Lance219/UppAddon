#include "log.h"

namespace lz{

class FilePtr {
public:
	~FilePtr()
	{
		if(fp)
			fclose(fp);
	}
	FilePtr()
		: fp(nullptr)
	{
	}
	FilePtr(const FilePtr&) = delete;
	FilePtr(FilePtr&& _fp)
		: fp(_fp.fp)
	{
		_fp.fp = nullptr;
	}

	operator FILE*() { return fp; }

	bool IsNull() const { return fp == nullptr; }

	FilePtr& operator=(FILE* _fp)
	{
		if(fp)
			fclose(fp);
		fp = _fp;
		return *this;
	}
	static std::string fname;

private:
	FILE* fp;
};

std::string FilePtr::fname;


const char* get_home_directory()
{
	const char* p;
	return (p = secure_getenv("HOME")) != nullptr ? p : getpwuid(getuid())->pw_dir;
}


FilePtr& get()
{
	static FilePtr obj;

	using namespace std::filesystem;
	
	if(obj.IsNull()) [[unlikely]] {
		if(FilePtr::fname.empty())
			FilePtr::fname = std::format("{}/vm.log", get_home_directory());
		auto _path = path(FilePtr::fname);
		create_directories(_path.parent_path());
		
		if( exists(_path.parent_path()) ){
			//DUMP(FilePtr::fname.c_str());
			obj = fopen(FilePtr::fname.c_str(), "w");
			//DUMP((void*)obj);
		}
		if(obj.IsNull())
			fprintf(stderr, "Unable to create log file '%s'\n", FilePtr::fname.c_str());
	}
	return obj;
}

void log(const char* s)
{
	if(FILE* fp = get(); fp) {
		fprintf(fp, "%s\n", s);
		fflush(fp);
	}
}

void log_(const char* s, ...)
{
	if(FILE* fp = get(); fp) {
		va_list arglist;
		va_start(arglist, s);
		vfprintf(fp, s, arglist);
		fflush(fp);
		va_end(arglist);
	}
}


void log(std::string_view s)
{
	if(FILE* fp = get(); fp) {
		//LOG("Loging with string_view");
		for(auto c:s)
			fputc(c, fp);
		fputc('\n', fp);
		fflush(fp);
	}
}



void SetLogFile(const char* file)
{
	if constexpr (debug)
	{
		std::string p = get_home_directory();
		FilePtr::fname = p+'/'+file;
	}
}


}