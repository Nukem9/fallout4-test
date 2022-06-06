//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
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

#include "StdAfx.h"

/*
Uses oneTBB (oneAPI Threading Building Blocks (oneTBB))
URL: https://github.com/oneapi-src/oneTBB/tree/a803f276186fa2c286a357207832112265b448e4

To increase the performance of the application, the functions are replaced with tbb
*/

LPVOID FIXAPI IMemory_v1_6::MemAlloc(size_t Size, UINT32 Alignment, bool Aligned, bool Zeroed)
{
	// If the caller doesn't care, force 4 byte aligns as a minimum
	if (!Aligned)
		Alignment = 4;

	if (Size <= 0) {
		Size = 1;
		Alignment = 2;
	}

	AssertMsg(Alignment != 0 && Alignment % 2 == 0, "Alignment is fucked");

	// Must be a power of 2, round it up if needed
	if ((Alignment & (Alignment - 1)) != 0) {
		Alignment--;
		Alignment |= Alignment >> 1;
		Alignment |= Alignment >> 2;
		Alignment |= Alignment >> 4;
		Alignment |= Alignment >> 8;
		Alignment |= Alignment >> 16;
		Alignment++;
	}

	// Size must be a multiple of alignment, round up to nearest
	if ((Size % Alignment) != 0)
		Size = ((Size + Alignment - 1) / Alignment) * Alignment;

	LPVOID ptr = scalable_aligned_malloc(Size, Alignment);

	if (ptr && Zeroed)
		memset(ptr, 0, Size);

	return ptr;
}

VOID FIXAPI IMemory_v1_6::MemFree(LPVOID Memory, bool Aligned)
{
	if (!Memory)
		return;

	scalable_aligned_free(Memory);
}

size_t FIXAPI IMemory_v1_6::MemSize(LPVOID Memory)
{
	return (size_t)scalable_msize(Memory);
}

LPSTR FIXAPI IMemory_v1_6::StrDup(LPCSTR string)
{
	size_t len = (strlen(string) + 1);
	return (LPSTR)memcpy(MemAlloc(len), string, len);
}

#if 0

#define ALIGN(n, a)	(((n) + (a) - 1) & ~((a) - 1))

IMemory* g_PoolAllocateMemory = NULL;

IMemory::IMemory(INT mode) {
	_mode = (mode >= MEM_MODE_MIN && mode <= MEM_MODE_MAX) ? mode : MEM_MODE_DEFAULT;

	switch (_mode)
	{
	case MEM_MODE_ALIGNED:
		_Allocate = [](size_t size) -> LPVOID { return _aligned_malloc(size, __MEMALIGN__); };
		_Deallocate = [](LPVOID ptr) { _aligned_free(ptr); };
		_Size = [](LPVOID ptr) -> size_t { return _aligned_msize(ptr, __MEMALIGN__, 0); };
		break;
	case MEM_MODE_TBB:
		_Allocate = scalable_malloc;
		_Deallocate = scalable_free;
		_Size = scalable_msize;
		break;
	case MEM_MODE_TBB_ALIGNED:
		_Allocate = [](size_t size) -> LPVOID { return scalable_aligned_malloc(size, __MEMALIGN__); };
		_Deallocate = [](LPVOID ptr) { scalable_aligned_free(ptr); };
		_Size = [](LPVOID ptr) -> size_t { return scalable_msize(ptr); };
		break;
	default:
		_Allocate = malloc;
		_Deallocate = free;
		_Size = _msize;
		break;
	}
}

LPVOID IMemory::Allocate(size_t size) const {
	// Avoiding null returns
	if (!size)
		size = 1;

	LPVOID ptr = _Allocate(size);
	AssertMsgVa(ptr, "Allocate return null pointer\nReturn Address: %X", (uintptr_t)_ReturnAddress());

	return ptr;
}

LPVOID IMemory::AllocateAligned(size_t size, size_t alignment) const {
	// Avoiding null returns
	if (!size)
		size = 1;

	// If the caller doesn't care, force 16 byte aligns as a minimum
	if (!alignment)
		alignment = __MEMALIGN__;
	
	AssertMsg(alignment % 2 == 0, "Alignment is fucked");

	// Size must be a multiple of alignment, round up to nearest
	if ((size % alignment) != 0)
		size = ((size + alignment - 1) / alignment) * alignment;

	LPVOID ptr = _Allocate(sizeof(LPVOID) + alignment - 1 + size);
	AssertMsgVa(ptr, "Allocate return null pointer\nReturn Address: %X", (uintptr_t)_ReturnAddress());

	LPVOID orig_ptr = ptr;
	ptr = (LPVOID)ALIGN((uintptr_t)ptr + sizeof(LPVOID), alignment);
	((LPVOID*)ptr)[-1] = orig_ptr;

	return ptr;
}

IMemory* IMemory::GetInstance(VOID) {
	return g_PoolAllocateMemory;
}

IMemory* IMemory::CreateInstance(DWORD mode) {
	if (g_PoolAllocateMemory)
		return g_PoolAllocateMemory;

	g_PoolAllocateMemory = new IMemory(mode);
	return g_PoolAllocateMemory;
}

//////////////////////////////////////////

LPVOID FIXAPI QMemAlloc(UINT64 Size, BOOL Zeroed, BOOL Aligned) {
#if FALLOUT4_MEMORY_ALIGNED_INT
	// only aligned memory on 16
	LPVOID Ret = g_PoolAllocateMemory->AllocateAligned(Size);
#else
	LPVOID Ret;
	if (Aligned)
		Ret = g_PoolAllocateMemory->AllocateAligned(Size);
	else
		Ret = g_PoolAllocateMemory->Allocate(Size);
#endif // !FALLOUT4_MEMORY_ALIGNED

	if (Ret && Zeroed)
		memset(Ret, 0, Size);

	return Ret;
}

VOID FIXAPI QMemFree(LPVOID Memory, BOOL Aligned) {
#if FALLOUT4_MEMORY_ALIGNED_INT
	g_PoolAllocateMemory->FreeAligned(Memory);
#else
	if (Aligned)
		g_PoolAllocateMemory->FreeAligned(Memory);
	else
		g_PoolAllocateMemory->Free(Memory);
#endif // !FALLOUT4_MEMORY_ALIGNED
}

UINT64 FIXAPI QMemSize(LPVOID Memory, BOOL Aligned) {
#if FALLOUT4_MEMORY_ALIGNED_INT
	return g_PoolAllocateMemory->SizeAligned(Memory);
#else
	if (Aligned)
		return g_PoolAllocateMemory->SizeAligned(Memory);
	else
		return g_PoolAllocateMemory->Size(Memory);
#endif // !FALLOUT4_MEMORY_ALIGNED
}

LPSTR FIXAPI QStrDup(LPCSTR string) {
	size_t len = (strlen(string) + 1);
	return (LPSTR)memcpy(QMemAlloc(len), (LPVOID)string, len);
}

#endif