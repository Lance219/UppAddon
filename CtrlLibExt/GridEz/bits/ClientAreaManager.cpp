#include "../GridEz.h"

BEGIN_NAMESPACE_LZ

typedef GridEz::CAManager CAManager;
typedef GridEz::CAManager::Accum Accum;

void CAManager::Set(Size& sz, int scrollX, int scrollY, int fixedLeftCols, int fixedRightCols,
                    int fixedTopRows, int fixedBtmRows)
{
	int r, lf, rf;
	col.scroll = scrollX;
	row.scroll = scrollY;
	col.alloc(fixedLeftCols, fixedRightCols, sz.cx, col.dimA, col.dimE, r, lf, col.dimFE, rf);
	row.alloc(fixedTopRows, fixedBtmRows, sz.cy, row.dimA, row.dimE, r, lf, row.dimFE, rf);
}

void Accum::alloc(int fixedstart, int fixedend, int cx, OUT int& lcx, OUT int& ccx,
                  OUT int& rcx, OUT int& lcxf, OUT int& ccxf, OUT int& rcxf)
{
	if(ends.size() < 1)
		return;

	lcxf = fixedstart > 0 ? end(fixedstart - 1) - start(0) : 0;

	rcxf = fixedend > 0 ? ends.back() - start(ends.size() - fixedend) : 0;

	ccxf = ends.back() - lcxf - rcxf;

	if(lcxf < cx) {
		lcx = lcxf;
		cx -= lcxf;

		if(rcxf < cx) {
			cx -= rcxf;
			rcx = rcxf;
			ccx = ccxf > cx ? cx : ccxf;
		}

		else {
			rcx = cx;
			ccx = 0;
		}
	}

	else {
		lcx = cx;
		rcx = ccx = 0;
	}
}

void Accum::UpdateVisibles(int fixedstart, int fixedend, int cx)
{
	int l, c, r, lf, cf, rf;
	// left, center, right in screen and full pic respectively
	int w;

	if(ends.size() < 1)
		return;

	visibles.Clear();

	alloc(fixedstart, fixedend, cx, l, c, r, lf, cf, rf);

	for(int i = 0; i < fixedstart && l > 0; ++i) {
		w = width(i);

		if(w > 0) {
			visibles.push_back(i);
			l -= w;
		}
	}

	if(c > 0) {
		int i = IndexOf(start(fixedstart)); // first visible row/col in Section E

		while(width(i) == 0)
			++i;

		// first scrollable column/row might be partially visible
		w = end(i) - scroll - lf;

		visibles.push_back(i);

		c -= w;

		while(++i < ends.size() - fixedend && c > 0) {
			w = width(i);

			if(w > 0) {
				c -= w;
				visibles.push_back(i);
			}
		}
	}

	if(visibles.GetCount() > fixedstart) {
		for(int i = IndexOf(start(fixedstart)); i < ends.size() - fixedend && c > 0; ++i) {
			w = width(i);

			if(w > 0) {
				visibles.push_back(i);
				c -= w;
			}
		}
	}

	if(r > 0) {
		Vector<int> fixedrights;

		for(int i = ends.size() - 1; r > 0 && i >= ends.size() - 1 - fixedend; --i) {
			w = width(i);

			if(w > 0) {
				fixedrights.push_back(i);
				r -= w;
			}
		}

		Reverse(fixedrights);

		visibles.Append(fixedrights);
	}
}

static void xlate(int& left, int& right, int& vleft, int& vright, int x1, int x2, int scroll,
                  int roffset, int lgroup, int rgroup)
{
	if(lgroup == 0) // 0 - left/top group, 1 center group, 2 right/bottom group
	{
		vleft = left;

		switch(rgroup) {

		case 0: // left/top group
			vright = right > x1 ? x1 : right;
			break;

		case 1: // center group
			vright = right - scroll;

			if(vright < x1)
				vright = x1;
			else if(vright > x2)
				vright = x2;

			break;

		default: // case 2 - right/bottom group
			vright = right - roffset;

			if(vright < x1)
				vright = x1;
		}
	}

	else if(rgroup == 2) {
		left -= roffset;
		right -= roffset;
		vright = right;
		vleft = left;

		if(lgroup == 1) // center group
		{
			if(vleft < x1)
				vleft = x1;

			if(vright < x2)
				vright = x2;
		}

		else {
			// same right group

			if(vleft < x2)
				vleft = x2;

			if(vright < x2)
				vright = x2;
		}
	}

	else {
		// joining center cells
		left -= scroll;
		right -= scroll;
		vleft = left, vright = right;

		if(vleft < x1)
			vleft = x1;

		if(vright > x2)
			vright = x2;
	}
}

const CAManager& CAManager::translateRect(Rect& r, Rect& vr, int toprowgroup, int leftcolgroup,
                                          int btmrowgroup, int rightcolgroup) const
{
	if(btmrowgroup == -1)
		btmrowgroup = toprowgroup;

	if(rightcolgroup == -1)
		rightcolgroup = leftcolgroup;

	xlate(r.left, r.right, vr.left, vr.right, col.dimA, col.dimA + col.dimE, col.scroll,
	      col.OffsetI(), leftcolgroup, rightcolgroup);

	xlate(r.top, r.bottom, vr.top, vr.bottom, row.dimA, row.dimA + row.dimE, row.scroll,
	      row.OffsetI(), toprowgroup, btmrowgroup);

	return *this;
}

END_NAMESPACE