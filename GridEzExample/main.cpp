#include <CtrlLib/CtrlLib.h>

//#include <GridEz/GridEz.h>
#include <GridCtrl/GridCtrl.h>
#include <TabBar/TabBar.h>
#include <CtrlLibExt/CtrlLibExt.h>

using namespace Upp;
using namespace lz;

#define LAYOUTFILE <GridEzExample/main.lay>
#include <CtrlCore/lay.h>
#include <vector>
// problem
// tab.Add(apple.SizePos());
// run normally in Debug, but will crash with stack overflow
// in release mode.
///
// put SizePos() at the end is fine;
// Directly add to TopWindow is fine.
//
struct MyApp :  WithMainLayout<TopWindow>
{
	MyApp(){
		CtrlLayout(*this,t_("GridEz Example"));

		CtrlLayout(ctrl);
		CtrlLayout(coord);
		CtrlLayout(layout);
		tab.Add(fs,t_("Apple Income Statement"));
		tab.Add(ctrl,"Ctrls Layout");
		tab.Add(coord, t_("Coordinate Translation"));
		tab.Add(layout, t_("More Controls"));
		fs.SizePos();
		ctrl.SizePos();
		layout.SizePos();
		coord.SizePos();

		AppleFS();
		InitCtrlTab();
		InitLayoutTab();
		
		coord.ez.DefaultColWidth(100).DefaultRowHeight(40);
		for(int c=0; c<30; ++c)
			coord.ez.AddColumn();
		for(int r=0; r<5000; ++r)
		{
			auto& row=coord.ez.AddRow(nullptr);
			for(int c=0; c<30; ++c)
				row.At(c)=String().Cat()<<r<<','<<c;
		}
		coord.ez.SetFixedTopRowCount(3);
		coord.ez.SetFixedBottomRowCount(2);
		coord.ez.SetFixedLeftColCount(2);
		coord.ez.SetFixedRightColCount(3);
		coord.ez.WhenMouseMove<<[=,this](Point p, dword){
			String s;
			GridEz::CellPoint cp=coord.ez.GridToCell(p);
			s << "Mouse at point " << p.ToString()<<
				"; row=" << cp.row << ", col=" << cp.col
				<<"; Mouse coordinate inside the cell is "
				<<cp.point.ToString();
			coord.prompt1.SetLabel(s);
		};
	}
	
	bool PaintCell(CellPainter& info);
	
	void AppleFS();
	void InitCtrlTab();
	void InitLayoutTab();
	
public:
	GridEz fs;
	WithGridEzLayout<Ctrl> ctrl;
	WithCoordinateTranslationLayout<Ctrl> coord;
	WithCtrlLayout<Ctrl> layout;
	EditString edit;
	EditInt edit1;
	
};


GUI_APP_MAIN
{
	MyApp p;
	DUMP(sizeof(GridEz::JoinedCellManager));
	DUMP(sizeof(Upp::Ctrl));
	DUMP(sizeof(Upp::ScrollBar));
	DUMP(sizeof(CellFormat));
	DUMP(sizeof(GridEz::CAManager));
	DUMP(sizeof(GridEz));
	DUMP(sizeof(Image));
	DUMP(sizeof(GridEz::CachedData));
	
	p.Sizeable().MinimizeBox().MaximizeBox().Maximize();
	p.Run();
}


static String Format(double number, unsigned decimal, const char * zeroAs="");

