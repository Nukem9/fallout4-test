#pragma once

#include "../../common.h"

class MemoryManager
{
private:
	MemoryManager(VOID) = default;
	~MemoryManager(VOID) = default;
public:
	static LPVOID Allocate(MemoryManager *Manager, UINT64 Size, UINT32 Alignment, BOOL Aligned);
	static VOID   Deallocate(MemoryManager *Manager, LPVOID Memory, BOOL Aligned);
	static UINT64 Size(MemoryManager *Manager, LPVOID Memory);
};

class ScrapHeap
{
private:
	ScrapHeap(VOID) = default;
	~ScrapHeap(VOID) = default;
public:
	LPVOID Allocate(UINT64 Size, UINT32 Alignment);
	VOID   Deallocate(LPVOID Memory);
};