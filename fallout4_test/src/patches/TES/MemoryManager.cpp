#include "MemoryManager.h"

LPVOID MemAlloc(UINT64 Size, UINT32 Alignment = 0, BOOL Aligned = FALSE, BOOL Zeroed = FALSE)
{
	ProfileCounterInc("Alloc Count");
	ProfileCounterAdd("Byte Count", Size);
	ProfileTimer("Time Spent Allocating");

#if FALLOUT4_USE_VTUNE
	__itt_heap_allocate_begin(ITT_AllocateCallback, Size, Zeroed ? 1 : 0);
#endif

	// If the caller doesn't care, force 4 byte aligns as a minimum
	if (!Aligned)
		Alignment = 4;

	if (Size <= 0)
	{
		Size = 1;
		Alignment = 2;
	}

	AssertMsg(Alignment != 0 && Alignment % 2 == 0, "Alignment is fucked");

	// Must be a power of 2, round it up if needed
	if ((Alignment & (Alignment - 1)) != 0)
	{
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

#if FALLOUT4_USE_PAGE_HEAP
	LPVOID ptr = VirtualAlloc(NULL, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	LPVOID ptr = scalable_aligned_malloc(Size, Alignment);

	if (ptr && Zeroed)
		memset(ptr, 0, Size);
#endif

#if FALLOUT4_USE_VTUNE
	__itt_heap_allocate_end(ITT_AllocateCallback, &ptr, Size, Zeroed ? 1 : 0);
#endif

	return ptr;
}

VOID MemFree(LPVOID Memory, BOOL Aligned = FALSE)
{
	ProfileCounterInc("Free Count");
	ProfileTimer("Time Spent Freeing");

	if (!Memory)
		return;

#if FALLOUT4_USE_VTUNE
	__itt_heap_free_begin(ITT_FreeCallback, Memory);
#endif

#if FALLOUT4_USE_PAGE_HEAP
	VirtualFree(Memory, 0, MEM_RELEASE);
#else
	scalable_aligned_free(Memory);
#endif

#if FALLOUT4_USE_VTUNE
	__itt_heap_free_end(ITT_FreeCallback, Memory);
#endif
}

UINT64 MemSize(LPVOID Memory)
{
#if FALLOUT4_USE_VTUNE
	__itt_heap_internal_access_begin();
#endif

#if FALLOUT4_USE_PAGE_HEAP
	MEMORY_BASIC_INFORMATION info;
	VirtualQuery(Memory, &info, sizeof(MEMORY_BASIC_INFORMATION));

	UINT64 result = (UINT64)info.RegionSize;
#else
	UINT64 result = (UINT64)scalable_msize(Memory);
#endif

#if FALLOUT4_USE_VTUNE
	__itt_heap_internal_access_end();
#endif

	return result;
}

//
// VS2015 CRT hijacked functions
//
LPVOID hk_calloc(size_t Count, size_t Size)
{
	// The allocated memory is always zeroed
	return MemAlloc(Count * Size, 0, FALSE, TRUE);
}

LPVOID hk_malloc(size_t Size)
{
	return MemAlloc(Size);
}

LPVOID hk_aligned_malloc(size_t Size, size_t Alignment)
{
	return MemAlloc(Size, Alignment, TRUE);
}

LPVOID hk_realloc(LPVOID Memory, size_t Size)
{
	LPVOID newMemory = NULL;

	if (Size > 0)
	{
		// Recalloc behaves like calloc if there's no existing allocation. Realloc doesn't. Zero it anyway.
		newMemory = MemAlloc(Size, 0, FALSE, TRUE);

		if (Memory)
			memcpy(newMemory, Memory, std::min(Size, MemSize(Memory)));
	}

	MemFree(Memory);
	return newMemory;
}

LPVOID hk_recalloc(LPVOID Memory, size_t Count, size_t Size)
{
	return hk_realloc(Memory, Count * Size);
}

VOID hk_free(LPVOID Block)
{
	MemFree(Block);
}

VOID hk_aligned_free(LPVOID Block)
{
	MemFree(Block, TRUE);
}

UINT64 hk_msize(LPVOID Block)
{
	return MemSize(Block);
}

LPSTR hk_strdup(LPCSTR str1)
{
	size_t len = (strlen(str1) + 1);
	return (LPSTR)memcpy(hk_malloc(len), str1, len);
}

//
// Internal engine heap allocators backed by VirtualAlloc()
//
LPVOID MemoryManager::Allocate(MemoryManager *Manager, UINT64 Size, UINT32 Alignment, BOOL Aligned)
{
	return MemAlloc(Size, Alignment, Aligned, TRUE);
}

VOID MemoryManager::Deallocate(MemoryManager *Manager, LPVOID Memory, BOOL Aligned)
{
	MemFree(Memory, Aligned);
}

UINT64 MemoryManager::Size(MemoryManager *Manager, LPVOID Memory)
{
	return MemSize(Memory);
}

LPVOID ScrapHeap::Allocate(UINT64 Size, UINT32 Alignment)
{
	if (Size > g_ScrapSize)
		return NULL;

	return MemAlloc(Size, Alignment, Alignment != 0);
}

VOID ScrapHeap::Deallocate(LPVOID Memory)
{
	MemFree(Memory);
}

VOID PatchMemory(VOID)
{
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