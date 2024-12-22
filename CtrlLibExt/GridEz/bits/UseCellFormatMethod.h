#define INCLUDE_CELL_FORMAT_METHODS \
		Upp::Font	Font()const{ return format.Font(); }\
		Self&		Font(Upp::Font f){ format.Font(f); return *this;}\
		Color		Paper()const{ return format.Paper(); }\
		Self&		Paper(Color c){ format.Paper(c); return *this; }\
		Color		Ink()const{ return format.Ink(); }\
		Self&		Ink(Color c){ format.Ink(c); return *this; }\
		Color		BorderColor()const{ return format.BorderColor(); }\
		Self&		BorderColor(Color c){ format.BorderColor(c); return *this; }\
		int			PaddingLeft()const{\
			return format.PaddingLeft(); }\
		int			PaddingRightt()const{\
			return format.PaddingRight(); }\
		int			PaddingTop()const{\
			return format.PaddingTop(); }\
		int			PaddingBottom()const{\
			return format.PaddingBottom(); }\
		Self&		PaddingLeft(int value){\
					format.PaddingLeft(value); return *this; }\
		Self&		PaddingRight(int value){\
					format.PaddingRight(value); return *this; }\
		Self&		PaddingTop(int value){\
					format.PaddingTop(value); return *this; }\
		Self&		PaddingBottom(int value){\
					format.PaddingBottom(value); return *this; }\
		Self&		Padding(int value){\
					format.Padding(value); return *this; }\
		int			BorderLeft()const{\
			return format.BorderLeft(); }\
		Self&		BorderLeft(int border){\
			format.BorderLeft(border);\
			return *this;\
		}\
		int			BorderRight()const{\
			return format.BorderRight(); }\
		Self&		BorderRight(int border){\
			format.BorderRight(border);\
			return *this;\
		}\
		int			BorderTop()const{ return format.BorderTop();}\
		Self&		BorderTop(int border){\
			format.BorderTop(border);\
			return *this;\
		}\
		int			BorderBottom()const{\
			return format.BorderBottom(); }\
		Self&		BorderBottom(int border){\
			format.BorderBottom(border);\
			return *this;\
		}\
		Self&		NoBorders(){\
			format.BorderTop(BORDER_NONE)\
				.BorderBottom(BORDER_NONE)\
				.BorderLeft(BORDER_NONE)\
				.BorderRight(BORDER_NONE);\
			return *this;\
		}\
		int			HorzAlign()const{ return format.HorzAlign();}\
		Self&		HorzAlign(int align){\
			format.HorzAlign(align); return *this;\
		}\
		int			VertAlign()const{ return format.VertAlign();}\
		Self&		VertAlign(int align){\
			format.VertAlign(align); return *this;\
		}

