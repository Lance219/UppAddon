// to be included by GridEz.h only
// moved out of class GridEz class body to reduce clutter
class JoinedCellManager{
public:
	//JoinedCellManager()=default;
	void Clear(){ joinedCells.Clear(); }
	
	void Add(int leftCol, int topRow, int rightCol, int btmRow);
	
	int GetCount()const{ return joinedCells.GetCount(); }
	int size()const{ return GetCount(); }
	const Rect& operator[](int index)const{ return joinedCells[index]; }
	auto begin()const{ return joinedCells.begin(); }
	auto end()const{ return joinedCells.end(); }
	
	bool RemoveRegionContainsCell(int col, int row);
	
	String ToString()const{ return joinedCells.ToString(); }
private:
	Vector<Rect> joinedCells; // Rect is in terms of row and column, instead of pixel coord
};