#include "CtrlCoreExt.h"

namespace lz
{


// it can be demonstrated that when a Ctrl to be relocated has parents,
// has 1 or 0 sibling and no children, it cannot be adjusted reliably
// without its old address, or the guarantee that content at its old address
// remain unchanged between the relocation and calling this function.
//
// As we cannot guarantee the later, we supply the otherwise dispensable
// `oldaddress`
	void CtrlHack::DoPostRelocationAdjustment(const CtrlHack * old)noexcept
	{
		if ( CtrlHack * p = GetParent(); p != nullptr )
		{
			ASSERT ( old == next_sibling->prev_sibling );

			if ( prev_sibling == old ) // when prev_sibling point to itself
			{
				// `*this` is the only child of its parent
				ASSERT ( next_sibling == old );
				p -> children = this;
				prev_sibling = next_sibling = this;
			}

			else
			{
				if ( p->children == old ) // *`this` is the first child of its parent
					p->children = this;

				prev_sibling -> next_sibling = this;

				next_sibling -> prev_sibling = this;
			}
		}

		if ( CtrlHack *p = children; children != nullptr )
		{
			do
			{
				p->uparent = this;
				p = p->next_sibling;
			}

			while ( p != children );
		}

	}


}
