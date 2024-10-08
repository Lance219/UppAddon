#include <Core/Core.h>

using namespace Upp;
//	Upp::Font	font;
//	Color		bg,fg,bordercolor;
//	uint16		paddingLeft, paddingRight, paddingTop, paddingBottom;
//	union{
//		int32 dummy;
//		struct{
//			unsigned	borderLeft:3;
//			unsigned	borderRight:3;
//			unsigned	borderTop:3;
//			unsigned	borderBottom:3;
//			unsigned	halign:2;
//			unsigned	valign:2; //16th bit
//		
//			bool		faceNotNull:1;
//			bool		boldNotNull:1;
//			bool		heightNotNull:1;
//			bool		widthNotNull:1;
//			bool		underlineNotNull:1;
//			bool		italicNotNull:1;
//			bool		strikeoutNotNull:1; //23rd bit
//		};
//	};

struct A{
	int64 a;
	int32 b1,b2,b3;
	uint16 c1,c2,c3,c4;
	bool  f:1;
};

CONSOLE_APP_MAIN
{
	DUMP(sizeof(A));
}
