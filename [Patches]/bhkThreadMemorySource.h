//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

#include "..\[Common]\StdAfx.h"

class bhkThreadMemorySource
{
public:
	CHAR _pad0[0x8];
	CRITICAL_SECTION m_CritSec;

public:
	DECLARE_CONSTRUCTOR_HOOK(bhkThreadMemorySource);

	bhkThreadMemorySource(VOID);
	virtual ~bhkThreadMemorySource(VOID);
	virtual LPVOID blockAlloc(UINT64 numBytes);
	virtual VOID   blockFree(LPVOID p, UINT64 numBytes);
	virtual LPVOID bufAlloc(UINT64& reqNumBytesInOut);
	virtual VOID   bufFree(LPVOID p, UINT64 numBytes);
	virtual LPVOID bufRealloc(LPVOID pold, UINT64 oldNumBytes, UINT64& reqNumBytesInOut);
	virtual VOID   blockAllocBatch(LPVOID *ptrsOut, UINT64 numPtrs, UINT64 blockSize);
	virtual VOID   blockFreeBatch(LPVOID *ptrsIn, UINT64 numPtrs, UINT64 blockSize);
	virtual VOID   getMemoryStatistics(class MemoryStatistics& u);
	virtual UINT64 getAllocatedSize(const LPVOID obj, UINT64 nbytes);
	virtual VOID   resetPeakMemoryStatistics(VOID);
#if FALLOUT4
	virtual LPVOID getExtendedInterface(VOID);
#endif
};
static_assert_offset(bhkThreadMemorySource, m_CritSec, 0x10);