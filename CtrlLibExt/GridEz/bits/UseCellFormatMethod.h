#define CELL_FORMAT_ITEM(obj, item, type)\
	type item()const{ return obj.item(); }\
	Self& item(type v){ obj.item(v); return *this; }

#define CELL_FORMAT_ITEMS(obj)\
		CELL_FORMAT_ITEM(obj, Font, Upp::Font)\
        CELL_FORMAT_ITEM(obj, Bold, tribool)\
        CELL_FORMAT_ITEM(obj, FaceName, String)\
        CELL_FORMAT_ITEM(obj, Underline, tribool)\
        CELL_FORMAT_ITEM(obj, Strikeout, tribool)\
        CELL_FORMAT_ITEM(obj, FontHeight, int)\
        CELL_FORMAT_ITEM(obj, FontWidth, int)\
        CELL_FORMAT_ITEM(obj, Italic, tribool)\
		CELL_FORMAT_ITEM(obj, Paper, Color)\
		CELL_FORMAT_ITEM(obj, Ink, Color)\
		CELL_FORMAT_ITEM(obj, BorderColor, Color)\
		CELL_FORMAT_ITEM(obj, PaddingLeft, int)\
		CELL_FORMAT_ITEM(obj, PaddingRight, int)\
		CELL_FORMAT_ITEM(obj, PaddingTop, int)\
		CELL_FORMAT_ITEM(obj, PaddingBottom, int)\
		Self&		Padding(int value){\
					obj.Padding(value); return *this; }\
		CELL_FORMAT_ITEM(obj, BorderLeft, int)\
		CELL_FORMAT_ITEM(obj, BorderRight, int)\
		CELL_FORMAT_ITEM(obj, BorderTop, int)\
		CELL_FORMAT_ITEM(obj, BorderBottom, int)\
		Self&		NoBorders(){\
			obj.BorderTop(BORDER_NONE)\
				.BorderBottom(BORDER_NONE)\
				.BorderLeft(BORDER_NONE)\
				.BorderRight(BORDER_NONE);\
			return *this;\
		}\
		CELL_FORMAT_ITEM(obj, HorzAlign, int)\
		CELL_FORMAT_ITEM(obj, VertAlign, int)
