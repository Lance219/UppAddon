// to be included from within GridEz.h only

struct CachedData{
//	bool	 IsChanged()const{
//		return //width_changed == false && height_changed == false &&
//			row_accumulator_changed == false && col_accumulator_changed == false;
//	}
	
//	bool	IsWidthChanged()const{ return width_changed; }
//	bool	IsHeightChanged()const{ return height_changed; }
//	bool	IsRowAccumulatorChanged()const{ return row_accumulator_changed; }
//	bool	IsColAccumulatorChanged()const{ return col_accumulator_changed; }
//	void	SetAllChanged(){ //width_changed = height_changed =
//				 row_accumulator_changed = col_accumulator_changed = true;}

	GridEz				*who;
	CAManager			map;
	Size				orig_size;
	int					orig_position;
	int					resizing_rc;
	unsigned short		orig_dimension;
	bool				is_row;//: 1;
	Vector<short>		orig_dimensions;

	union{
		int32 dummy;
		struct{
			bool scrollx_changed			: 1;
			bool scrolly_changed			: 1;
			bool x_dimension_recalculate	: 1;
			bool y_dimension_recalculate	: 1;
			bool joined_cell_changed		: 1;
//			bool updated					: 1;
//			bool width_changed				: 1;
//			bool height_changed				: 1;
//			bool row_accumulator_changed	: 1;
//			bool col_accumulator_changed	: 1;
		};
	};
	
	void FlagMarkAll(){ scrollx_changed = scrolly_changed
		= x_dimension_recalculate= y_dimension_recalculate
		= joined_cell_changed = true;
	}
	
	void FlagClearAll(){ dummy = 0; }
	
	bool IsAnyFlagSet()const{ return dummy != 0; }
	bool NoFlagIsSet()const { return dummy == 0; }

};