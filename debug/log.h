#ifndef __LZ_LOG_H__
#define __LZ_LOG_H__
#include <source_location>
#include <filesystem>
#include <format>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
//#include <Core/Core.h>


//#include "misc.h"
namespace lz{
	constexpr static bool debug =
#ifdef _DEBUG
									true;
#else
									false;
#endif

void log(std::string_view s);
void SetLogFile( const char* path);

#define DumP(a) Log("{}={}", #a, a)

class Enter
{
public:
	Enter(bool show=true, const std::source_location& _loc = std::source_location::current())
		: loc(_loc), show(show){
		Log("Entered {}({}):{}", loc.file_name(), loc.line(), loc.function_name());
	}
	template<typename ... Args>
	Enter& Log(std::format_string<Args...> fmt, Args&&... args)
	{
		if constexpr (debug){
			if(show)
				log(std::format(fmt, std::forward<Args>(args)...));
				
		}
		return *this;
	}
	~Enter(){
		Log("Exiting {}({}):{}", loc.file_name(), loc.line(), loc.function_name());
	}
	auto& Disable(){ show = false; return *this; }
	auto& Enable(){ show = true; return *this; }
private:
	std::source_location loc;
	int depth;
	bool show;
public:
};

}
#endif
