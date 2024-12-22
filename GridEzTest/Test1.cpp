
#include "Test1.h"


TEST(CellFormatTest, Basic)
{
	using namespace lz;

	CellFormat c(nullptr);
	EXPECT_TRUE(c.NoPropertiesSet());
	EXPECT_TRUE(c.NoFontPropertiesSet());
	EXPECT_TRUE(IsNull(c.Paper()));
	EXPECT_TRUE(IsNull(c.Ink()));
	EXPECT_TRUE(IsNull(c.BorderColor()));
	EXPECT_TRUE(IsNull(c.PaddingLeft()));
	EXPECT_TRUE(IsNull(c.PaddingRight()));
	EXPECT_TRUE(IsNull(c.PaddingTop()));
	EXPECT_TRUE(IsNull(c.PaddingBottom()));
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_TRUE(IsNull(c.FontWidth()));

	EXPECT_EQ(c.BorderLeft(), BORDER_NULL);
	EXPECT_EQ(c.BorderRight(), BORDER_NULL);
	EXPECT_EQ(c.BorderTop(), BORDER_NULL);
	EXPECT_EQ(c.BorderBottom(), BORDER_NULL);

	EXPECT_EQ(c.HorzAlign(), ALIGN_NULL);
	EXPECT_EQ(c.VertAlign(), ALIGN_NULL);

	EXPECT_TRUE(IsNull(c.FaceName()));
	EXPECT_EQ(c.Underline(), tribool::Null);
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_TRUE(IsNull(c.FontWidth()));
	EXPECT_EQ(c.Italic(), tribool::Null);
	EXPECT_EQ(c.Strikeout(), tribool::Null);

	// set face
	c.FaceName("serif");

	EXPECT_FALSE(IsNull(c.FaceName()));
	//DUMP(c.FaceName());
	//EXPECT_EQ(c.FaceName(), "serif");
	EXPECT_EQ(c.Underline(), tribool::Null);
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_TRUE(IsNull(c.FontWidth()));
	EXPECT_EQ(c.Italic(), tribool::Null);
	EXPECT_EQ(c.Strikeout(), tribool::Null);

	c.ClearFont();
	EXPECT_TRUE(IsNull(c.Paper()));
	EXPECT_TRUE(IsNull(c.Ink()));
	EXPECT_TRUE(IsNull(c.BorderColor()));
	EXPECT_TRUE(IsNull(c.PaddingLeft()));
	EXPECT_TRUE(IsNull(c.PaddingRight()));
	EXPECT_TRUE(IsNull(c.PaddingTop()));
	EXPECT_TRUE(IsNull(c.PaddingBottom()));
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_TRUE(IsNull(c.FontWidth()));

	EXPECT_EQ(c.BorderLeft(), BORDER_NULL);
	EXPECT_EQ(c.BorderRight(), BORDER_NULL);
	EXPECT_EQ(c.BorderTop(), BORDER_NULL);
	EXPECT_EQ(c.BorderBottom(), BORDER_NULL);

	EXPECT_EQ(c.HorzAlign(), ALIGN_NULL);
	EXPECT_EQ(c.VertAlign(), ALIGN_NULL);

	EXPECT_EQ(c.Underline(), tribool::Null);
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_TRUE(IsNull(c.FontWidth()));
	EXPECT_EQ(c.Italic(), tribool::Null);
	EXPECT_EQ(c.Strikeout(), tribool::Null);

	c.ClearFont();
	c.Strikeout(tribool::Yes);
	EXPECT_TRUE(IsNull(c.FaceName()));
	EXPECT_EQ(c.Underline(), tribool::Null);
	EXPECT_EQ(c.Bold(), tribool::Null);
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_TRUE(IsNull(c.FontWidth()));
	EXPECT_EQ(c.Italic(), tribool::Null);
	EXPECT_EQ(c.Strikeout(), tribool::True);

	c.ClearFont();
	c.FontHeight(32);
	EXPECT_TRUE(IsNull(c.FaceName()));
	EXPECT_EQ(c.Underline(), tribool::Null);
	EXPECT_EQ(c.Bold(), tribool::Null);
	EXPECT_EQ(c.FontHeight(), 32);
	EXPECT_TRUE(IsNull(c.FontWidth()));
	EXPECT_EQ(c.Italic(), tribool::Null);
	EXPECT_EQ(c.Strikeout(), tribool::Null);

	c.ClearFont();
	c.FontWidth(25);
	EXPECT_TRUE(IsNull(c.FaceName()));
	EXPECT_EQ(c.Underline(), tribool::Null);
	EXPECT_EQ(c.Bold(), tribool::Null);
	EXPECT_EQ(c.FontWidth(), 25);
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_EQ(c.Italic(), tribool::Null);
	EXPECT_EQ(c.Strikeout(), tribool::Null);

	c.ClearFont();
	c.Italic(tribool::True);
	EXPECT_TRUE(IsNull(c.FaceName()));
	EXPECT_EQ(c.Underline(), tribool::Null);
	EXPECT_EQ(c.Bold(), tribool::Null);
	EXPECT_TRUE(IsNull(c.FontWidth()));
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_EQ(c.Italic(), tribool::True);
	EXPECT_EQ(c.Strikeout(), tribool::Null);

	c.ClearFont();
	c.Bold(tribool::True);
	EXPECT_TRUE(IsNull(c.FaceName()));
	EXPECT_EQ(c.Underline(), tribool::Null);
	EXPECT_EQ(c.Bold(), tribool::Yes);
	EXPECT_TRUE(IsNull(c.FontWidth()));
	EXPECT_TRUE(IsNull(c.FontHeight()));
	EXPECT_EQ(c.Italic(), tribool::Null);
	EXPECT_EQ(c.Strikeout(), tribool::Null);

	c = c.DefaultCellFormat();
	EXPECT_FALSE(IsNull(c.Paper()));
	EXPECT_FALSE(IsNull(c.Ink()));
	EXPECT_FALSE(IsNull(c.BorderColor()));
	EXPECT_FALSE(IsNull(c.PaddingLeft()));
	EXPECT_FALSE(IsNull(c.PaddingRight()));
	EXPECT_FALSE(IsNull(c.PaddingTop()));
	EXPECT_FALSE(IsNull(c.PaddingBottom()));
	EXPECT_FALSE(c.BorderLeft() == BORDER_NULL);
	EXPECT_FALSE(c.BorderRight() == BORDER_NULL);
	EXPECT_FALSE(c.BorderTop() == BORDER_NULL);
	EXPECT_FALSE(c.BorderBottom() == BORDER_NULL);
	EXPECT_FALSE(c.HorzAlign() == ALIGN_NULL);
	EXPECT_FALSE(c.VertAlign() == ALIGN_NULL);
	EXPECT_TRUE(c.AllFontPropertiesSet());
	EXPECT_TRUE(c.AllPropertiesSet());
}

