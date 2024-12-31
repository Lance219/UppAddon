#define INCLUDE_CELL_FORMAT_METHODS \
		CELL_FORMAT_GET(format)\
		CELL_FORMAT_SET(format)
//		Upp::Font	Font()const{ return format.Font(); }\
//		Self&		Font(Upp::Font f){ format.Font(f); return *this;}\
//		Color		Paper()const{ return format.Paper(); }\
//		Self&		Paper(Color c){ format.Paper(c); return *this; }\
//		Color		Ink()const{ return format.Ink(); }\
//		Self&		Ink(Color c){ format.Ink(c); return *this; }\
//		Color		BorderColor()const{ return format.BorderColor(); }\
//		Self&		BorderColor(Color c){ format.BorderColor(c); return *this; }\
//		int			PaddingLeft()const{\
//			return format.PaddingLeft(); }\
//		int			PaddingRightt()const{\
//			return format.PaddingRight(); }\
//		int			PaddingTop()const{\
//			return format.PaddingTop(); }\
//		int			PaddingBottom()const{\
//			return format.PaddingBottom(); }\
//		Self&		PaddingLeft(int value){\
//					format.PaddingLeft(value); return *this; }\
//		Self&		PaddingRight(int value){\
//					format.PaddingRight(value); return *this; }\
//		Self&		PaddingTop(int value){\
//					format.PaddingTop(value); return *this; }\
//		Self&		PaddingBottom(int value){\
//					format.PaddingBottom(value); return *this; }\
//		Self&		Padding(int value){\
//					format.Padding(value); return *this; }\
//		int			BorderLeft()const{\
//			return format.BorderLeft(); }\
//		Self&		BorderLeft(int border){\
//			format.BorderLeft(border);\
//			return *this;\
//		}\
//		int			BorderRight()const{\
//			return format.BorderRight(); }\
//		Self&		BorderRight(int border){\
//			format.BorderRight(border);\
//			return *this;\
//		}\
//		int			BorderTop()const{ return format.BorderTop();}\
//		Self&		BorderTop(int border){\
//			format.BorderTop(border);\
//			return *this;\
//		}\
//		int			BorderBottom()const{\
//			return format.BorderBottom(); }\
//		Self&		BorderBottom(int border){\
//			format.BorderBottom(border);\
//			return *this;\
//		}\
//		Self&		NoBorders(){\
//			format.BorderTop(BORDER_NONE)\
//				.BorderBottom(BORDER_NONE)\
//				.BorderLeft(BORDER_NONE)\
//				.BorderRight(BORDER_NONE);\
//			return *this;\
//		}\
//		int			HorzAlign()const{ return format.HorzAlign();}\
//		Self&		HorzAlign(int align){\
//			format.HorzAlign(align); return *this;\
//		}\
//		int			VertAlign()const{ return format.VertAlign();}\
//		Self&		VertAlign(int align){\
//			format.VertAlign(align); return *this;\
//		}
//		tribool		Bold()const{ return a.Bold(); }\
//		String		FaceName()const{ return a.FaceName(); }\
//		tribool		Underline()const{ return a.Underline(); }\
//		Color		Paper()const{ return a.Paper(); }\
//		Color		Ink()const{ return a.Ink(); }\
//		Color		BorderColor()const{ return a.BorderColor(); }\
//		int			PaddingLeft()const{\
//			return a.PaddingLeft(); }\
//		int			PaddingRightt()const{\
//			return a.PaddingRight(); }\
//		int			PaddingTop()const{\
//			return a.PaddingTop(); }\
//		int			PaddingBottom()const{\
//			return a.PaddingBottom(); }\
//		int			BorderLeft()const{\
//			return a.BorderLeft(); }\
//		int			BorderRight()const{\
//			return a.BorderRight(); }\
//		int			BorderTop()const{ return a.BorderTop();}\
//		int			BorderBottom()const{\
//			return a.BorderBottom(); }\
//		int			HorzAlign()const{ return a.HorzAlign();}\
//		int			VertAlign()const{ return a.VertAlign();}

