////////////////////////////////////////////
///*
//* Copyright (c) 2022 Perchik71 <email:perchik71@outlook.com>
//*
//* Permission is hereby granted, free of charge, to any person obtaining a copy of this
//* software and associated documentation files (the "Software"), to deal in the Software
//* without restriction, including without limitation the rights to use, copy, modify, merge,
//* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
//* persons to whom the Software is furnished to do so, subject to the following conditions:
//*
//* The above copyright notice and this permission notice shall be included in all copies or
//* substantial portions of the Software.
//*
//* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
//* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//* DEALINGS IN THE SOFTWARE.
//*/
////////////////////////////////////////////
//
#include "BSPointerHandleManager.h"

#include "TESObjectREFR.h"
#include "..\[Common]\StdAfx.h"

BSHandleRefObjectManager* g_HandleRefObjectManager = NULL;

// IBSUntypedPointerHandle

template <typename T, int IndexBits, int AgeCountBits>
VOID IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::SetBitwiseNull(VOID)
{
	m_Bits = 0;
}

template <typename T, int IndexBits, int AgeCountBits>
BOOL IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::IsBitwiseNull(VOID) const
{
	return !m_Bits;
}

template <typename T, int IndexBits, int AgeCountBits>
T IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::GetIndex(VOID) const
{
	return m_Bits & MASK_INDEX_BIT;
}

template <typename T, int IndexBits, int AgeCountBits>
T IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::GetAge(VOID) const
{
	return m_Bits & MASK_AGE_BIT;
}

template <typename T, int IndexBits, int AgeCountBits>
VOID IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::SetInUse(VOID)
{
	m_Bits |= MASK_ACTIVE_BIT;
}

template <typename T, int IndexBits, int AgeCountBits>
VOID IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::SetNotInUse(VOID)
{
	m_Bits &= ~MASK_ACTIVE_BIT;
}

template <typename T, int IndexBits, int AgeCountBits>
BOOL IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::IsInUse(VOID) const
{
	return (m_Bits & MASK_ACTIVE_BIT) != 0;
}

template <typename T, int IndexBits, int AgeCountBits>
VOID IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::IncrementAge(VOID)
{
	m_Bits = ((m_Bits << INDEX_BIT) & MASK_AGE_BIT) | (m_Bits & ~MASK_AGE_BIT);
}

template <typename T, int IndexBits, int AgeCountBits>
VOID IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::Set(T Index, T Age)
{
	AssertMsg(Index < MAX_HANDLE_COUNT, "IBSUntypedPointerHandle::Set - parameter Index is too large");

	m_Bits = Index | Age;
}

template <typename T, int IndexBits, int AgeCountBits>
VOID IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::SetIndex(T Index)
{
	AssertMsg(Index < MAX_HANDLE_COUNT, "IBSUntypedPointerHandle::Set - parameter Index is too large");

	m_Bits = (Index & MASK_INDEX_BIT) | (m_Bits & ~MASK_INDEX_BIT);
}

template <typename T, int IndexBits, int AgeCountBits>
IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>& IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::operator=(const IBSUntypedPointerHandle& Other)
{
	m_Bits = Other.m_Bits;
	return *this;
}

template <typename T, int IndexBits, int AgeCountBits>
BOOL IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::operator==(const IBSUntypedPointerHandle& Other) const
{
	return m_Bits == Other.m_Bits;
}

template <typename T, int IndexBits, int AgeCountBits>
BOOL IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::operator!=(const IBSUntypedPointerHandle& Other) const
{
	return m_Bits != Other.m_Bits;
}

template <typename T, int IndexBits, int AgeCountBits>
IBSUntypedPointerHandle<T, IndexBits, AgeCountBits>::IBSUntypedPointerHandle(VOID)
{
	SetBitwiseNull();
}

// BSHandleRefObject

LONGLONG BSHandleRefObject::IncRef(VOID)
{
	AssertMsgDebug(GetRef() < REF_COUNT_MASK, "BSHandleRefObject - IncRefCount is about to cause refcount wraparound to 0.");

	InterlockedIncrement64(&m_ll64RefCount);

	return m_ll64RefCount;
}

