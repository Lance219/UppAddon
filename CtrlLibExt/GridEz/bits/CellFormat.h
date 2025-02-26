static constexpr int const
	BORDER_NULL = 0,
	BORDER_NONE = 1,
	BORDER_SINGLE =2,
	BORDER_THICK=3,
	BORDER_DOUBLE=4;
static constexpr int const
	DEFAULT_COL_WIDTH = -1,
	DEFAULT_ROW_HEIGHT = -1;

class CellFormat : Moveable<CellFormat>{
	typedef CellFormat Self;
public:
	
	CellFormat();
	/*_CXX23_CONSTEXPR*/ CellFormat(std::nullptr_t)
	{
		static_assert(std::is_same_v<decltype(paddingLeft),uint8> &&
			std::is_same_v<decltype(marginLeft), uint8>,
			"Changes in paddingLeft..'s type break initial assumption and the following code");
		//paddingLeft = paddingRight = paddingTop
		//	= paddingBottom = std::numeric_limits<decltype(paddingLeft)>::max();
		*reinterpret_cast<int64*>(&paddingLeft)=-1ll;
	}

	Upp::Font	Font()const{ return font; }
	Self&		Font(Upp::Font f){ font=f; return FontSetAll();}
	Color		Paper()const{ return bg; }
	Self&		Paper(Color c){ bg=c; return *this; }
	Color		Ink()const{ return fg; }
	Self&		Ink(Color c){ fg=c; return *this; }
	Color		BorderColor()const{ return bordercolor; }
	CellFormat&	BorderColor(Color c){ bordercolor=c; return *this; }
		
	CellFormat&		Padding(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		paddingLeft = paddingRight = paddingBottom = paddingTop = value;
		return *this;
	}

	CellFormat&		PaddingLeft(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		paddingLeft = value;
		return *this;
	}

	CellFormat&		PaddingRight(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		paddingRight = value;
		return *this;
	}

	CellFormat&		PaddingTop(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		paddingTop = value;
		return *this;
	}

	CellFormat&		PaddingBottom(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		
		paddingBottom = value;
		return *this;
	}
	
	int	PaddingLeft()const{
		int v = paddingLeft;
		
		if( v == std::numeric_limits<decltype(paddingLeft)>::max() )
			v = Null;
		return v;
	}

	int	PaddingRight()const{
		int v = paddingRight;
		
		if( v == std::numeric_limits<decltype(paddingLeft)>::max() )
			v = Null;
		return v;
	}


	int	PaddingTop()const{
		int v = paddingTop;
		
		if( v == std::numeric_limits<decltype(paddingLeft)>::max() )
			v = Null;
		return v;
	}

	int	PaddingBottom()const{
		int v = paddingBottom;
		
		if( v == std::numeric_limits<decltype(paddingLeft)>::max() )
			v = Null;
		return v;
	}

	CellFormat&		Margin(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		paddingLeft = paddingRight = paddingBottom = paddingTop = value;
		return *this;
	}

	CellFormat&		MarginLeft(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		paddingLeft = value;
		return *this;
	}

	CellFormat&		MarginRight(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		paddingRight = value;
		return *this;
	}

	CellFormat&		MarginTop(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		paddingTop = value;
		return *this;
	}

	CellFormat&		MarginBottom(int value){
		if(value < 0 || IsNull(value))
			value = std::numeric_limits<decltype(paddingLeft)>::max();
		
		ASSERT(value <= std::numeric_limits<decltype(paddingLeft)>::max());
		
		paddingBottom = value;
		return *this;
	}
	
	int	MarginLeft()const{
		int v = paddingLeft;
		
		if( v == std::numeric_limits<decltype(paddingLeft)>::max() )
			v = Null;
		return v;
	}

	int	MarginRight()const{
		int v = paddingRight;
		
		if( v == std::numeric_limits<decltype(paddingLeft)>::max() )
			v = Null;
		return v;
	}


