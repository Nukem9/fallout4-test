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

#pragma once

#include "../common.h"

/*
==================
class MemoryManager

Main
==================
*/
class MemoryManager
{
private:
	// We do not describe constructors and destructors
	// The class is just a shell

	MemoryManager(VOID) = default;
	~MemoryManager(VOID) = default;
public:
	static LPVOID Allocate(MemoryManager *Manager, UINT64 Size, UINT32 Alignment, BOOL Aligned);
	static VOID   Deallocate(MemoryManager *Manager, LPVOID Memory, BOOL Aligned);
	static UINT64 Size(MemoryManager *Manager, LPVOID Memory);
};

/*
==================
class ScrapHeap

For small things (maximum 32, 64, 128 Mbytes size)
==================
*/
class ScrapHeap
{
private:
	// We do not describe constructors and destructors
	// The class is just a shell

	ScrapHeap(VOID) = default;
	~ScrapHeap(VOID) = default;
public:
	LPVOID Allocate(UINT64 Size, UINT32 Alignment);
	VOID   Deallocate(LPVOID Memory);
};