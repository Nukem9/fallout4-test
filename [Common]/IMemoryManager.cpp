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

LPVOID MemoryManager::Allocate(MemoryManager *Manager, UINT64 Size, UINT32 Alignment, BOOL Aligned) {
	return QMemAlloc(Size, Alignment, Aligned, TRUE);
}

VOID MemoryManager::Deallocate(MemoryManager *Manager, LPVOID Memory, BOOL Aligned) {
	QMemFree(Memory, Aligned);
}

UINT64 MemoryManager::Size(MemoryManager *Manager, LPVOID Memory) {
	return QMemSize(Memory);
}

LPVOID ScrapHeap::Allocate(UINT64 Size, UINT32 Alignment) {
	if (Size > g_ScrapSize)
		return NULL; 

	return QMemAlloc(Size, Alignment, Alignment != 0);
}

VOID ScrapHeap::Deallocate(LPVOID Memory) {
	QMemFree(Memory);
}