LONGLONG BSHandleRefObject::DecRef(VOID)
{
	AssertMsgDebug(GetRef() != 0, "BSHandleRefObject - DecRefCount called with refcount already 0.");

	InterlockedDecrement64(&m_ll64RefCount);
	LONGLONG count = m_ll64RefCount & REF_COUNT_MASK;

	if (count <= 0)
		DeleteThis();

	return count;
}

VOID BSHandleRefObject::SetHandleEntryIndex(LONGLONG HandleIndex)
{
	m_ll64RefCount = (HandleIndex << HANDLE_BIT_INDEX) | (1u << ACTIVE_BIT_INDEX) | GetRef();
}

LONGLONG BSHandleRefObject::GetHandleEntryIndex(VOID) const
{
	return m_ll64RefCount >> HANDLE_BIT_INDEX;
}

VOID BSHandleRefObject::ClearHandleEntryIndex(VOID)
{
	m_ll64RefCount &= REF_COUNT_MASK;
}

BOOL BSHandleRefObject::IsHandleValid(VOID) const
{
	return (m_ll64RefCount & (1u << ACTIVE_BIT_INDEX)) != 0;
}

LONGLONG BSHandleRefObject::GetRef(VOID) const
{
	return m_ll64RefCount & REF_COUNT_MASK;
}

// BSHandleRefObjectManager

template <typename T>
VOID IBSHandleRefObjectManager<T>::Entry::SetNextFreeEntry(T Index)
{
	SetIndex(Index);
}

template <typename T>
T IBSHandleRefObjectManager<T>::Entry::GetNextFreeEntry(VOID) const
{
	return GetIndex();
}

template <typename T>
VOID IBSHandleRefObjectManager<T>::Entry::SetPointer(TESObjectREFR* Pointer)
{
	m_Pointer = Pointer;
}

template <typename T>
TESObjectREFR* IBSHandleRefObjectManager<T>::Entry::GetPointer(VOID) const
{
	return m_Pointer.operator TESObjectREFR * ();
}

template <typename T>
BOOL IBSHandleRefObjectManager<T>::Entry::IsValid(T Age) const
{
	return IsInUse() && Age == Age;
}

template <typename T>
VOID IBSHandleRefObjectManager<T>::Lock(VOID)
{
	m_HandleManagerLock.LockForWrite();
}

template <typename T>
VOID IBSHandleRefObjectManager<T>::Unlock(VOID)
{
	m_HandleManagerLock.Unlock();
}

template <typename T>
VOID IBSHandleRefObjectManager<T>::ResetList(VOID)
{
	m_FreeListHead = INVALID_INDEX;
	m_FreeListTail = INVALID_INDEX;
}

template <typename T>
const BSUntypedPointerHandle IBSHandleRefObjectManager<T>::NullHandle;

template <typename T>
IBSHandleRefObjectManager<T>::IBSHandleRefObjectManager(VOID):
	m_FreeListHead(0), m_FreeListTail(BSUntypedPointerHandle::MAX_HANDLE_COUNT - 1)
{
	m_HandleEntries.resize(BSUntypedPointerHandle::MAX_HANDLE_COUNT);

	for (T i = 0; i < BSUntypedPointerHandle::MAX_HANDLE_COUNT; i++)
	{
		if ((i + 1) >= BSUntypedPointerHandle::MAX_HANDLE_COUNT)
			m_HandleEntries[i].SetNextFreeEntry(i);
		else
			m_HandleEntries[i].SetNextFreeEntry(i + 1);
	}
}

template <typename T>
IBSHandleRefObjectManager<T>::~IBSHandleRefObjectManager(VOID)
{
	m_HandleManagerLock.LockForWrite();

	for (T i = 0; i < BSUntypedPointerHandle::MAX_HANDLE_COUNT; i++)
	{
		auto& arrayHandle = m_HandleEntries[i];

		if (!arrayHandle.IsInUse())
			continue;

		if (arrayHandle.GetPointer())
			arrayHandle.GetPointer()->ClearHandleEntryIndex();

		arrayHandle.SetPointer(NULL);
		arrayHandle.SetNotInUse();

		if (m_FreeListTail == INVALID_INDEX)
			m_FreeListHead = i;
		else
			m_HandleEntries[m_FreeListTail].SetNextFreeEntry(i);

		arrayHandle.SetNextFreeEntry(i);
		m_FreeListTail = i;
	}

	m_HandleManagerLock.Unlock();
}

