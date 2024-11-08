#include <CtrlLib/CtrlLib.h>
#include <CtrlLibExt/CtrlLibExt.h>
#include <regex>

using namespace Upp;

class MyWin : public TopWindow
{
public:
	MyWin(){ fe.SetText("Hello, world!"); }
	
	void Paint(Draw& w)override{
		w.Clipoff(10,30,100,20);
		fe.Paint(w, Size{100,20});
		w.End();
	}

private:
	lz::FakeEdit fe;

};

GUI_APP_MAIN
{
	MyWin().Run();
}
