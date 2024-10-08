#include <Core/Core.h>

using namespace Upp;

struct A{
	int64 a;
	int32 b;
	uint16 c1,c2,c3,c4;
	bool  f:1;
};

CONSOLE_APP_MAIN
{
	DUMP(sizeof(A));
}