void MyApp::AppleFS()
{
		fs.Font(StdFont().Height(22))
			.DefaultColWidthFromFont().DefaultRowHeightFromFont()
			.NoBorders();
		
		fs.FitColumns();
		fs.AddColumn(600/* width */,0 /* field no */).ScaleWeight(10); // item description
		fs.AddColumn(20); // gap of 20 pixels wide
		fs.AddColumn(150,1).HorzAlign(ALIGN_RIGHT).ScaleWeight(7); // 3months ends march 26 ,2016
		fs.AddColumn(20); // gap
		fs.AddColumn(150,2).HorzAlign(ALIGN_RIGHT).ScaleWeight(7); // 3months ends march 28 ,2015
		fs.AddColumn(20); // separator
		fs.AddColumn(150,3).HorzAlign(ALIGN_RIGHT).ScaleWeight(7); // 6 months ends march 26 ,2016
		fs.AddColumn(20);
		fs.AddColumn(150,4).HorzAlign(ALIGN_RIGHT).ScaleWeight(7); // 6 months ends march 28 ,2015
		
		
		//fs.SetFixedLeftColCount(1);
		
		
		fs.AddHeightedRow(200,"Apple Inc"_z).HorzAlign(ALIGN_CENTER).VertAlign(ALIGN_BOTTOM);
		fs.AddRow("UNAUDITED CONDENSED STATEMENTS OF OPERATIONS"_z)
				.HorzAlign(ALIGN_CENTER).Bold(tribool::Yes);
		fs.AddHeightedRow(80,"(In millions, except number of shares which are reflected in "
			"thousands and per share amounts)"_z).HorzAlign(ALIGN_CENTER);
		fs.JoinCells(0,0,9,1);
		fs.JoinCells(0,1,9,1);
		fs.JoinCells(0,2,9,1);
		fs.AddHeightedRow(80,nullptr,nullptr,"Three Months ended"_z,nullptr,nullptr,nullptr,
		    "Three Months ended"_z).HorzAlign(ALIGN_CENTER)
		    .VertAlign(ALIGN_BOTTOM).Bold(tribool::Yes);
		fs.JoinCells(2,3,3,1).JoinCells(6,3,3,1);
		fs.AddRow(nullptr,nullptr,"March 26,"_z,nullptr,"March 28,"_z,
			nullptr,"March 26,"_z,nullptr,"March 28,"_z)
			.HorzAlign(ALIGN_CENTER).VertAlign(ALIGN_BOTTOM).Bold(tribool::Yes);
		fs.AddRow(nullptr,nullptr,"2016"_z,nullptr,"2015"_z,
			nullptr,"2016"_z,nullptr,"2015"_z)
			.HorzAlign(ALIGN_CENTER).VertAlign(ALIGN_BOTTOM).Bold(tribool::Yes);
			
		fs.SetFixedTopRowCount(fs.GetRowCount());
		fs.AddExternalRow(0,60).VertAlign(ALIGN_BOTTOM);
		fs.AddExternalRow(1);
		fs.AddExternalRow(2);
		fs.AddRow("Operating expenses:"_z);
		for(int i=3; i<11; ++i)
			fs.AddExternalRow(i);
		fs.AddRow("Earnings per share:"_z/*,60*/);
		fs.AddExternalRow(11);
		fs.AddExternalRow(12);
		fs.AddRow("Earnings used in computing earnings per share:"_z);
		fs.AddExternalRow(13);
		fs.AddExternalRow(14);
		fs.AddRow("Cash dividends declared per share"_z);
		fs.AddRow(" Includes share-based compensation expense as follows:"_z);
		fs.AddExternalRow(15);
		fs.AddExternalRow(16);
		fs.AddExternalRow(17);
//		fs.AddRow("Hello, world!", "Hello, world!"_z);
//		fs.SetFixedBottomRowCount(1);
		
		fs.WhenPaintingCell<<[=, this](CellPainter& info){return PaintCell(info);};
}


bool MyApp::PaintCell(CellPainter& info)
{
	if(info.row==3 || info.row==5 || info.row==7 || info.row==8 ||
		info.row==11 || info.row==12 || info.row==14 || info.row==16
	)
	{
		if(info.datacol>0 )
			info.BorderBottom(BORDER_SINGLE);
	}else if(info.row==17 && info.datacol>0)
		info.BorderBottom(BORDER_DOUBLE);
	
	if(info.RequestingData())
	{
		// faking the situation where data is retrieved from
		// a database
		struct row{ const char * item; double data[4];};
		static row data[]={
			{"Net sales", 50557,58010,12642,132609},
			{"Cost of sales",30636,34354,76085,79212},
			{"\tGross margin",19921,23656,50344,53397},
			{" Research and development",2511,1918,4915,3813},
			{" Selling, general and administrative", 3423,3460,7271,7060},
			{"\t Total operation expenses",5934,5378,12186,10873},
			{"Operating income",13987,18278,38158,42524},
			{"Other income(expense), net", 155, 286, 557, 456},
			{"Income before provision for income taxes", 14142, 18564, 38715, 42980},
			{"Provision for income taxes", 3626, 4995, 9838, 11387},
			{"Net income",10516,13569,28877,31593},
			{"  Basic",1.91,2.34,5.22,5.43},
			{"  Diluted",1.90,2.33,5.19,5.39},
			{"  Basic", 5514381, 5793799, 5536656, 5818441},
			{"  Diluted", 5540886, 5834858, 5567506, 5858330},
			{"Cash dividends delcared per share", 0.52, 0.47, 1.04, 0.94},
			{"  Cost of sales", 191, 142, 395,282},
			{"  Research and development", 468, 384, 934, 758},
			{"  Selling, general and administrative", 389, 401, 797, 775}
		};
		if(info.datacol==0)
			info.text<<data[info.datarow].item;
		else{
			double d=data[info.datarow].data[info.datacol-1];
			info.text<<Format(d, d == double(int(d)) ? 0 : 2 );
		}
	}
	return false;
}

