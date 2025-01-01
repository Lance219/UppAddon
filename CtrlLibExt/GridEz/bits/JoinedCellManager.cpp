#include "../../CtrlLibExt.h"

BEGIN_NAMESPACE_LZ

void GridEz::JoinedCellManager::Add(int leftCol, int topRow, int rightCol, int btmRow)
{
	Rect r(leftCol, topRow, rightCol, btmRow);
	int insertion_point = -1;

	int j = 0;
	for(int i = 0; i < joinedCells.size(); ++i) {
		if(!r.Intersects(joinedCells[i])) {
			if(insertion_point==-1 &&
				(r.left < joinedCells[i].left ||
					(r.left==joinedCells[i].left && r.top < joinedCells[i].top)
				)
			){
				insertion_point = j;
			}
			if(i != j)
				joinedCells[j] = joinedCells[i];
			++j;
			
		}
	}
	
	joinedCells.SetCount(j);
	if(insertion_point==-1)
		joinedCells.Add(r);
	else
		joinedCells.Insert(insertion_point, r);
}

bool GridEz::JoinedCellManager::RemoveRegionContainsCell(int col, int row)
{
	bool v = false;
	for(auto& region : joinedCells)
		if(region.Contains(col, row))
		{
			joinedCells.Remove(int(&region - joinedCells.Begin()));
			v = true;
		}
	return v;
}


END_NAMESPACE