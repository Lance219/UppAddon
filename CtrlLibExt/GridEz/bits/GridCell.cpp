#include "../../CtrlLibExt.h"


BEGIN_NAMESPACE_LZ

Cell::Cell ( String s )
{
	pCellData = new CellT<String> ( s );
}

Cell::Cell ( lz::StrLiteral text, bool isT )
{
	if ( text.get() == nullptr ) [[unlikely]]
		pCellData = nullptr;
	else if (CellData* p = reinterpret_cast<CellData*>(
				const_cast<char*>(text.get())
		);	(AsInt(p) & 7) == 0
	)[[unlikely]]
	{
//		ptr_as_int p(reinterpret_cast<CellData*>(
//			const_cast<char*>(text.get())
//		));
		pCellData = (ptr_as_int(p)+= isT ? 2: 1);
//		LOG(String().Cat() << "In " << __func__ << ", text.p is conformant");
//		MaskAsASCIIZPointer ( text.get() );
	}
	else
	{
//		LOG("Cell::Cell( lz::StrLiteral ) : pointer not conformant");
//		DUMP(text.get());
//		DUMP((const void*)text.get());
		One<CellT<String>> tmp;
		tmp.Create(text.get());
		pCellData = tmp.Detach();
	}
}

Cell::~Cell()
{
	//Ctrl * c = reinterpret_cast<Ctrl*> ( lz::AsInt ( pCellData ) &  ~3 ) );

	switch ( GetFlags() )
	{
	case 0:
		delete pCellData;
		break;
		// case 1: break; // do nothing for ASCIIZ string literal

	case 3: // Ctrl*;
		{
			Ctrl * c = reinterpret_cast<Ctrl*> ( AsInt(pCellData) - 3 );
			ASSERT ( lz::AsUInt ( pCellData ) - lz::AsUInt ( c ) == 3 );
			// it's weired, but our ctrl will be responsible to free
			// its Box parent if any and registered to be deallocated
			// by setting its owned flag
			Ctrl * p = c->GetParent();
			if(p && dynamic_cast<_detail::Box*>(p)!=nullptr )
			{
				p->Remove();
				if(IsOwned( p)){
					LOG("An owned Box is deallocated");
					delete p;
				}else{
					LOG("A box that's not owned");
				}
			}

			c->Remove();
	
			if ( IsOwned ( c ) )
			{
				delete c;
			}
	
			break;
		}
	}
}

const char * Cell::GetDataPChar() const
{
	if ( pCellData == nullptr )
		return nullptr;

	switch ( GetFlags() )
	{

		case 1:
			return const_cast<const char*> (
					   reinterpret_cast<char*> (
						   lz::AsInt ( pCellData ) - 1
					   )
				   );

		case 2://
			return nullptr;
//		return t_(const_cast<const char*>(
//			reinterpret_cast<char*>(
//				lz::AsInt(pCellData)-2
//			)
//		));

		case 3:// Ctrl.
			return nullptr;
	}

	return pCellData->GetDataPChar();
}

END_NAMESPACE