	int	MarginTop()const{
		int v = paddingTop;
		
		if( v == std::numeric_limits<decltype(paddingLeft)>::max() )
			v = Null;
		return v;
	}

	int	MarginBottom()const{
		int v = paddingBottom;
		
		if( v == std::numeric_limits<decltype(paddingLeft)>::max() )
			v = Null;
		return v;
	}

	int			BorderLeft()const{	return int(flags.borderLeft); }

	CellFormat&		BorderLeft(int border){
		ASSERT(border>=BORDER_NULL && border<=BORDER_DOUBLE);
		flags.borderLeft=border;
		return *this;
	}
	
	int				BorderRight()const{
		return int(flags.borderRight); }
	
	CellFormat&		BorderRight(int border){
		ASSERT(border>=BORDER_NULL && border<=BORDER_DOUBLE);
		flags.borderRight=border;
		return *this;
	}
	
	int				BorderTop()const{ return flags.borderTop;}
	CellFormat&		BorderTop(int border){
		ASSERT(border>=BORDER_NULL && border<=BORDER_DOUBLE);
		flags.borderTop=border;
		return *this;
	}
	
	int				BorderBottom()const{
		return flags.borderBottom; }
	CellFormat&		BorderBottom(int border){
		ASSERT(border>=BORDER_NULL && border<=BORDER_DOUBLE);
		flags.borderBottom=border;
		return *this;
	}
	
	CellFormat& Border(int border){
		ASSERT(border>=BORDER_NULL && border<=BORDER_DOUBLE);
		flags.Border(border);
		return *this;
	}
	
	int				HorzAlign()const{ return flags.halign;}
	CellFormat&		HorzAlign(int align){
		static_assert(static_cast<int>(Alignment::ALIGN_NULL)==0,
			"Change in <Gtypes.h> - Alignment enum value breaks codes!");
		ASSERT(align>=ALIGN_NULL && align<=ALIGN_CENTER);
		flags.halign = align; return *this;
	}
	int				VertAlign()const{ return flags.valign;}
	CellFormat&		VertAlign(int align){
		static_assert(static_cast<int>(Alignment::ALIGN_NULL)==0,
			"Change in <Gtypes.h> - Alignment enum value breaks codes!");
		ASSERT(align>=ALIGN_NULL && align<=ALIGN_CENTER);
		flags.valign = align; return *this;
	}
	
	tribool	Bold()const{  return flags.boldNotNull ? static_cast<tribool>(font.IsBold()) : tribool::Null; }
	CellFormat& Bold(tribool v){
		if( v== tribool::Null )
		{
			flags.boldNotNull = false;
		}else{
			flags.boldNotNull = true;
			font.Bold(static_cast<bool>(v));
		}
		return *this;
	}
	
	String FaceName()const{ return flags.faceNotNull ? font.GetFaceName() : Null; };
	Self&  FaceName(String face){
		bool v = false;
		if (int n = Font::FindFaceNameIndex(face) ; n != -1 ){
			font.Face(n);
			v = true;
		}
		flags.faceNotNull = v;
		return *this;
	}
	
	tribool Underline()const{ return
		flags.underlineNotNull ? static_cast<tribool>(font.IsUnderline()) : tribool::Null; }
	Self& Underline(tribool v){
		if( v== tribool::Null )
		{
			flags.underlineNotNull = false;
		}else{
			flags.underlineNotNull = true;
			font.Underline(static_cast<bool>(v));
		}
		return *this;
	}
		
	tribool Strikeout()const{ return
		flags.strikeoutNotNull ? static_cast<tribool>(font.IsStrikeout()) : tribool::Null; }
	Self& Strikeout(tribool v){
		if( v== tribool::Null )
		{
			flags.strikeoutNotNull = false;
		}else{
			flags.strikeoutNotNull = true;
			font.Strikeout(static_cast<bool>(v));
		}
		return *this;
	}
		
