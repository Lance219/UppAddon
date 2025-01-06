
struct Column: public Moveable<Column>
{
	friend class GridEz;

public:
	typedef Column Self;
	
	~Column(){ CheckRemoveEdit(); }

	Column() :  naturalwidth ( DEFAULT_COL_WIDTH ),
				datacolumn ( -1 ), hidden(false), scaleweight(0),
				canresize(true),format(nullptr),edit(nullptr) { }

	int		Width() const
	{
		return width == 0xffff ? -1 : width;
	}
	
	
	Column& Width ( int w );

	int		FullWidth() const {	return naturalwidth == 0xffff ? -1 : naturalwidth;}
	Self&   FullWidth(int _width){
			if( _width< DEFAULT_COL_WIDTH || _width>std::numeric_limits<decltype(naturalwidth)>::max()-1 )
			{
				if( _width < DEFAULT_COL_WIDTH )
					_width = DEFAULT_COL_WIDTH;
				else
					_width = std::numeric_limits<decltype(naturalwidth)>::max()-1;
				LOG("GridEz::Column::FullWidth(int w): set width to a value exceeding implementatioin limit is ignored");
			}
			naturalwidth = _width;
			return *this;
	}
	//int		FullWidthIfNoHidden() const { return hidden ? 0 : FullWidth();	}
	bool	HasDefaultWidth()const {
		return naturalwidth == std::numeric_limits<decltype(naturalwidth)>::max();
	}

	int		DataColumn() const
	{
		return datacolumn;
	}
	
	Ctrl*	Edit()const{ return const_cast<Self&>(*this).edit; }
	Self&	Edit(Ctrl* ctrl){
		if(edit!=ctrl)
			CheckRemoveEdit();
		edit = ctrl;
		return *this;
	}
	template <typename T, typename ...Ts>
	Self& EditWith(Ts&&... ts)
	{
		CheckRemoveEdit();
		One<T> c;
		c.Create(std::forward<Ts>(ts)...);
		edit = MarkAsOwned(c.Detach());
	}
		
	byte	ScaleWeight()const			{ return scaleweight;			}
	Self&   ScaleWeight(byte w)			{ scaleweight = w; return *this;}
	
	bool	IsHidden()const				{ return hidden;				}
	Column&  SetHidden(bool b = true)	{ hidden = b; return *this;		}
	Column&  Hide(bool b = true)		{ hidden = b; return *this;		}
	
	bool	IsResizable()const			{ return canresize;				}
	Column& SetResizable(bool b=true)	{ canresize = b; return *this;	}
	

	Column& DataColumn ( int fieldno )
	{
		if ( fieldno < -1 || fieldno >= 0xffff  )
			Panic ( "Column& GridEz::DataColumn(int fiedno)-- value of fieldno out of range!" );

		datacolumn = uint16 ( fieldno );

		return *this;
	}

	operator const CellFormat& () const
	{
		return format;
	}

	CELL_FORMAT_ITEMS(format)
private:
	void SetWidthInternal(uint16 width){ this->width = width; }
	void CheckRemoveEdit()
	{
		if(edit){
			edit->Remove();
			if(IsOwned(edit))
				delete edit;
		}
	}

private:
	uint16		width; // this field is calculated at each Paint(may optimize later to reduce calculation)
	uint16		naturalwidth;
	uint16		datacolumn;
	byte		scaleweight;
	bool		hidden:1;
	bool		canresize:1;
	CellFormat  format;
	Ctrl*		edit;
};
