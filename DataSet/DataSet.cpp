#include "DataSet.h"


BEGIN_NAMESPACE_LZ

// return index of the named recordset in the
//      current DataSet; or -1 if none exists.
// this also rely on the underlying container's supporting
// array-like random access. May change in the future!!
int DataSet::IndexOf(const char *RecordSetName)const
{
	for(int i = 0; i < recordSets.GetCount(); ++i)
		if(_stricmp(RecordSetName, (*this)[i].GetName()) == 0)
			return i;
	return -1;
}



END_NAMESPACE