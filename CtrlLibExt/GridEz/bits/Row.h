
struct Row : public Moveable<Row> {

	class RowData {

	public:
		RowData() : rowfmt(nullptr){}
		~RowData();
		CellFormat& RowFormat();
		int64 DataRowId() const;
		RowData& DataRowId(int64 id);

		int Height() const { return height; }

		RowData& Height(int height);

		tribool Bold() const { return rowfmt.Bold(); }

		RowData& Bold(tribool v)
		{
			rowfmt.Bold(v);
			return *this;
		}

		int HorzAlign() const { return rowfmt.HorzAlign(); }

		RowData& HorzAlign(Alignment align)
		{
			rowfmt.HorzAlign(align);
			return *this;
		}

		int VertAlign() const { return rowfmt.VertAlign(); }

		RowData& VertAlign(Alignment align)
		{
			rowfmt.VertAlign(align);
			return *this;
		}

		static RowData* NewFrom(int64 dataid, int height);
		static size_t ActualSize(bool data_external, bool rowfmt, bool cellfmt);

		bool hasrowfmt : 1; // has row-level CellFormat?

		bool modified : 1;

		bool hasctrl : 1;

		byte data_type : 3; // how the row's CellData values are stored

		uint16 data_cnt;
		uint16 max_col_no;
		int16 height;
		// union{
		int64 datarowid;
		Vector<Cell> cells;
		// CellData * data;
		// };
		CellFormat rowfmt;

		friend struct Row;
	};

	String ToString() const
	{
		String s;

		if(!info)
			s << "No data, default height";

		switch(PtrType()) {

		case 1: // not a pointer
		{
			int64 id;
			int h;
			GetCombinedDataIdNHeight(id, h);
			s << "InTheRowExternal Data(id:" << id << ",row height:" << h;
		}

			return s;

		case 2:
			s << "PtrType=2,?";
			return s;

		case 3:
			s << "PtrType=3, Ctrl";
			return s;
		}

		switch(info->data_type) {

		case GridEz::DT_LOCAL: {
			String s;

			for(auto& i : info->cells)
				s << i.GetDataPChar() << ' ';

			return s;
		}
		}

		return "TO be implemented";
	}

private:
	RowData* info;

	int PtrType() const { return lz::AsInt(info) & 3; }

	void AllocRowFormat();
	bool HasRowData() const { return info != nullptr && lz::AsInt(info) % 4 == 0; }

	void GetCombinedDataIdNHeight(int64& dataid, int& height) const;
	Row& StoreDataIDAndHeight(int64 dataid, int height);

public:
	Row()
		: info(nullptr)
	{
	}

	~Row();

	Row& Bold(tribool v);
	Cell& At(int col)
	{
		if(!HasRowData()) {
			int64 dataid;
			int height;
			GetCombinedDataIdNHeight(dataid, height);
			info = new RowData;
			info->data_type = GridEz::DT_LOCAL;
			info->Height(height);
		}

		else {
			info->data_type = GridEz::DT_LOCAL;
		}

		return info->cells.At(col);
	}

	const Cell& GetCell(int col) const
	{
		if(info && (lz::AsInt(info) & 3) == 0 && col < info->cells.GetCount()) {
			return info->cells[col];
		}

		return Cell::NullCell();
	}

	tribool Bold() const { return HasRowData() ? info->Bold() : tribool::Null; }

	int HorzAlign() const { return HasRowData() ? info->HorzAlign() : ALIGN_NULL; }

	Row& HorzAlign(Alignment align);
	int VertAlign() const { return HasRowData() ? info->VertAlign() : ALIGN_NULL; }

	Row& VertAlign(Alignment align);
	CellFormat* GetRowFormat() { return HasRowData() ? &info->rowfmt : nullptr; }

	const CellFormat* GetRowFormat() const
	{
		return const_cast<const CellFormat*>(const_cast<Row&>(*this).GetRowFormat());
	}

	const CellFormat& GetFormat(int col) const
	{
		return HasRowData() ? info->rowfmt : lz::CellFormat::NullCellFormat();
	}

	bool WithDefaultHeight() const { return Height() == DEFAULT_ROW_HEIGHT; }
	int Height() const;
	Row& Height(int h);
	int64 RowDataId() const;
	Row& RowDataId(int64 id);

	bool IsEmpty() const { return !HasRowData() || info->cells.IsEmpty(); }

	int DataStorage() const;
	String GetText(int col) const;
	Value GetData(int col) const;

	friend class GridEz;
};
