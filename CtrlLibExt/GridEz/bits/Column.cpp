//#include "../GridEz.h"
#include <CtrlLibExt/CtrlLibExt.h>

BEGIN_NAMESPACE_LZ

GridEz::Column& GridEz::Column::Width ( int w )
{
	ASSERT ( w >= DEFAULT_COL_WIDTH && w < ( 1 << sizeof ( width ) * 8 ) );
	// runtime correction, silently fix out of range value
	if ( w < DEFAULT_COL_WIDTH ){
		w = DEFAULT_COL_WIDTH;
		LOG("Set GridEz::Column width to negative value quietly ignored. Set to DEFAULT instead");
	}else
		if ( w >= ( 1 << sizeof ( width ) * 8 ) ){
			w = ( 1 << sizeof ( width ) * 8 ) - 1;
			LOG("GridEz::Column: attempt to set column width to a value exceeding implementation limit is ignored");
		}
	naturalwidth = w;
	width = w;

	return *this;
}

END_NAMESPACE