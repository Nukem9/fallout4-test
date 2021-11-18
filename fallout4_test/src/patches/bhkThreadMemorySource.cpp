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

#include "bhkThreadMemorySource.h"

bhkThreadMemorySource::bhkThreadMemorySource(VOID)
{
	InitializeCriticalSection(&m_CritSec);
}

bhkThreadMemorySource::~bhkThreadMemorySource(VOID)
{
	DeleteCriticalSection(&m_CritSec);
}

LPVOID bhkThreadMemorySource::blockAlloc(UINT64 numBytes)
{
	return MemoryManager::Allocate(NULL, numBytes, 16, TRUE);
}

VOID bhkThreadMemorySource::blockFree(LPVOID p, UINT64 numBytes)
{
	MemoryManager::Deallocate(NULL, p, TRUE);
}

LPVOID bhkThreadMemorySource::bufAlloc(UINT64& reqNumBytesInOut)
{
	return blockAlloc(reqNumBytesInOut);
}

VOID bhkThreadMemorySource::bufFree(LPVOID p, UINT64 numBytes)
{
	return blockFree(p, numBytes);
}

LPVOID bhkThreadMemorySource::bufRealloc(LPVOID pold, UINT64 oldNumBytes, UINT64& reqNumBytesInOut)
{
	LPVOID p = blockAlloc(reqNumBytesInOut);
	memcpy(p, pold, oldNumBytes);
	blockFree(pold, oldNumBytes);

	return p;
}

VOID bhkThreadMemorySource::blockAllocBatch(LPVOID *ptrsOut, UINT64 numPtrs, UINT64 blockSize)
{
	for (INT32 i = 0; i < numPtrs; i++)
		ptrsOut[i] = blockAlloc(blockSize);
}

VOID bhkThreadMemorySource::blockFreeBatch(LPVOID *ptrsIn, UINT64 numPtrs, UINT64 blockSize)
{
	for (INT32 i = 0; i < numPtrs; i++)
		blockFree(ptrsIn[i], blockSize);
}

VOID bhkThreadMemorySource::getMemoryStatistics(class MemoryStatistics& u)
{
	// Nothing
}

UINT64 bhkThreadMemorySource::getAllocatedSize(const LPVOID obj, UINT64 nbytes)
{
	Assert(FALSE);
	return 0;
}

VOID bhkThreadMemorySource::resetPeakMemoryStatistics(VOID)
{
	// Nothing
}

#if FALLOUT4
LPVOID bhkThreadMemorySource::getExtendedInterface(VOID)
{
	return NULL;
}
#endif