// BSPointerHandleManagerInterface

VOID BSPointerHandleManagerInterface::Initialize(VOID)
{
	g_HandleRefObjectManager = new BSHandleRefObjectManager();
}

VOID BSPointerHandleManagerInterface::Release(VOID)
{
	delete g_HandleRefObjectManager;
}

BSUntypedPointerHandle BSPointerHandleManagerInterface::GetCurrentHandle(TESObjectREFR* Refr)
{
	BSUntypedPointerHandle untypedHandle;

	if (Refr && g_HandleRefObjectManager && Refr->IsHandleValid())
	{
		auto& handle = (*g_HandleRefObjectManager)[Refr->GetHandleEntryIndex()];
		untypedHandle.Set(Refr->GetHandleEntryIndex(), handle.Age);

		AssertMsg(untypedHandle.Age == handle.Age, "BSPointerHandleManagerInterface::GetCurrentHandle - Handle already exists but has wrong age!");
	}

	return untypedHandle;
}

BSUntypedPointerHandle BSPointerHandleManagerInterface::CreateHandle(TESObjectREFR* Refr)
{
	BSUntypedPointerHandle untypedHandle;

	if (!Refr || !g_HandleRefObjectManager)
		return untypedHandle;

	// Shortcut: Check if the handle is already valid
	untypedHandle = GetCurrentHandle(Refr);
	if (untypedHandle != g_HandleRefObjectManager->NullHandle)
		return untypedHandle;

	// Wasn't present. Acquire lock and add it (unless someone else inserted it in the meantime)
	g_HandleRefObjectManager->Lock();
	{
		untypedHandle = GetCurrentHandle(Refr);

		if (untypedHandle == g_HandleRefObjectManager->NullHandle)
		{
			if (g_HandleRefObjectManager->Head() == BSHandleRefObjectManager::INVALID_INDEX)
			{
				untypedHandle.SetBitwiseNull();
				AssertMsgVa(false, "OUT OF HANDLE ARRAY ENTRIES. Null handle created for pointer 0x%p.", Refr);
			}
			else
			{
				auto& newHandle = (*g_HandleRefObjectManager)[g_HandleRefObjectManager->Head()];
				newHandle.IncrementAge();
				newHandle.SetInUse();

				untypedHandle.Set(newHandle.GetNextFreeEntry(), newHandle.Age);
				newHandle.SetPointer(Refr);

				Refr->SetHandleEntryIndex(g_HandleRefObjectManager->Head());
				Assert(Refr->GetHandleEntryIndex() == g_HandleRefObjectManager->Head());

				if (newHandle.GetNextFreeEntry() == g_HandleRefObjectManager->Head())
				{
					// Table reached the maximum count
					Assert(g_HandleRefObjectManager->Head() == g_HandleRefObjectManager->Tail());
					g_HandleRefObjectManager->ResetList();
				}
				else
				{
					Assert(g_HandleRefObjectManager->Head() != g_HandleRefObjectManager->Tail());
					g_HandleRefObjectManager->SetHead(newHandle.GetNextFreeEntry());
				}
			}
		}
	}
	g_HandleRefObjectManager->Unlock();

	return untypedHandle;
}

VOID BSPointerHandleManagerInterface::Destroy1(const BSUntypedPointerHandle& Handle)
{
	if (Handle.IsBitwiseNull())
		return;

	g_HandleRefObjectManager->Lock();
	{
		const auto handleIndex = Handle.Index;
		auto& arrayHandle = (*g_HandleRefObjectManager)[handleIndex];

		if (arrayHandle.IsValid(Handle.Age))
		{
			arrayHandle.GetPointer()->ClearHandleEntryIndex();
			arrayHandle.SetPointer(nullptr);
			arrayHandle.SetNotInUse();

			if (g_HandleRefObjectManager->Tail() == BSHandleRefObjectManager::INVALID_INDEX)
				g_HandleRefObjectManager->SetHead(handleIndex);
			else
				(*g_HandleRefObjectManager)[g_HandleRefObjectManager->Tail()].SetNextFreeEntry(handleIndex);

			arrayHandle.SetNextFreeEntry(handleIndex);
			g_HandleRefObjectManager->SetTail(handleIndex);
		}
	}
	g_HandleRefObjectManager->Unlock();
}