TEST(CellFormatTest, Compose)
{
	using namespace lz;
	CellFormat c(nullptr), col(nullptr), row(nullptr), grid;

	//grid = grid.DefaultCellFormat();
	c.Compose(row, col, grid);
	EXPECT_TRUE(c == c.DefaultCellFormat());

	col.FaceName("monospace");
	col.FontHeight(300);
	col.FontWidth(75);
	col.Bold(tribool::True);
	col.Italic(tribool::True);
	col.Underline(tribool::True);
	col.Strikeout(tribool::True);
	col.Paper(Upp::Red());
	col.Ink(Upp::Blue());
	col.BorderColor(Upp::Green());
	col.Border(BORDER_DOUBLE);
	col.Padding(50);
	col.HorzAlign(ALIGN_CENTER).VertAlign(ALIGN_BOTTOM);
	c.Compose(row, col, grid);
	EXPECT_TRUE(c == col);

	row.FaceName("serif");
	row.FontHeight(200);
	row.FontWidth(175);
	row.Bold(tribool::False);
	row.Italic(tribool::False);
	row.Underline(tribool::False);
	row.Strikeout(tribool::False);
	row.Paper(Upp::White());
	row.Ink(Upp::Green());
	row.BorderColor(Upp::Red());
	row.Border(BORDER_SINGLE);
	row.Padding(150);
	row.HorzAlign(ALIGN_RIGHT).VertAlign(ALIGN_CENTER);
	c.Compose(row, col, grid);
	EXPECT_TRUE(c == row);

	Rect a(1,2,3,4), b(0,0,1,2);
	EXPECT_FALSE(a.Intersects(b));
	b = Rect(3,2,4,3);
	EXPECT_FALSE(a.Intersects(b));
	b = Rect(2,3,4,4);
	EXPECT_TRUE(a.Intersects(b));
	
//	row.FontHeight(20);
//	LOG("Font Name Serif");
//	LOG("SET Font Height to 20");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontHeight(30);
//	LOG("SET Font Height to 30");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontWidth(25);
//	LOG("SET Font Width to 25");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontWidth(50);
//	LOG("SET Font Width to 50");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	
//	row.FaceName("monospace");
//	LOG("");
//	LOG("Font name: monospace");
//	LOG("SET Font Height to 20");
//	row.FontHeight(20);
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontHeight(30);
//	LOG("SET Font Height to 30");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontWidth(25);
//	LOG("SET Font Width to 25");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontWidth(50);
//	LOG("SET Font Width to 50");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	
//	LOG("");
//	LOG("Font Name: sansserif");
//	row.FaceName("sansserif");
//	LOG("SET Font Height to 20");
//	row.FontHeight(20);
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontHeight(30);
//	LOG("SET Font Height to 30");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontWidth(25);
//	LOG("SET Font Width to 25");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
//	row.FontWidth(50);
//	LOG("SET Font Width to 50");
//	DUMP(row.FontHeight());
//	DUMP(row.FontWidth());
}

