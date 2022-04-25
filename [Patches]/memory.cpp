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

#include "..\[Common]\StdAfx.h"

#define MEM_THRESHOLD 4294967296

LPVOID hk_calloc(size_t Count, size_t Size) { return QMemAlloc(Count * Size, TRUE); }
LPVOID hk_malloc(size_t Size) { return QMemAlloc(Size); }
LPVOID hk_realloc(LPVOID Memory, size_t Size) {
	LPVOID newMemory = NULL;

	if (Size > 0) {
		// Recalloc behaves like calloc if there's no existing allocation. Realloc doesn't. Zero it anyway.
		newMemory = QMemAlloc(Size, TRUE);

		if (Memory)
			memcpy(newMemory, Memory, std::min(Size, QMemSize(Memory)));
	}

	QMemFree(Memory);
	return newMemory;
}
LPVOID hk_recalloc(LPVOID Memory, size_t Count, size_t Size) { return hk_realloc(Memory, Count * Size); }
VOID hk_free(LPVOID Memory) { QMemFree(Memory); }
size_t hk_msize(LPVOID Memory) { return QMemSize(Memory); }

LPVOID hk_aligned_malloc(size_t Size, size_t Alignment) { return QMemAlloc(Size, FALSE, TRUE); }
VOID hk_aligned_free(LPVOID Memory) { QMemFree(Memory, TRUE); }
size_t hk_aligned_msize(LPVOID Memory, size_t Alignment, size_t Offset) { return QMemSize(Memory, TRUE); }

DWORD64 FIXAPI Sys_GetPhysicalMemory(VOID) {
	MEMORYSTATUSEX statex = { 0 };
	statex.dwLength = sizeof(MEMORYSTATUSEX);
	if (!GlobalMemoryStatusEx(&statex))
		return 0;
	return statex.ullTotalPhys;
}

BOOL FIXAPI Sys_LowPhysicalMemory(VOID) { return (Sys_GetPhysicalMemory() > MEM_THRESHOLD) ? TRUE : FALSE; }

VOID FIXAPI Fix_PatchMemory(VOID) {
	AssertMsg(Sys_LowPhysicalMemory(), "Not enough memory to run the program");

	auto GB = Sys_GetPhysicalMemory() / 1073741824;
	g_ScrapSize = (GB > 4) ? 0x8000000 : 0x4000000;
	g_bhkMemSize = (GB > 8) ? 0x80000000 : 0x40000000;

	_F4CKMSG_FMT("//	Total memory %u Gb", (DWORD)GB);
	_F4CKMSG_FMT("//	ScrapHeap set %u Mb", DWORD(g_ScrapSize / 1048576));

	if (g_bhkMemSize >= 0x40000000)
		_F4CKMSG_FMT("//	bhkThreadMemorySource set %u Gb", DWORD(g_bhkMemSize / 1073741824));
	else
		_F4CKMSG_FMT("//	bhkThreadMemorySource set %u Mb", DWORD(g_bhkMemSize / 1048576));

	PatchIAT(hk_calloc, "MSVCR110.dll", "calloc");
	PatchIAT(hk_calloc, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "calloc");
	PatchIAT(hk_malloc, "MSVCR110.dll", "malloc");
	PatchIAT(hk_malloc, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "malloc");
	PatchIAT(hk_aligned_malloc, "MSVCR110.dll", "_aligned_malloc");
	PatchIAT(hk_aligned_malloc, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_aligned_malloc");
	PatchIAT(hk_realloc, "MSVCR110.dll", "realloc");
	PatchIAT(hk_recalloc, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_recalloc");
	PatchIAT(hk_free, "MSVCR110.dll", "free");
	PatchIAT(hk_free, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "free");
	PatchIAT(hk_aligned_free, "MSVCR110.dll", "_aligned_free");
	PatchIAT(hk_aligned_free, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_aligned_free");
	PatchIAT(hk_msize, "MSVCR110.dll", "_msize");
	PatchIAT(hk_msize, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_msize");
	PatchIAT(hk_aligned_msize, "MSVCR110.dll", "_aligned_msize");
	PatchIAT(hk_aligned_msize, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "_aligned_msize");
	PatchIAT(memcpy_s, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "memcpy_s");
	PatchIAT(memcpy_s, "MSVCR110.dll", "memcpy_s");
	PatchIAT(memcpy_s, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "memmove_s");
	PatchIAT(memcpy_s, "MSVCR110.dll", "memmove_s");
	PatchIAT(memcmp, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "memcmp");
	PatchIAT(memcmp, "MSVCR110.dll", "memcmp");
	PatchIAT(memset, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "memset");
	PatchIAT(memset, "MSVCR110.dll", "memset");
	PatchIAT(QStrDup, "MSVCR110.dll", "_strdup");
	PatchIAT(QStrDup, "API-MS-WIN-CRT-STRING-L1-1-0.DLL", "_strdup");
	PatchIAT(QMemCopy, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "memcpy");
	PatchIAT(QMemCopy, "MSVCR110.dll", "memcpy");
	PatchIAT(QMemCopy, "API-MS-WIN-CRT-HEAP-L1-1-0.DLL", "memmove");
	PatchIAT(QMemCopy, "MSVCR110.dll", "memmove");
}