	int FontHeight()const{ return flags.heightNotNull? font.GetHeight() : Null; }
	Self& FontHeight(int h){
		font.Height(h);
		flags.heightNotNull = true;
		return *this;
	}
		
	
	int FontWidth()const{ return flags.widthNotNull ? font.GetWidth() : Null ; }
	Self& FontWidth(int w){
		font.Width(w);
		flags.widthNotNull = true;
		return *this;
	}
	
	tribool Italic()const{ return
		flags.italicNotNull ? static_cast<tribool>(font.IsItalic()) : tribool::Null; }
	Self& Italic(tribool v){
		if( v== tribool::Null )
		{
			flags.italicNotNull = false;
		}else{
			flags.italicNotNull = true;
			font.Italic(static_cast<bool>(v));
		}
		return *this;
	}

		
	void Compose(const CellFormat& row, const CellFormat& col, const CellFormat& grid);
	
	Self& ClearFont(){
		return FontClearAll();
	}
	
	bool AllPropertiesSet()const;
	bool NoPropertiesSet()const;
	
	bool AllFontPropertiesSet()const{
		return flags.AllFontPropertiesSet();
	}
	
	bool NoFontPropertiesSet()const{
		return flags.NoFontPropertiesSet();
	}
	
	bool operator == (const Self& rhs)const;

	static CellFormat& DefaultCellFormat();
	static const CellFormat& NullCellFormat();
	
	String ToString()const{
		String s;
		using Upp::Format;
		s<<"Font:"<<font<<", paper:"<<bg<<", ink:"<<fg<<", line color:"<<bordercolor
		 <<Format("\npadding:%d,%d,%d,%d", paddingLeft, paddingRight, paddingTop, paddingBottom)
		 <<Format("\nborder:%d,%d,%d, %d", static_cast<int>(flags.borderLeft),
			static_cast<int>(flags.borderRight), static_cast<int>(flags.borderTop),
			static_cast<int>(flags.borderBottom))
		 <<Format("\nalignment:%d,%d", static_cast<int>(flags.halign), static_cast<int>(flags.valign))
		 <<Format("\nfaceNotNull:%d, boldNotNull:%d, heightNotNull:%d, widthNotNull:%d",
			flags.faceNotNull, flags.boldNotNull, flags.heightNotNull, flags.widthNotNull)
		 <<Format("\nunderlineNotNull:%d, italicNotNull:%d, strikeoutNotNull:%d\n",
			flags.underlineNotNull, flags.italicNotNull, flags.strikeoutNotNull);
		return s;
	}
	
	
	
private:
#include "CellFormatBitfields.h"
	Upp::Font	font;
	uint8		paddingLeft, paddingRight, paddingTop, paddingBottom;
	uint8		marginLeft, marginRight, marginTop, marginBottom;
	Color		bg,fg,bordercolor;
	Flags       flags;

	CellFormat&	FontSetAll(){
		flags.SetAllFontProperties();
		return *this;
	}
	CellFormat& FontClearAll(){
		flags.ClearAllFontProperties();
		return *this;
	}
};

//void ComposeFormat(CellFormat& dest, const CellFormat& CellData,
//		const CellFormat& col, const CellFormat& grid
//);


class GridEz;

struct CellPainter: public CellFormat
{
	GridEz * p;
	Draw& w;
	Ctrl * ctrl;
	String text;
	Rect cellrect;
	Rect visible_rect;
	int64 datarow;
	int datacol;
	int col;
	int row;
	
	CellPainter(Draw& w, GridEz* p):CellFormat(nullptr), w(w), p(p)
	{
		Clear();
	}
	
	void Clear(){
		// value of w and p will remain unchanged in its life time
		// col,  row, datacol, cellrect, visible_rect, etc will be
		// updated, so we leave them as garbage value.
		std::construct_at((CellFormat*)this, nullptr);
		ctrl = nullptr;
		text.Clear();
	}
	
	void Paint();
	
	void DrawBorders();
	
	bool RequestingData()const{ return datacol != -1 && datarow != -1; }
};

class CellFormatFromIndex
{
private:
	Upp::VectorMap<String, CellFormat> map;
};