TEST(JoinedCellManagerTest, Add)
{
	using namespace lz;
	GridEz::JoinedCellManager m;
	m.Add(0,0,5,5);
	EXPECT_EQ(m.GetCount(), 1);
	m.Add(1,1,5,5);
	EXPECT_EQ(m.GetCount(), 1);
	m.Add(1,5,3,7);
	m.Add(5,3,7,5);
	m.Add(5,5,7,7);
	EXPECT_EQ(m.GetCount(), 4);
	m.Add(2,2,6,6);
	EXPECT_EQ(m.GetCount(), 1);
	
	m.Clear();
	for(int r=0; r<100; r+=2)
		for(int t=0; t<100; t+=2)
			m.Add(r,t,r+2, t+2);
	EXPECT_EQ(m.GetCount(),2500);
	m.Clear();
	for(int r=100; r>0; r-=2)
		for(int t=100; t>0; t-=2)
			m.Add(r-2,t-2,r, t);
	EXPECT_EQ(m.GetCount(),2500);
	
	m.Add(1,1,99,99);
	EXPECT_EQ(m.GetCount(),1);
	
}

TEST(GridEzTest, AddColumn)
{
	using namespace lz;

	GridEz grid;
	{
		auto& c = grid.AddColumn();
		EXPECT_EQ(c.Width(), static_cast<uint16>(DEFAULT_COL_WIDTH));
		EXPECT_EQ(grid.GetColCount(), 1);
	}
	{
		auto& c = grid.AddColumn("abc"_z, 200);
		EXPECT_EQ(c.Width(), 200u);
		EXPECT_EQ(grid.GetColCount(), 2);
	}
}

TEST(GridEzTest, FullPictureToGrid)
{
	using namespace lz;

	GridEz grid;
	grid.NoFitColumns();
	for(int i=0; i<20; ++i)
		grid.AddColumn(100);
	for(int i=0; i<200; ++i)
		grid.AddRow();
	grid.SetRect(0,0,1000,800);
	
	DUMP(grid.GetSize());
}
