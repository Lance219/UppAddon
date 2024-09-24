#include "Test1.h"
#include <CtrlCoreExt/CtrlCoreExt.h>
#include <CoreExt/CoreExt.h>
#include <CtrlLib/CtrlLib.h>
#include <Core/Core.h>

using namespace Upp;

void RawCopy ( void * dest, const auto* src )
{
#pragma CLANG diagnostic push
#pragma CLANG diagnostic ignored "-Wdynamic-class-memaccess"
	memcpy ( dest, src, sizeof ( decltype ( *src ) ) );
#pragma CLANG diagnostic pop

}

TEST ( CtrlHackTest, SizeLayoutFine )
{
	/* Note, the definition of class Ctrl in Upp are
	*  not expected to change frequently.
	*
	* However, if it indeed changes, this hack will fail
	* without being modified accordingly.
	*
	* The failure of this test will indicate Ctrl definition
	* change; but a success may not be able to rely on.
	*
	* Ideally Upp::Ctrl can expose used/padding space to derived classes
	*
	*/

	using lz::CtrlHack;
	using Upp::Ctrl;
	Ctrl c;
	CtrlHack& h = reinterpret_cast<CtrlHack&> ( c );
	h.uparent = reinterpret_cast<CtrlHack*> ( &c );
	EXPECT_EQ ( sizeof ( lz::CtrlHack ), sizeof ( Upp::Ctrl ) );
	EXPECT_EQ ( c.GetParent(), &c );
	h.children = reinterpret_cast<CtrlHack*> ( &c );
	EXPECT_EQ ( c.GetFirstChild(), &c );
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
	EXPECT_EQ ( offsetof ( CtrlHack, WhenAction ), offsetof ( Ctrl, WhenAction ) );
#pragma GCC diagnostic pop
	h.uparent = nullptr;
	h.children = nullptr;

}


TEST ( CtrlHackTest, OwnedFlag )
{
	Ctrl c;
	lz::MarkAsOwned ( c );
	EXPECT_TRUE ( lz::IsOwned ( c ) );
	lz::MarkAsOwned ( c, false );
	EXPECT_FALSE ( lz::IsOwned ( c ) );
}

TEST ( CtrlHackTest, ReclaimPaddingSpace )
{
#ifdef CPU_64
	Ctrl c;
	lz::SetReserved( c, 0x12345678);
	EXPECT_EQ ( 0x12345678, lz::GetReserved(c) );
#endif
}

// Ctrl Relocate needs rework and redesign
//TEST ( CtrlHackTest, RolcationCtrlWithParentNoSibling )
//{
//	using lz::CtrlHack;
//	using lz::AsInt;
//	using lz::AsPtrOf;
//	Ctrl p;
//	Ctrl c1, c2;
//	char buff[sizeof ( Ctrl ) ];
//
//	p.Add ( c1 );
//	RawCopy ( buff, &c1 );
//	CtrlHack& h = *reinterpret_cast<CtrlHack*> ( &*buff );
//	h.DoPostRelocationAdjustment(AsPtrOf<const CtrlHack>(&c1));
//	EXPECT_EQ ( AsInt ( p.GetFirstChild() ), AsInt ( &*buff ) );
//	EXPECT_EQ ( h.prev_sibling, h.next_sibling );
//	EXPECT_EQ ( AsInt ( h.prev_sibling ), AsInt ( &*buff ) );
//}
//
//TEST ( CtrlHackTest, RolcationCtrlWithParentAndSiblings )
//{
//	using lz::CtrlHack;
//	using lz::AsInt;
//	Ctrl p;
//	Ctrl c1, c2;
//	char buff[sizeof ( Ctrl ) ];
//
//	p.Add ( c1 );
//	p.Add ( c2 );
//	RawCopy ( buff, &c1 );
//	CtrlHack& h = *reinterpret_cast<CtrlHack*> ( buff );
//	h.DoPostRelocationAdjustment(AsPtrOf<const CtrlHack>(&c1));
//	EXPECT_EQ ( AsInt ( p.GetFirstChild() ), AsInt ( &*buff ) );
//	EXPECT_EQ ( AsInt ( h.prev_sibling ), AsInt ( &c2 ) );
//	EXPECT_EQ ( AsInt ( h.next_sibling ), AsInt ( &c2 ) );
//	EXPECT_EQ ( AsInt ( h.prev_sibling->next_sibling ), AsInt ( &*buff ) );
//	EXPECT_EQ ( AsInt ( h.next_sibling->prev_sibling ), AsInt ( &*buff ) );
//}
//
//TEST ( CtrlHackTest, RolcationWithChildren )
//{
//	using lz::CtrlHack;
//	using lz::AsInt;
//	Ctrl p;
//	Ctrl c1, c2, c3;
//	char buff[sizeof ( Ctrl ) ];
//
//	p.Add ( c1 );
//	p.Add ( c2 );
//	p.Add ( c3 );
//
//	RawCopy ( buff, &p );
//	CtrlHack& h = *reinterpret_cast<CtrlHack*> ( &*buff );
//
//	h.DoPostRelocationAdjustment(&p);
//	EXPECT_EQ ( AsInt ( h.children->GetParent() ), AsInt ( &*buff ) );
//	EXPECT_EQ ( AsInt ( h.children->next_sibling->GetParent() ), AsInt ( &*buff ) );
//	EXPECT_EQ ( AsInt ( h.children->next_sibling->next_sibling->GetParent() ),
//				AsInt ( &*buff ) );
//	// make p consistent again so that it can be properly destructed
//	reinterpret_cast<CtrlHack*> ( &p )->DoPostRelocationAdjustment(&h);
//}
//
//TEST ( CtrlRelocatable, Ditto )
//{
//	EXPECT_TRUE ( lz::relocatable<lz::CtrlHack> );
//}