#define CELL_FORMAT_GET_ITEM(a, item) \
	auto item()const{ return a.item(); }
#define CELL_FORMAT_SET_ITEM(a, item, type) \
	Self& item(type v){ a.item(v); return *this; }

#define CELL_FORMAT_GET(a) \
		CELL_FORMAT_GET_ITEM(a, Font)\
		CELL_FORMAT_GET_ITEM(a, Bold)\
		CELL_FORMAT_GET_ITEM(a, FaceName)\
		CELL_FORMAT_GET_ITEM(a, Underline)\
		CELL_FORMAT_GET_ITEM(a, Strikeout)\
		CELL_FORMAT_GET_ITEM(a, FontHeight)\
		CELL_FORMAT_GET_ITEM(a, FontWidth)\
		CELL_FORMAT_GET_ITEM(a, Italic)\
		CELL_FORMAT_GET_ITEM(a, Paper)\
		CELL_FORMAT_GET_ITEM(a, Ink)\
		CELL_FORMAT_GET_ITEM(a, BorderColor)\
		CELL_FORMAT_GET_ITEM(a, PaddingLeft)\
		CELL_FORMAT_GET_ITEM(a, PaddingRight)\
		CELL_FORMAT_GET_ITEM(a, PaddingTop)\
		CELL_FORMAT_GET_ITEM(a, PaddingBottom)\
		CELL_FORMAT_GET_ITEM(a, BorderLeft)\
		CELL_FORMAT_GET_ITEM(a, BorderRight)\
		CELL_FORMAT_GET_ITEM(a, BorderTop)\
		CELL_FORMAT_GET_ITEM(a, BorderBottom)\
		CELL_FORMAT_GET_ITEM(a, HorzAlign)\
		CELL_FORMAT_GET_ITEM(a, VertAlign)

#define CELL_FORMAT_SET(a)\
		Self&		Font(Upp::Font f){ a.Font(f); return *this;}\
		Self&		Bold(tribool v){ a.Bold(v); return *this; }\
		Self&		FaceName(String face){ a.FaceName(face); return *this;}\
		CELL_FORMAT_SET_ITEM(a, Underline, tribool)\
		CELL_FORMAT_SET_ITEM(a, Strikeout, tribool)\
		CELL_FORMAT_SET_ITEM(a, FontHeight, int)\
		CELL_FORMAT_SET_ITEM(a, FontWidth, int)\
		CELL_FORMAT_SET_ITEM(a, Italic, tribool)\
		Self&		Paper(Color c){ a.Paper(c); return *this; }\
		Self&		Ink(Color c){ a.Ink(c); return *this; }\
		Self&		BorderColor(Color c){ a.BorderColor(c); return *this; }\
		Self&		PaddingLeft(int value){\
					a.PaddingLeft(value); return *this; }\
		Self&		PaddingRight(int value){\
					a.PaddingRight(value); return *this; }\
		Self&		PaddingTop(int value){\
					a.PaddingTop(value); return *this; }\
		Self&		PaddingBottom(int value){\
					a.PaddingBottom(value); return *this; }\
		Self&		Padding(int value){\
					a.Padding(value); return *this; }\
		Self&		BorderLeft(int border){\
			a.BorderLeft(border);\
			return *this;\
		}\
		Self&		BorderRight(int border){\
			a.BorderRight(border);\
			return *this;\
		}\
		Self&		BorderTop(int border){\
			a.BorderTop(border);\
			return *this;\
		}\
		Self&		BorderBottom(int border){\
			a.BorderBottom(border);\
			return *this;\
		}\
		Self&		NoBorders(){\
			a.BorderTop(BORDER_NONE)\
				.BorderBottom(BORDER_NONE)\
				.BorderLeft(BORDER_NONE)\
				.BorderRight(BORDER_NONE);\
			return *this;\
		}\
		Self&		HorzAlign(int align){\
			a.HorzAlign(align); return *this;\
		}\
		Self&		VertAlign(int align){\
			a.VertAlign(align); return *this;\
		}
