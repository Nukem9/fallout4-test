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

#include "..\StdAfx.h"

/*
Uses oneTBB (oneAPI Threading Building Blocks (oneTBB))
URL: https://github.com/oneapi-src/oneTBB/tree/a803f276186fa2c286a357207832112265b448e4

To increase the performance of the application, the functions are replaced with tbb
*/

#define MEM_THRESHOLD 2147483648

/*
==================
MemAlloc
==================
*/
LPVOID MemAlloc(UINT64 Size, UINT32 Alignment, BOOL Aligned, BOOL Zeroed) {
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

/*
==================
MemFree
==================
*/
VOID MemFree(LPVOID Memory, BOOL Aligned) {
	if (!Memory)
		return;

	scalable_aligned_free(Memory);
}

/*
==================
MemSize
==================
*/
UINT64 MemSize(LPVOID Memory) {
	return (UINT64)scalable_msize(Memory);
}

/*
==================
hk_calloc

Replacement calloc
==================
*/
LPVOID hk_calloc(size_t Count, size_t Size) {
	// The allocated memory is always zeroed
	return MemAlloc(Count * Size, 0, FALSE, TRUE);
}

/*
==================
hk_malloc

Replacement malloc
==================
*/
LPVOID hk_malloc(size_t Size) {
	return MemAlloc(Size);
}

/*
==================
hk_aligned_malloc

Replacement _aligned_malloc
==================
*/
LPVOID hk_aligned_malloc(size_t Size, size_t Alignment) {
	return MemAlloc(Size, Alignment, TRUE);
}

/*
==================
hk_realloc

Replacement realloc
==================
*/
LPVOID hk_realloc(LPVOID Memory, size_t Size) {
	LPVOID newMemory = NULL;

	if (Size > 0) {
		// Recalloc behaves like calloc if there's no existing allocation. Realloc doesn't. Zero it anyway.
		newMemory = MemAlloc(Size, 0, FALSE, TRUE);

		if (Memory)
			memcpy(newMemory, Memory, std::min(Size, MemSize(Memory)));
	}

	MemFree(Memory);
	return newMemory;
}

/*
==================
hk_realloc

Replacement recalloc
==================
*/
LPVOID hk_recalloc(LPVOID Memory, size_t Count, size_t Size) {
	return hk_realloc(Memory, Count * Size);
}

/*
==================
hk_free

Replacement free
==================
*/
VOID hk_free(LPVOID Block) {
	MemFree(Block);
}

/*
==================
hk_aligned_free

Replacement _aligned_free
==================
*/
VOID hk_aligned_free(LPVOID Block) {
	MemFree(Block, TRUE);
}

/*
==================
hk_msize

Replacement _msize
==================
*/
UINT64 hk_msize(LPVOID Block) {
	return MemSize(Block);
}

/*
==================
hk_Sleep

Replacement WINAPI Sleep
==================
*/
LPSTR hk_strdup(LPCSTR str1) {
	size_t len = (strlen(str1) + 1);
	return (LPSTR)memcpy(hk_malloc(len), str1, len);
}

//
// Internal engine heap allocators backed by VirtualAlloc()
//
LPVOID MemoryManager::Allocate(MemoryManager *Manager, UINT64 Size, UINT32 Alignment, BOOL Aligned) {
	return MemAlloc(Size, Alignment, Aligned, TRUE);
}

VOID MemoryManager::Deallocate(MemoryManager *Manager, LPVOID Memory, BOOL Aligned) {
	MemFree(Memory, Aligned);
}

UINT64 MemoryManager::Size(MemoryManager *Manager, LPVOID Memory) {
	return MemSize(Memory);
}

LPVOID ScrapHeap::Allocate(UINT64 Size, UINT32 Alignment) {
	if (Size > g_ScrapSize)
		return NULL; 

	return MemAlloc(Size, Alignment, Alignment != 0);
}

VOID ScrapHeap::Deallocate(LPVOID Memory) {
	MemFree(Memory);
}

/*
==================
Sys_LowPhysicalMemory
==================
*/
DWORD64 FIXAPI Sys_GetPhysicalMemory(VOID) {
	MEMORYSTATUSEX statex;
	if (!GlobalMemoryStatusEx(&statex))
		return 0;
	return statex.ullTotalPhys;
}

/*
==================
Sys_LowPhysicalMemory
==================
*/
BOOL FIXAPI Sys_LowPhysicalMemory(VOID)  {
	return (Sys_GetPhysicalMemory() < MEM_THRESHOLD) ? TRUE : FALSE;
}

/*
==================
Fix_PatchMemory

Implements the code in the process
Changes memory allocation functions, but not all of them
==================
*/
VOID FIXAPI Fix_PatchMemory(VOID) {
	AssertMsg(Sys_LowPhysicalMemory(), "Not enough memory to run the program")

	auto GB = Sys_GetPhysicalMemory() / (1024 * 1024 * 1024);
	g_ScrapSize = (GB > 4) ? 0x8000000 : 0x4000000;
	g_bhkMemSize = (GB > 8) ? 0x80000000 : 0x40000000;

	// Turning on large pages (deprecated)
	scalable_allocation_mode(TBBMALLOC_USE_HUGE_PAGES, 1);

	PatchIAT(hk_calloc, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "calloc");
	PatchIAT(hk_malloc, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "malloc");
	PatchIAT(hk_aligned_malloc, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_aligned_malloc");
	PatchIAT(hk_recalloc, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_recalloc");
	PatchIAT(hk_free, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "free");
	PatchIAT(hk_aligned_free, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_aligned_free");
	PatchIAT(hk_msize, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_msize");
	PatchIAT(hk_strdup, "API-MS-WIN-CRT-STRING-L1-1-0.DLL", "_strdup");

	PatchIAT(hk_calloc, "MSVCR110.dll", "calloc");
	PatchIAT(hk_malloc, "MSVCR110.dll", "malloc");
	PatchIAT(hk_aligned_malloc, "MSVCR110.dll", "_aligned_malloc");
	PatchIAT(hk_realloc, "MSVCR110.dll", "realloc");
	PatchIAT(hk_free, "MSVCR110.dll", "free");
	PatchIAT(hk_aligned_free, "MSVCR110.dll", "_aligned_free");
	PatchIAT(hk_msize, "MSVCR110.dll", "_msize");
	PatchIAT(hk_strdup, "MSVCR110.dll", "_strdup");
}