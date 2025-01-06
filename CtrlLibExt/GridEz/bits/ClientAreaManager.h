// included by GridEz.h only.
struct CAManager
{
	// so that we can use it for both rows and columns
	//
	struct Accum
	{
		int start(int rc)const
		{
			ASSERT(rc>=0 && rc<ends.size());
			return rc==0 ? 0 : ends[--rc];
		}
		
		int end(int rc)const
		{
			ASSERT(rc>=0 && rc<ends.size());
			return ends[rc];
		}
		
		int width(int rc)const
		{
			return end(rc)-start(rc);
		}
		
		int TotalWidth()const
		{
			return ends.back();
		}
		
		int GridToFullPicture(int position)const
		{
			if(position >= dimA + dimE)// in fixed right columns or fixed bottom rows
				position += OffsetI();
			else if(position >= dimA){ // in center scrollable area
				position += scroll;
			}
			return position;
		}
		
		int FullPictureToGrid(int position, int& section)const
		{
			section = 0;
			if(position >= dimA + dimFE)
			{
				section = 2;
				position -= OffsetI();
			}else if(position >= dimA){
				position -= scroll;
				section = 1;
			}
			return position;
		}
		
		int IndexFullPictureToGrid(int fp_index)const
		{
			int p=std::lower_bound(visibles.begin(),visibles.end(),fp_index)
							-visibles.begin();
			return p!=visibles.size() && visibles[p] == fp_index ? p : -1;
		}

		int OffsetI()const{ return dimFE-dimE;}
		
		int IndexOf(int position)const
		{
			int rc=-1;
			position=GridToFullPicture(position);
			if(position>=0 && !ends.IsEmpty() && position<ends.back())
			{
				rc=(int)(std::upper_bound(ends.begin(),ends.end(),position)-ends.begin());
			}
			return rc;
		}
		
		bool IsVisible(int index)const
		{
			return IndexFullPictureToGrid(index) != -1;
		}
		
//		bool IsFullyVisible(int index)const
//		{
//			int i = IndexFullPictureToGrid(index);
//			return ;
//		}
		
		int MakeVisible(int index)const
		{
			if(dimE==0) return scroll;
			int _start=start(index), _end=end(index)-1;
			if(_start>=dimA && _start<dimA+dimFE && _start-dimA<scroll)
				return _start-dimA;
			if(_end>=dimA && _end<dimA+dimFE && _end-dimA>scroll+dimE)
				return _end-dimA-dimE;
			return scroll;
		}
		
		void UpdateVisibles(int fixedstart, int fixedend, int cx);
		
		void alloc(int fixedstart, int fixedend, int cx,
			OUT int&lcx, OUT int&ccx, OUT int&rcx,
			OUT int&lcxf, OUT int&ccxf, OUT int&rcxf);
		
		Vector<int> ends;
		Vector<int>	visibles; // only used when !FitColumns(). visibles.size() is the number of column in GridEz
		int total_weight; // only used when FitColumns();
		int delta; // only used when FitColumns();
		
		int scroll; // only used when !FitColumns()
		// Section Diagram of a representative GridEz matrix
		// A B C
		// D E F
		// G H I
		//
		int dimA;  // only used when !FitColumns();
		int dimE;  // only used When !FitColumns();
		int dimFE; // only used when !FitColumns();
	};
	Accum col,row; // col.ends.size() is the number of columns in full picture
					// col.visibles.size() is the number of column in GridEz
	Vector<Vector<int>> joined;

	CAManager(){}
	void Set(Size& sz, int scrollX, int scrollY, int fixedLeftColCount,
		int fixedRightColCount, int fixedTopRowCount, int fixedBtmRowCount
	);
	
	int Left  (int fullpic_col)const{return col.start(fullpic_col);}
	int Right (int fullpic_col)const{return col.end(fullpic_col);  }
	int Top   (int fullpic_row)const{return row.start(fullpic_row);}
	int Bottom(int fullpic_row)const{return row.end(fullpic_row);  }
	int TotalWidth()const{ return col.TotalWidth(); }
	int TotalHeight()const{ return row.TotalWidth(); }
	int WidthOfEInGrid()const{ return col.dimE; }
	int HeightOfEInGrid()const{ return row.dimE; }
	int WidthOfEInFullPicture()const{ return col.dimFE;}
	int HeightOfEInFullPicture()const{ return row.dimFE;}
	int GetColumnIndexAt(int x)const{ return col.IndexOf(x);         }
	int GetRowIndexAt(int y)   const{ return row.IndexOf(y);            }
	int AtRightBoundOfColumn(int x)const{
		int c=col.IndexOf(x);
		if(c==-1)
			return -1;
		x = col.GridToFullPicture(x);
		if(x>5 && c!=0 && Left(c)+5>x)
			return c-1;
		if(c>0 && Right(c)-5<x)
			return c;
		return -1;
	}
	
	int AtBottomBoundOfRow(int y)const{
		int r=row.IndexOf(y);
		if(r == -1)
			return -1;
		y = row.GridToFullPicture(y);
		if(y>5 && r!=0 && Top(r)+5>y)
			return r-1;
		if(r>0 && Bottom(r)-5<y)
			return r;
		return -1;
	}
	
	CAManager& Clear(){
		this->~CAManager();
		new(this)CAManager();
		return *this;
	}
	
	const CAManager& translateRect(Rect& r, Rect& vr, int toprowgroup,  int leftcolgroup,
		int btmrowgroup=-1,	 int rightcolgroup=-1)const;
};