VOID BSPointerHandleManagerInterface::Destroy2(BSUntypedPointerHandle& Handle)
{
	if (Handle.IsBitwiseNull())
		return;

	g_HandleRefObjectManager->Lock();
	{
		const auto handleIndex = Handle.Index;
		auto& arrayHandle = (*g_HandleRefObjectManager)[handleIndex];

		if (arrayHandle.IsValid(Handle.Age))
		{
			arrayHandle.GetPointer()->ClearHandleEntryIndex();
			arrayHandle.SetPointer(nullptr);
			arrayHandle.SetNotInUse();

			if (g_HandleRefObjectManager->Tail() == BSHandleRefObjectManager::INVALID_INDEX)
				g_HandleRefObjectManager->SetHead(handleIndex);
			else
				(*g_HandleRefObjectManager)[g_HandleRefObjectManager->Tail()].SetNextFreeEntry(handleIndex);

			arrayHandle.SetNextFreeEntry(handleIndex);
			g_HandleRefObjectManager->SetTail(handleIndex);
		}

		// Identical to Destroy1 except for this Handle.SetBitwiseNull();
		Handle.SetBitwiseNull();
	}
	g_HandleRefObjectManager->Unlock();
}

bool BSPointerHandleManagerInterface::GetSmartPointer1(const BSUntypedPointerHandle& Handle, NiPointer<TESObjectREFR>& Out)
{
	if (Handle.IsBitwiseNull())
	{
		Out = nullptr;
		return false;
	}

	const uint32_t handleIndex = Handle.Index;
	auto& arrayHandle = (*g_HandleRefObjectManager)[handleIndex];

	Out = static_cast<TESObjectREFR*>(arrayHandle.GetPointer());

	if (!arrayHandle.IsValid(Handle.Age) || Out->GetHandleEntryIndex() != handleIndex)
		Out = nullptr;

	return Out != nullptr;
}

bool BSPointerHandleManagerInterface::GetSmartPointer2(BSUntypedPointerHandle& Handle, NiPointer<TESObjectREFR>& Out)
{
	if (Handle.IsBitwiseNull())
	{
		Out = NULL;
		return false;
	}

	const uint32_t handleIndex = Handle.Index;
	auto& arrayHandle = (*g_HandleRefObjectManager)[handleIndex];

	Out = static_cast<TESObjectREFR*>(arrayHandle.GetPointer());

	if (!arrayHandle.IsValid(Handle.Age) || Out->GetHandleEntryIndex() != handleIndex)
	{
		// Identical to GetSmartPointer1 except for this Handle.SetBitwiseNull();
		Handle.SetBitwiseNull();
		Out = nullptr;
	}

	return Out != nullptr;
}

VOID FIXAPI Fix_HandleRefPatch(VOID)
{
	XUtil::DetourJump(OFFSET(0xA2940, 0), &BSHandleRefObject::IncRef);

	XUtil::DetourJump(OFFSET(0x5D9640, 0), &BSPointerHandleManagerInterface::Initialize);
	XUtil::DetourCall(OFFSET(0x5B71D9, 0), &BSPointerHandleManagerInterface::Release);
	XUtil::DetourJump(OFFSET(0x422590, 0), &BSPointerHandleManagerInterface::GetCurrentHandle);
	XUtil::DetourJump(OFFSET(0x421F20, 0), &BSPointerHandleManagerInterface::CreateHandle);
	XUtil::DetourJump(OFFSET(0x186840, 0), &BSPointerHandleManagerInterface::Destroy1);
	XUtil::DetourJump(OFFSET(0x422110, 0), &BSPointerHandleManagerInterface::Destroy2);
	XUtil::DetourJump(OFFSET(0xA27A0, 0), &BSPointerHandleManagerInterface::GetSmartPointer1);
	XUtil::DetourJump(OFFSET(0xA2610, 0), &BSPointerHandleManagerInterface::GetSmartPointer2);

	// Stub out the rest of the functions which shouldn't ever be called now
	XUtil::PatchMemory(OFFSET(0x423530, 0), { 0xCC });// BSUntypedPointerHandle::Set
}