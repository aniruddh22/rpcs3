#pragma once

#define NOMINMAX

#include <wx/string.h>

#include <wx/wx.h>
#include <wx/propdlg.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include <wx/wfstream.h>
#include <wx/dir.h>
#include <wx/generic/progdlgg.h>
#include <wx/spinctrl.h>
#include <wx/filepicker.h>

#include <wx/wxprec.h>

#define uint unsigned int

#define u8  unsigned __int8
#define u16 unsigned __int16
#define u32 unsigned __int32
#define u64 unsigned __int64

#define s8  signed __int8
#define s16 signed __int16
#define s32 signed __int32
#define s64 signed __int64

union u128
{
	struct
	{
		u64 hi;
		u64 lo;
	};

	u64 _u64[2];
	u32 _u32[4];
	u16 _u16[8];
	u8  _u8[16];

	operator u64() const { return _u64[0]; }
	operator u32() const { return _u32[0]; }
	operator u16() const { return _u16[0]; }
	operator u8()  const { return _u8[0];  }

	operator bool() const { return _u64[0] != 0 || _u64[1] != 0; }

	static u128 From64( u64 src )
	{
		u128 ret = {0, src};
		return ret;
	}

	static u128 From32( u32 src )
	{
		u128 ret;
		ret._u32[0] = src;
		ret._u32[1] = 0;
		ret._u32[2] = 0;
		ret._u32[3] = 0;
		return ret;
	}

	bool operator == ( const u128& right ) const
	{
		return (lo == right.lo) && (hi == right.hi);
	}

	bool operator != ( const u128& right ) const
	{
		return (lo != right.lo) || (hi != right.hi);
	}
};

union s128
{
	struct
	{
		s64 hi;
		s64 lo;
	};

	u64 _i64[2];
	u32 _i32[4];
	u16 _i16[8];
	u8  _i8[16];

	operator s64() const { return _i64[0]; }
	operator s32() const { return _i32[0]; }
	operator s16() const { return _i16[0]; }
	operator s8()  const { return _i8[0];  }

	operator bool() const { return _i64[0] != 0 || _i64[1] != 0; }

	static s128 From64( u64 src )
	{
		s128 ret = {src, 0};
		return ret;
	}

	static s128 From32( u32 src )
	{
		s128 ret;
		ret._i32[0] = src;
		ret._i32[1] = 0;
		ret.hi = 0;
		return ret;
	}

	bool operator == ( const s128& right ) const
	{
		return (lo == right.lo) && (hi == right.hi);
	}

	bool operator != ( const s128& right ) const
	{
		return (lo != right.lo) || (hi != right.hi);
	}
};

#include <emmintrin.h>

//TODO: SSE style
/*
struct u128
{
	__m128 m_val;

	u128 GetValue128()
	{
		u128 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}

	u64 GetValue64()
	{
		u64 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}

	u32 GetValue32()
	{
		u32 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}

	u16 GetValue16()
	{
		u16 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}

	u8 GetValue8()
	{
		u8 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}
};
*/

template<typename T>
static void safe_realloc(T* ptr, uint new_size)
{
	if(new_size == 0) return;
	ptr = (T*)((ptr == NULL) ? malloc(new_size * sizeof(T)) : realloc(ptr, new_size * sizeof(T)));
}

#define safe_delete(x) {free(x);(x)=NULL;}

enum Status
{
	Runned,
	Paused,
	Stopped,
};

#include "Utilities/Thread.h"
#include "Utilities/Array.h"
#include "Utilities/Timer.h"
#include "Utilities/IdManager.h"

#include "Ini.h"
#include "Gui/FrameBase.h"
#include "Gui/ConLog.h"
#include "Emu/System.h"
#include "Emu/Memory/Memory.h"
#include "rpcs3.h"

#define _PRGNAME_ "RPCS3"
#define _PRGVER_ "0.0.0.2"