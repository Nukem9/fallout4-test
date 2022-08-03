//////////////////////////////////////////
/*
* Copyright (c) 2022 Perchik71 <email:perchik71@outlook.com>
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

#include "..\[Common]\CommInc.h"

#include "NiTypes.h"
#include "NiClasses.h"
#include "NiRefObject.h"

#include <vector>

class TESObjectREFR;

#pragma warning (disable : 4293)

// vanilla --- class BSUntypedPointerHandle<21,5>

// 4 (DWORD)
template <typename T = DWORD, int IndexBits = 26, int AgeCountBits = 5>
class IBSUntypedPointerHandle
{
public:
	// NOTE: For DWORD (32bit)
	//
	// 31       30       26                   0
	// |--------|--------|--------------------|
	// | Active |    Age |    Handle Index    |
	// |--------|--------|--------------------|
	//
	constexpr static T INDEX_BIT		= IndexBits;
	constexpr static T AGE_BIT			= AgeCountBits;
	constexpr static T UNUSED_BIT_START = INDEX_BIT + AGE_BIT;							// 26 in vanilla (fixes no)

	constexpr static T MASK_INDEX_BIT	= (((T)1) << INDEX_BIT) - ((T)1);				// 03FFFFFF
	constexpr static T MASK_AGE_BIT		= ((((T)1) << AGE_BIT) - ((T)1)) << INDEX_BIT;	// 7C000000
	constexpr static T MASK_ACTIVE_BIT	= ((T)1) << UNUSED_BIT_START;					// 80000000
	constexpr static T MAX_HANDLE_COUNT = ((T)1) << INDEX_BIT;							// 04000000
private:
	T m_Bits;    
public:
	VOID SetBitwiseNull(VOID);
	BOOL IsBitwiseNull(VOID) const;
	T GetIndex(VOID) const;
	T GetAge(VOID) const;
	VOID SetInUse(VOID);
	VOID SetNotInUse(VOID);
	INLINE BOOL IsInUse(VOID) const;
	INLINE VOID IncrementAge(VOID);
	VOID Set(T Index, T Age);
	VOID SetIndex(T Index);
public:
	IBSUntypedPointerHandle& operator=(const IBSUntypedPointerHandle& Other);
	BOOL operator==(const IBSUntypedPointerHandle& Other) const;
	BOOL operator!=(const IBSUntypedPointerHandle& Other) const;
public:
	READ_PROPERTY(GetAge) T Age;
	PROPERTY(GetIndex, SetIndex) T Index;
public:
	IBSUntypedPointerHandle(VOID);
};

#if UNTYPE_PTR_64
// NOTE: For DWORD64 (64bit)
//
// 63                   35       34       30                   0
// |--------------------|--------|--------|--------------------|
// |       Unused       | Active |   Age  |    Handle Index    |
// |--------------------|--------|--------|--------------------|
//

#define BSUntypedPointerHandle IBSUntypedPointerHandle<DWORD64, 30, 5>
static_assert(sizeof(BSUntypedPointerHandle) == 0x8);
#else
#define BSUntypedPointerHandle IBSUntypedPointerHandle<>
static_assert(sizeof(BSUntypedPointerHandle) == 0x4);
#endif // UNTYPE_PTR_64

// no vanilla (from 32 to 64)
class BSHandleRefObject : public NiRefObject
{
public:
	//
	// 63             11       10          0
	// |--------------|--------|-----------|
	// | Handle Index | Active | Ref Count |
	// |--------------|--------|-----------|
	//
	constexpr static LONGLONG ACTIVE_BIT_INDEX	= 10;								// 400
	constexpr static LONGLONG HANDLE_BIT_INDEX	= 11;
	constexpr static LONGLONG REF_COUNT_MASK	= (1ll << ACTIVE_BIT_INDEX) - 1ll;	// 3FF
public:
	LONGLONG IncRef(VOID);
	LONGLONG DecRef(VOID);	
	LONGLONG GetRef(VOID) const;

	VOID SetHandleEntryIndex(LONGLONG HandleIndex);
	LONGLONG GetHandleEntryIndex(VOID) const;
	VOID ClearHandleEntryIndex(VOID);
	BOOL IsHandleValid(VOID) const;
public:
	READ_PROPERTY(GetRefCount) LONGLONG RefCount;
	PROPERTY(GetHandleEntryIndex, SetHandleEntryIndex) LONGLONG HandleIndex;
};

//#define BSHandleRefObject IBSHandleRefObject<>
static_assert(sizeof(BSHandleRefObject) == 0x10);

#if UNTYPE_PTR_64
template <typename T = DWORD64>
#else
template <typename T = DWORD>
#endif // UNTYPE_PTR_64
class IBSHandleRefObjectManager
{
public:
	constexpr static T INVALID_INDEX = (T)-1;
public:
	class Entry : public BSUntypedPointerHandle
	{
	private:
		NiPointer<TESObjectREFR> m_Pointer;
	public:
		VOID SetNextFreeEntry(T Index);
		T GetNextFreeEntry(VOID) const;
		VOID SetPointer(TESObjectREFR* Pointer);
		TESObjectREFR* GetPointer(VOID) const;
		BOOL IsValid(T Age) const;
	};
private:
	T m_FreeListHead;
	T m_FreeListTail;
	BSReadWriteLock m_HandleManagerLock;
	std::vector<Entry> m_HandleEntries;
public:
	const static BSUntypedPointerHandle NullHandle;
public:
	INLINE T Head(VOID) const { return m_FreeListHead; }
	INLINE T Tail(VOID) const { return m_FreeListTail; }
	INLINE Entry& GetAt(size_t id) { return m_HandleEntries[id]; }
public:
	INLINE Entry& operator[](size_t id) { return m_HandleEntries[id]; }
public:
	VOID Lock(VOID);
	VOID Unlock(VOID);
	VOID ResetList(VOID);
	INLINE VOID SetHead(T newHead) { m_FreeListHead = newHead; }
	INLINE VOID SetTail(T newTail) { m_FreeListTail = newTail; }
public:
	IBSHandleRefObjectManager(VOID);
	~IBSHandleRefObjectManager(VOID);
};

#define BSHandleRefObjectManager IBSHandleRefObjectManager<>

namespace BSPointerHandleManagerInterface
{
	VOID Initialize(VOID);
	VOID Release(VOID);

	BSUntypedPointerHandle GetCurrentHandle(TESObjectREFR* Refr);
	BSUntypedPointerHandle CreateHandle(TESObjectREFR* Refr);
	VOID Destroy1(const BSUntypedPointerHandle& Handle);
	VOID Destroy2(BSUntypedPointerHandle& Handle);
	bool GetSmartPointer1(const BSUntypedPointerHandle& Handle, NiPointer<TESObjectREFR>& Out);
	bool GetSmartPointer2(BSUntypedPointerHandle& Handle, NiPointer<TESObjectREFR>& Out);
};

extern BSHandleRefObjectManager* g_HandleRefObjectManager;
