//////////////////////////////////////////
/*
* Copyright (c) 2020-2022 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#pragma once

#include "CommInc.h"

struct IMemory_v1_6 
{
	static LPVOID FIXAPI MemAlloc(size_t Size, UINT32 Alignment = 0, bool Aligned = FALSE, bool Zeroed = FALSE);
	static VOID FIXAPI MemFree(LPVOID Memory, bool Aligned = FALSE);
	static size_t FIXAPI MemSize(LPVOID Memory);
	static LPSTR FIXAPI StrDup(LPCSTR string);
};

#if 0

#define __MEMALIGN__ 16
#define __IMEM_VER 2

class IMemory {
public:
	typedef std::function<LPVOID(size_t)> MallocT;
	typedef std::function<VOID(LPVOID)> FreeT;
	typedef std::function<size_t(LPVOID)> MSizeT;

	enum {
		MEM_MODE_DEFAULT = 0,
		MEM_MODE_ALIGNED,
		MEM_MODE_TBB,
		MEM_MODE_TBB_ALIGNED,
		MEM_MODE_MIN = MEM_MODE_DEFAULT,
		MEM_MODE_MAX = MEM_MODE_TBB_ALIGNED,
	};
private:
	DWORD _mode;
	MallocT _Allocate;
	FreeT _Deallocate;
	MSizeT _Size;
public:
	NOTHROW LPVOID Allocate(size_t size) const;
	INLINE NOTHROW VOID Free(LPVOID ptr) const { if (ptr) _Deallocate(ptr); }
	INLINE NOTHROW size_t Size(LPVOID ptr) const { return ptr ? _Size(ptr) : 0; }

	INLINE NOTHROW LPVOID GetPointerAligned(LPVOID ptr) const { return ((LPVOID*)ptr)[-1]; }
	NOTHROW LPVOID AllocateAligned(size_t size, size_t alignment = __MEMALIGN__) const;
	INLINE NOTHROW VOID FreeAligned(LPVOID ptr) const { if (ptr) _Deallocate(GetPointerAligned(ptr)); }
	INLINE NOTHROW size_t SizeAligned(LPVOID ptr) const { return ptr ? _Size(GetPointerAligned(ptr)) : 0; }
public:
	IMemory(INT mode = MEM_MODE_DEFAULT);
public:
	static IMemory* GetInstance(VOID);
	static IMemory* CreateInstance(DWORD mode = MEM_MODE_DEFAULT);
};

#define TESPOOLMEMORY IMemory::GetInstance()

LPVOID	FIXAPI QMemAlloc(UINT64 Size, BOOL Zeroed = FALSE, BOOL Aligned = FALSE);
VOID	FIXAPI QMemFree(LPVOID Memory, BOOL Aligned = FALSE);
UINT64	FIXAPI QMemSize(LPVOID Memory, BOOL Aligned = FALSE);
LPSTR	FIXAPI QStrDup(LPCSTR string);

#endif

#define QMemAlloc IMemory_v1_6::MemAlloc
#define QMemFree IMemory_v1_6::MemFree
#define QMemSize IMemory_v1_6::MemSize
#define QStrDup IMemory_v1_6::StrDup
#define QMemCopy memcpy
