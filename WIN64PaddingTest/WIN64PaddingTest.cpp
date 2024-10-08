#include <Core/Core.h>

using namespace Upp;

struct A{
	int64 a;
	int32 b;
	bool  f:1;
};

CONSOLE_APP_MAIN
{
	DUMP(sizeof(A));
}
