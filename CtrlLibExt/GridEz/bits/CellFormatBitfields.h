union Flags{
	int32 dummy;
	struct{
		byte	borderLeft  :3;
		byte	borderRight :3;
		byte	borderTop   :3;
		byte	borderBottom:3;
		byte	halign      :2;
		byte	valign      :2; //16th bit
	
		bool	faceNotNull     :1;
		bool	boldNotNull     :1;
		bool	heightNotNull   :1;
		bool	widthNotNull    :1;
		bool	underlineNotNull:1;
		bool	italicNotNull   :1;
		bool	strikeoutNotNull:1; //23rd bit
	};
	
	constexpr Flags() : dummy(0){ static_assert(sizeof(*this)==sizeof(dummy)); }
	
	
	static constexpr int32 FontMask()
	{
		Flags f;
		f.faceNotNull = true;
		f.boldNotNull = true;
		f.heightNotNull = true;
		f.widthNotNull = true;
		f.underlineNotNull = true;
		f.italicNotNull = true;
		f.strikeoutNotNull = true;
		return f.dummy;
	}
	
	static constexpr int32 AllNullFlag()
	{
		Flags f;
		f.borderTop = BORDER_NULL;
		f.borderBottom = BORDER_NULL;
		f.borderLeft = BORDER_NULL;
		f.borderRight = BORDER_NULL;
		f.halign = ALIGN_NULL;
		f.valign = ALIGN_NULL;
		return f.dummy;
	}
	
	void Border(int border)
	{
		borderLeft = borderRight = borderTop = borderBottom = border;
	}
	
	void SetAllFontProperties()
	{
		dummy |= FontMask();
	}

	void ClearAllFontProperties()
	{
		dummy &= ~FontMask();
	}
	
	bool AllFontPropertiesSet()const
	{
		return ( dummy & FontMask() ) == FontMask();
	}
	
	bool NoFontPropertiesSet()const
	{
		return ( dummy & ~FontMask() ) == 0;
	}
	
	bool AllPropertiesNull()const
	{
		return dummy == AllNullFlag();
	}
};

