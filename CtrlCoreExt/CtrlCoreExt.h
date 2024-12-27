#ifndef _CtrlCoreExt_CtrlCoreExt_h_
#define _CtrlCoreExt_CtrlCoreExt_h_

#include <CtrlCore/CtrlCore.h>
#include <concepts>
#include <CoreExt/CoreExt.h>

#define IMAGECLASS CtrlExtImg
#define IMAGEFILE <CtrlCoreExt/ext.iml>
#include <Draw/iml_header.h>

namespace lz{

#include "bits/using_upp_names.h"
#include "bits/FakeCtrl.h"

template <class T>
concept CtrlClass = std::derived_from<T, Ctrl>;

// we aim to make use of the padding in Upp::Ctrl
// on 64bit CPU (which is mainstream atm)
//
struct CtrlHack : public Pte<CtrlHack>
{
	typedef CtrlHack Ctrl;
	struct Frame : Upp::Moveable<Frame>
	{
		CtrlFrame *frame;
		Rect16     view;
	};


	Frame        frame;
	Upp::Ctrl::LogPos       pos;//8
	Rect16       rect; //8

	union{
		Ctrl		*uparent;
		Ctrl		*utop;
	};

	Ctrl        *prev_sibling = nullptr;
	Ctrl        *next_sibling = nullptr;
	Ctrl        *children = nullptr;
	PackedData   attrs;

	byte         overpaint;
	bool         unicode:1;			//9
	bool         fullrefresh:1;		//10
	bool         transparent:1;     //11
	bool         visible:1;			//12
	bool         enabled:1;			//13
	bool         wantfocus:1;		//14
	bool         initfocus:1;		//15
	bool         activepopup:1;		//16
	bool         editable:1;		//17
	bool         modify:1;			//18
	bool         ignoremouse:1;		//19
	bool         inframe:1;			//20
	bool         inloop:1;			//21
	bool         isopen:1;			//22
	bool         popup:1;			//23
	bool         popupgrab:1;		//24
	byte         backpaint:2;		//25
	bool         akv:1;				//27
	bool         destroying:1;		//28
	bool         layout_id_literal:1; // info_ptr points to layout char * literal, no heap involved
	bool         multi_frame:1; // there is more than single frame, they are stored in heap
	bool         top:1;				//31
	bool         megarect:1;        //32
	// below are added fields
	
#ifdef CPU_64
	union{
		int32 padding;
		struct{
			byte			skipped[3];
			bool			owned:1;           //33
			bool			paintAsFocused:1;
		};
	};
	int32 GetReserved()const{ return padding; }
	void SetReserved(int32 v){ padding = v; }
#endif
	// end of block for added fields

	Upp::Event<>          WhenAction;


	Upp::Ctrl::LogPos      GetPos() const                           { return pos; }

	void        Shutdown()
	{
		destroying = true;
	}

	bool        IsShutdown() const
	{
		return destroying;
	}

//#ifdef CPU_64
//	int32	GetReserved()const{ return reserved; }
//	void	SetReserved(int32 v){ reserved = v;  }
//
//#endif


	void		SetOwned ( bool b = true )
	{
		owned = b;
	}

	bool		IsOwned() const
	{
		return owned;
	}

	void		SetPaintAsFocused( bool b = true)
	{
		paintAsFocused = b;
	}
	
	bool		IsPaintAsFocused()const
	{
		return paintAsFocused;
	}
	// the hack is very likely to work but is not fool-proof.
	//
	// changes in Upp::Ctrl class definition may break it (not very likely though).
	//
	// If GridEz with owned child Ctrls behave erratically, check
	// the definition of Upp::Ctrl (in <CtrlCore/CtrlCore.h>) for
	// possible reason.
	static void ValidateHack()
	{
		Upp::Ctrl c, d;

		CtrlHack &e = *reinterpret_cast<CtrlHack*> ( &c ),
					  &f = *reinterpret_cast<CtrlHack*> ( &d );

		c.Shutdown();
		f.SetOwned();
		e.SetOwned ( false );
		static_assert( sizeof(c) == sizeof(Ctrl) );
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
		static_assert (
			offsetof ( CtrlHack, WhenAction ) == offsetof ( Ctrl, WhenAction ),
			"Unexpected difference in class/struct layout"
		);
#pragma GCC diagnostic pop
//		ASSERT ( offsetof ( CtrlHack, WhenAction ) == offsetof ( Ctrl, WhenAction ) );
		ASSERT ( e.IsShutdown() && !f.IsShutdown() );
		ASSERT ( e.IsOwned() != f.IsOwned() );
	}

	virtual ~CtrlHack() { }

	CtrlHack()
	{
		ValidateHack();
	}
	
	void DoPostRelocationAdjustment(const CtrlHack * old)noexcept;
	
	CtrlHack* GetParent(){ return top ? nullptr : uparent; }
	
	bool IsTheSingleChild()const;
	
};

//template <class T>
//inline T& MarkAsOwned ( T& ctrl, bool owned = true )
inline auto& MarkAsOwned ( CtrlClass auto& ctrl, bool owned = true )
{
#ifdef _DEBUG
	static CtrlHack h;
#endif

	reinterpret_cast<CtrlHack*> ( &ctrl )->SetOwned ( owned );
	return ctrl;
}

//template <class T>
inline auto MarkAsOwned ( CtrlClass auto* ctrl, bool owned = true )
{
	return &MarkAsOwned ( *ctrl, owned );
}


inline bool IsOwned ( CtrlClass auto& ctrl )
{
//	static_assert ( std::is_base_of<Upp::Ctrl, T>::value || std::is_same<Ctrl, T>::value,
//					"Expect Ctrl or its derivative" );
	return reinterpret_cast<CtrlHack*> ( &ctrl )->IsOwned();
}

//template <class T>
inline bool IsOwned ( CtrlClass auto* ctrl )
{
	return IsOwned ( *ctrl );
}

inline auto& SetPaintAsFocused ( CtrlClass auto& ctrl, bool paf = true )
{
#ifdef _DEBUG
	static CtrlHack h;
#endif

	reinterpret_cast<CtrlHack*> ( &ctrl )->SetPaintAsFocused(paf);
	return ctrl;
}

//template <class T>
inline auto SetPaintAsFocused ( CtrlClass auto* ctrl, bool paf = true )
{
	return &SetPaintAsFocused ( *ctrl, paf );
}


inline bool IsPaintAsFocused ( CtrlClass auto& ctrl )
{
	return reinterpret_cast<CtrlHack*> ( &ctrl )->IsPaintAsFocused();
}

inline bool IsPaintAsFocused ( CtrlClass auto* ctrl )
{
	return reinterpret_cast<CtrlHack*> ( ctrl )->IsPaintAsFocused();
}

#ifdef CPU_64
inline int32 GetReserved( const CtrlClass auto& ctrl)
{
	return reinterpret_cast<const CtrlHack&> ( ctrl ).GetReserved();
}

inline int32 GetReserved( const CtrlClass auto* ctrl)
{
	return GetReserved( *ctrl );
}

inline void SetReserved( CtrlClass auto& ctrl, int32 value)
{
	reinterpret_cast<CtrlHack&> ( ctrl ).SetReserved( value );
}

inline void SetReserved( CtrlClass auto* ctrl, int32 value)
{
	SetReserved( *ctrl, value );
}

#endif

}//eons lz

#endif