static String Format(double number, unsigned decimal, const char * zeroAs)
{
	unsigned long long ipart,fract,mult=1ULL;
	String s;
	char buff[20];
	bool sign = number < 0;

	if(IsNull(number))
		number = 0.;

	if(number < -1e10 || number > 1e10)
		return String("ERROR");

	if(sign)
		number = -number;

	double f=.5;

	for(unsigned i=0; i<decimal; i++){
		mult*=10;
		f/=10.;
	}
	number+=f;
	ipart=(unsigned long long)number;
	fract=(unsigned long long)((number-ipart)*mult);
	if(ipart==0uLL && fract==0uLL && zeroAs!=NULL)
		return s<<zeroAs;

	if(decimal>0)
		s<<fract;
	
	if((unsigned)s.GetLength()<decimal)
		s=String('0', decimal-s.GetLength()).Cat()<<s;

	if(decimal>0)
		s='.'+s;
	if(ipart==0)
		s="0"+s;
	while(ipart)
	{
		int j=ipart%1000;
		ipart /=1000;
		if(ipart)
		{
			sprintf(buff, ",%03d", j);
		}else{
			sprintf(buff, "%d", j);
		}
		s=buff+s;
	}
	if(sign)
		s.Insert(0,'-');
	return s;
}

void MyApp::InitCtrlTab()
{
	GridEz& g=ctrl.fs;
	
	g.Font(StdFont().Height(22))
			.DefaultColWidthFromFont().DefaultRowHeightFromFont()
			.NoBorders()
			.NoFitColumns()
			.HighlightCurRow(false);
		
	auto& c = g.AddColumn(200);
	g.AddColumn(200);
	g.AddColumn(200);
	g.AddColumn(200);
	g.AddRow(); // an empty row
	GridEz::Row& r=g.AddRow("Hello"_z, &edit, "World"_z,nullptr);// row 1
	GridEz::Row& s=g.AddRow("Name"_z,nullptr,"Age"_z); // row 2
	s.At(1).Create<EditInt>().SetFrame(NullFrame()).SetData(156);
	s.At(3).Create<EditDouble>().SetData(100.35);
	GridEz::Row& t=g.AddHeightedRow(200); // row 3
	Clock& clock=t.At(1).Create<Clock>();
	g.SetFixedLeftColCount(1);
	g.JoinCells(1,3,3,1);
	SetTimeCallback(-1000,[&clock]{clock.Reset();},1);
}


void MyApp::InitLayoutTab()
{
	auto& g = layout.grid;

	g.Font(StdFont().Height(22))
			.DefaultColWidthFromFont().DefaultRowHeightFromFont()
			.NoBorders()
			.FitColumns()
			.FitRows()
			
			;//.NoFitColumns();
	g.AddColumn(300).HorzAlign(ALIGN_RIGHT).PaddingRight(10).PaddingBottom(5);
	g.AddColumn(300);
	g.AddRow("Item"_z, "Enter Value"_z).Bold(tribool::True).Ink(Green()).FontHeight(24);
	g.SetFixedTopRowCount(1);
	for(int i=0; i<30; ++i)
	{
		auto& row = g.AddRow(String().Cat()<<"Prompt "<<i+1, nullptr);
		row.At(1).Create<EditString>().SetFont(StdFont().Height(22))
			.SetFrame(BlackFrame());
	}
	
}