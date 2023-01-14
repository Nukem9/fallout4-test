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
	m_ll64RefCount = (HandleIndex << HANDLE_BIT_INDEX) | (1ll << ACTIVE_BIT_INDEX) | GetRef();
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
	return (m_ll64RefCount & (1ll << ACTIVE_BIT_INDEX)) != 0;
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
	m_HandleManagerLock.TryLockForWrite();
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
	m_HandleManagerLock.TryLockForWrite();

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
		Out = NULL;
		return false;
	}

	const uint32_t handleIndex = Handle.Index;
	auto& arrayHandle = (*g_HandleRefObjectManager)[handleIndex];

	Out = static_cast<TESObjectREFR*>(arrayHandle.GetPointer());

	if (!arrayHandle.IsValid(Handle.Age) || Out->GetHandleEntryIndex() != handleIndex)
		Out = NULL;

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
		Out = NULL;
	}

	return Out != nullptr;
}

VOID FIXAPI F_IncRefPatch(VOID)
{
	auto SignIncRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "4C 8D 05 ? ? ? ? 48 8D 0D ? ? ? ? BA 3B 00 00 00 E8");
	size_t patched = 0, need_p = 0;
	uint8_t szbuff[64];

	for (auto sign : SignIncRef)
	{
		auto start_r = sign + 24;

		if ((*((uint8_t*)start_r) == 0xF0)) // lock
		{
			if (*((uint8_t*)(start_r + 1)) == 0xFF) // inc
			{
				memcpy_s(szbuff, 3, (uint8_t*)start_r + 1, 3);

				memset((uint8_t*)start_r, 0x90, 0xB);
				((uint8_t*)start_r)[0] = 0xF0;
				((uint8_t*)start_r)[1] = 0x48;
				memcpy_s((uint8_t*)(start_r + 2), 3, szbuff, 3);

				((uint8_t*)start_r)[0xB] = 0xEB;	// jmp	(skip test)

				patched++;
#if FALLOUT4_DEVELOPER_MODE
				memset((uint8_t*)sign, 0x90, 7);
#endif
			}
			else if ((*((uint8_t*)(start_r + 1)) == 0x41) && (*((uint8_t*)(start_r + 2)) == 0xFF)) // inc r8 - r15
			{
				((uint8_t*)start_r)[1] = 0x49;
				((uint8_t*)start_r)[5] = 0x49;

				patched++;
#if FALLOUT4_DEVELOPER_MODE
				memset((uint8_t*)sign, 0x90, 7);
#endif
			}
#if FALLOUT4_DEVELOPER_MODE
			need_p++;
#endif
		}
	}

	_F4CKMSG("[F4CK]		BSHandleRefObject::IncRef");
	_F4CKMSG_FMT("[F4CK]			Patched: %d", patched);
}

VOID FIXAPI F_DecRefPatch(VOID)
{
	auto IsJump = [](uintptr_t off) -> bool {
		return ((*((uint8_t*)(off)) == 0x75) || (*((uint8_t*)(off)) == 0x74) ||
			((*((uint8_t*)(off)) == 0x0F) && ((*((uint8_t*)(off + 1)) == 0x85) || (*((uint8_t*)(off + 1)) == 0x84))));
	};

	size_t patched = 0;
	uint8_t szbuff[64];
	uint8_t* src = nullptr;
	int8_t jump_to = 0;
	int8_t jump_from = 0;

	auto refmask = (DWORD)BSHandleRefObject::REF_COUNT_MASK;

	auto SignDecRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "83 ? FF F0 0F ? ? ? FF ? ? FF 03 00 00");

	for (auto sign : SignDecRef)
	{
		if (*((uint8_t*)(sign - 0x1A)) == 0x77)
		{
			jump_from = 0x1A;
			jump_to = 0x29;

		patched_var1:

			src = (uint8_t*)(sign - jump_from);

			memcpy_s(szbuff, 64, (uint8_t*)sign, 0xF);
			memset(src, 0x90, jump_to);

			src[0] = 0x48;
			memcpy_s(src + 1, 0xF, szbuff, 3);
			src[4] = 0xF0;
			src[5] = 0x48;
			memcpy_s(src + 6, 0xF, szbuff + 4, 4);
			src[10] = 0x48;
			memcpy_s(src + 11, 0xF, szbuff + 8, 2);
			src[13] = 0x48;
			src[14] = szbuff[10];
			memcpy_s(src + 15, 0xF, &refmask, sizeof(refmask));
			src[19] = 0xEB;
			src[20] = jump_to - 21;

			src = (uint8_t*)(sign + 3);
			src[0] = 0xEB;
			src[1] = -(jump_from + 5);
			
			patched++;
		}
		else if (*((uint8_t*)(sign - 0x1E)) == 0x77)
		{
			jump_from = 0x1E;
			jump_to = 0x2D;

			goto patched_var1;
		}
		else if ((*((uint8_t*)(sign - 0x1B)) == 0x77) && (*((uint8_t*)(sign - 1)) == 0x48))
		{
			jump_from = 0x1B;
			jump_to = 0x2A;

			goto patched_var1;
		}
		else if (*((uint8_t*)(sign - 0x18)) == 0x77)
		{
			jump_from = 0x18;
			jump_to = 0x27;

			goto patched_var1;
		}
		else if (*((uint8_t*)(sign - 0x19)) == 0x77)
		{
			jump_from = 0x19;
			jump_to = 0x28;

			goto patched_var1;
		}
	}

	SignDecRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "83 ? FF F0 0F ? ? ? FF ? F7 ? FF 03 00 00");

	for (auto sign : SignDecRef)
	{
		if (*((uint8_t*)(sign - 0x1A)) == 0x77)
		{
			jump_from = 0x1A;
			jump_to = 0x2A;

		patched_var2:

			src = (uint8_t*)(sign - jump_from);
			memcpy_s(szbuff, 64, (uint8_t*)sign, 0x10);
			memset(src, 0x90, jump_to);

			src[0] = 0x48;
			memcpy_s(src + 1, 0x10, szbuff, 3);
			src[4] = 0xF0;
			src[5] = 0x48;
			memcpy_s(src + 6, 0x10, szbuff + 4, 4);
			src[10] = 0x48;
			memcpy_s(src + 11, 0x10, szbuff + 8, 2);
			src[13] = 0x48;
			memcpy_s(src + 14, 0x10, szbuff + 10, 2);
			memcpy_s(src + 16, 0x10, &refmask, sizeof(refmask));
			src[20] = 0xEB;
			src[21] = jump_to - 22;

			src = (uint8_t*)(sign + 3);
			src[0] = 0xEB;
			src[1] = -(jump_from + 5);

			patched++;
		}
		else if ((*((uint8_t*)(sign - 0x1B)) == 0x77) && (*((uint8_t*)(sign - 1)) == 0x48))
		{
			jump_from = 0x1B;
			jump_to = 0x2B;

			goto patched_var2;
		}
		else if (*((uint8_t*)(sign - 0x18)) == 0x77)
		{
			jump_from = 0x18;
			jump_to = 0x28;

			goto patched_var2;
		}
	}

	SignDecRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "83 ? FF F0 ? 0F ? ? ? FF ? ? FF 03 00 00");

	for (auto sign : SignDecRef)
	{
		if (*((uint8_t*)(sign - 0x1A)) == 0x77)
		{
			jump_from = 0x1A;
			jump_to = 0x2A;

			src = (uint8_t*)(sign - jump_from);
			memcpy_s(szbuff, 64, (uint8_t*)sign, 0x10);
			memset(src, 0x90, jump_to);

			src[0] = 0x48;
			memcpy_s(src + 1, 0x10, szbuff, 3);
			src[4] = 0xF0;
			src[5] = szbuff[4] == 0x41 ? 0x49 : 0x4C;
			memcpy_s(src + 6, 0x10, szbuff + 5, 4);
			src[10] = 0x48;
			memcpy_s(src + 11, 0x10, szbuff + 9, 2);
			src[13] = 0x48;
			src[14] = szbuff[11];
			memcpy_s(src + 15, 0x10, &refmask, sizeof(refmask));
			src[19] = 0xEB;
			src[20] = jump_to - 21;

			src = (uint8_t*)(sign + 3);
			src[0] = 0xEB;
			src[1] = -(jump_from + 5);

			patched++;
		}
	}

	SignDecRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "8B ? F0 0F ? ? ? FF ? ? FF 03 00 00");

	// or r??, FF
	// lock xadd qword ptr ss:[r??+38], r??
	// dec r??
	// test r??, 3FF
	// 
	// ^ ^ converting the bottom to the top ^ ^
	// 
	// mov e??, e??
	// lock xadd dword ptr ss:[r??+38], e??
	// dec e??
	// test e??, 3FF
	
	for (auto sign : SignDecRef)
	{
		if (*((uint8_t*)(sign - 0x1A)) == 0x77)
		{
			jump_from = 0x1A;
			jump_to = 0x28;

		patched_var3:

			src = (uint8_t*)(sign - jump_from);
			memcpy_s(szbuff, 64, (uint8_t*)sign, 0xE);
			memset(src, 0x90, jump_to);

			src[0] = 0x48;
			src[1] = 0x83;
			src[2] = szbuff[8];
			src[3] = 0xFF;

			src[4] = 0xF0;
			src[5] = 0x48;
			memcpy_s(src + 6, 0xE, szbuff + 3, 4);
			src[10] = 0x48;
			memcpy_s(src + 11, 0xE, szbuff + 7, 2);
			src[13] = 0x48;
			src[14] = szbuff[9];
			memcpy_s(src + 15, 0xE, &refmask, sizeof(refmask));
			src[19] = 0xEB;
			src[20] = jump_to - 21;

			src = (uint8_t*)(sign + 2);
			src[0] = 0xEB;
			src[1] = -(jump_from + 4);

			patched++;
		}
		if ((*((uint8_t*)(sign - 0x1B)) == 0x77) && (*((uint8_t*)(sign - 1)) == 0x41))
		{
			jump_from = 0x1B;
			jump_to = 0x29;

			goto patched_var3;
		}
		if ((*((uint8_t*)(sign - 0x1D)) == 0x77) && (*((uint8_t*)(sign - 3)) == 0x83))
		{
			jump_from = 0x1D;
			jump_to = 0x2B;

			goto patched_var3;
		}
		else if ((*((uint8_t*)(sign - 0x1F)) == 0x77) && (*((uint8_t*)(sign - 4)) == 0x83))
		{
			jump_from = 0x1F;
			jump_to = 0x2D;

			goto patched_var3;
		}
		else if (*((uint8_t*)(sign - 0x18)) == 0x77)
		{
			jump_from = 0x18;
			jump_to = 0x26;

			goto patched_var3;
		}
		else if (*((uint8_t*)(sign - 0x19)) == 0x77)
		{
			jump_from = 0x19;
			jump_to = 0x27;

			goto patched_var3;
		}
	}

	SignDecRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "41 8B ? F0 ? 0F ? ? ? FF ? ? FF 03 00 00");

	for (auto sign : SignDecRef)
	{
		if (*((uint8_t*)(sign - 0x1A)) == 0x77)
		{
			jump_from = 0x1A;
			jump_to = 0x2A;

		patched_var5:

			src = (uint8_t*)(sign - jump_from);
			memcpy_s(szbuff, 64, (uint8_t*)sign, 0x10);
			memset(src, 0x90, jump_to);

			src[0] = 0x48;
			src[1] = 0x83;
			src[2] = szbuff[10];
			src[3] = 0xFF;

			src[4] = 0xF0;
			src[5] = (szbuff[4] == 0x44) ? 0x4C : 0x49;
			memcpy_s(src + 6, 0x10, szbuff + 5, 4);
			src[10] = 0x48;
			memcpy_s(src + 11, 0xE, szbuff + 9, 2);
			src[13] = 0x48;
			src[14] = szbuff[11];
			memcpy_s(src + 15, 0x10, &refmask, sizeof(refmask));
			src[19] = 0xEB;
			src[20] = jump_to - 21;

			src = (uint8_t*)(sign + 2);
			src[0] = 0xEB;
			src[1] = -(jump_from + 4);

			patched++;
		}
		else if ((*((uint8_t*)(sign - 0x1E)) == 0x77) && (*((uint8_t*)(sign - 3)) == 0x83))
		{
			jump_from = 0x1E;
			jump_to = 0x2E;

			goto patched_var5;
		}
	}

	SignDecRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "F0 ? 0F ? ? ? 41 FF ? 41 F7 ? FF 03 00 00");

	// or r??, FF
	// lock xadd qword ptr ss:[r??+38], r??
	// dec r??
	// test r??, 3FF
	// 
	// ^ ^ converting the bottom to the top ^ ^
	// 
	// lock xadd dword ptr ss:[r??+38], e??
	// dec e??
	// test e??, 3FF

	for (auto sign : SignDecRef)
	{
		if (*((uint8_t*)(sign - 0x1A)) == 0x77)
		{
			jump_from = 0x1A;
			jump_to = 0x2A;

		patched_var6:

			src = (uint8_t*)(sign - jump_from);
			memcpy_s(szbuff, 64, (uint8_t*)sign, 0x10);
			memset(src, 0x90, jump_to);

			src[0] = 0x49;
			src[1] = 0x83;
			src[2] = szbuff[8];
			src[3] = 0xFF;

			src[4] = 0xF0;
			src[5] = szbuff[1] == 0x44 ? 0x4C : 0x49;
			memcpy_s(src + 6, 0x10, szbuff + 2, 4);
			src[10] = 0x49;
			memcpy_s(src + 11, 0x10, szbuff + 7, 2);
			src[13] = 0x49;
			memcpy_s(src + 14, 0x10, szbuff + 10, 2);
			memcpy_s(src + 16, 0x10, &refmask, sizeof(refmask));
			src[20] = 0xEB;
			src[21] = jump_to - 22;

			src = (uint8_t*)(sign + 2);
			src[0] = 0xEB;
			src[1] = -(jump_from + 4);

			patched++;
		}
		else if ((*((uint8_t*)(sign - 0x1E)) == 0x77) && (*((uint8_t*)(sign - 3)) == 0x83))
		{
			jump_from = 0x1E;
			jump_to = 0x2E;

			goto patched_var6;
		}
	}

	SignDecRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "F0 0F ? ? ? FF ? F7 ? FF 03 00 00");

	for (auto sign : SignDecRef)
	{
		if (*((uint8_t*)(sign - 0x1A)) == 0x77)
		{
			jump_from = 0x1A;
			jump_to = 0x27;

		patched_var7:

			src = (uint8_t*)(sign - jump_from);
			memcpy_s(szbuff, 64, (uint8_t*)sign, 0xD);
			memset(src, 0x90, jump_to);

			src[0] = 0x48;
			src[1] = 0x83;
			src[2] = szbuff[6];
			src[3] = 0xFF;

			src[4] = 0xF0;
			src[5] = 0x48;
			memcpy_s(src + 6, 0x10, szbuff + 1, 4);
			src[10] = 0x48;
			memcpy_s(src + 11, 0x10, szbuff + 5, 2);
			src[13] = 0x48;
			memcpy_s(src + 14, 0x10, szbuff + 7, 2);
			memcpy_s(src + 16, 0x10, &refmask, sizeof(refmask));
			src[20] = 0xEB;
			src[21] = jump_to - 22;

			src = (uint8_t*)(sign + 2);
			src[0] = 0xEB;
			src[1] = -(jump_from + 4);

			patched++;
		}
		else if (*((uint8_t*)(sign - 0x18)) == 0x77)
		{
			jump_from = 0x18;
			jump_to = 0x25;

			goto patched_var7;
		}
	}

	SignDecRef = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase, "8B ? F0 ? 0F ? ? ? FF ? ? FF 03 00 00");

	for (auto sign : SignDecRef)
	{
		if (*((uint8_t*)(sign - 0x18)) == 0x77)
		{
			jump_from = 0x18;
			jump_to = 0x27;

			src = (uint8_t*)(sign - jump_from);
			memcpy_s(szbuff, 64, (uint8_t*)sign, 0xF);
			memset(src, 0x90, jump_to);

			src[0] = 0x48;
			src[1] = 0x83;
			src[2] = szbuff[9];
			src[3] = 0xFF;

			src[4] = 0xF0;
			src[5] = szbuff[3] == 0x41 ? 0x49 : 0x4C;
			memcpy_s(src + 6, 0xF, szbuff + 4, 4);
			src[10] = 0x48;
			memcpy_s(src + 11, 0xF, szbuff + 8, 2);
			src[13] = 0x48;
			src[14] = szbuff[10];
			memcpy_s(src + 15, 0xF, &refmask, sizeof(refmask));
			src[19] = 0xEB;
			src[20] = jump_to - 21;

			src = (uint8_t*)(sign + 2);
			src[0] = 0xEB;
			src[1] = -(jump_from + 4);

			patched++;
		}
	}

	auto sign = OFFSET(0xDAFE40, 0);
	jump_from = 0x1A;
	jump_to = 0x30;

	src = (uint8_t*)(sign - jump_from);
	memcpy_s(szbuff, 64, (uint8_t*)sign, 0x16);
	memset(src, 0x90, jump_to);

	src[0] = 0x48;
	memcpy_s(src + 1, 0x16, szbuff, 3);
	src[4] = 0xF0;
	src[5] = 0x48;
	memcpy_s(src + 6, 0x16, szbuff + 4, 4);
	memcpy_s(src + 10, 0x16, szbuff + 8, 7);
	src[17] = 0x48;
	memcpy_s(src + 18, 0x16, szbuff + 15, 2);
	src[20] = 0x48;
	src[21] = szbuff[17];
	memcpy_s(src + 22, 0x16, &refmask, sizeof(refmask));
	src[26] = 0xEB;
	src[27] = jump_to - 28;

	src = (uint8_t*)(sign + 2);
	src[0] = 0xEB;
	src[1] = -(jump_from + 4);

	patched++;

	sign = OFFSET(0xF70DEF, 0);
	jump_from = 0x1A;
	jump_to = 0x28;

	src = (uint8_t*)(sign - jump_from);
	memcpy_s(szbuff, 64, (uint8_t*)sign, 0xE);
	memset(src, 0x90, jump_to);

	src[0] = 0x48;
	src[1] = 0x83;
	src[2] = szbuff[7];
	src[3] = 0xFF;

	src[4] = 0xF0;
	src[5] = 0x49;
	memcpy_s(src + 6, 0xE, szbuff + 2, 4);
	src[10] = 0x48;
	memcpy_s(src + 11, 0xE, szbuff + 6, 2);
	src[13] = 0x48;
	memcpy_s(src + 14, 0xE, szbuff + 8, 2);
	memcpy_s(src + 16, 0xE, &refmask, sizeof(refmask));
	src[20] = 0xEB;
	src[21] = jump_to - 22;

	src = (uint8_t*)(sign + 2);
	src[0] = 0xEB;
	src[1] = -(jump_from + 4);

	patched++;

	sign = OFFSET(0xDFBF5B, 0);
	jump_from = 0x1A;
	jump_to = 0x2B;

	src = (uint8_t*)(sign - jump_from);
	memcpy_s(szbuff, 64, (uint8_t*)sign, 0x11);
	memset(src, 0x90, jump_to);

	src[0] = 0x48;
	memcpy_s(src + 1, 0x11, szbuff, 3);
	src[4] = 0xF0;
	src[5] = 0x49;
	memcpy_s(src + 6, 0x11, szbuff + 5, 5);
	src[11] = 0x48;
	memcpy_s(src + 12, 0x11, szbuff + 10, 2);
	src[14] = 0x48;
	src[15] = szbuff[12];
	memcpy_s(src + 16, 0x11, &refmask, sizeof(refmask));
	src[20] = 0xEB;
	src[21] = jump_to - 22;

	src = (uint8_t*)(sign + 2);
	src[0] = 0xEB;
	src[1] = -(jump_from + 4);

	sign = OFFSET(0x112F584, 0);
	jump_from = 0x1A;
	jump_to = 0x2B;

	src = (uint8_t*)(sign - jump_from);
	memcpy_s(szbuff, 64, (uint8_t*)sign, 0x11);
	memset(src, 0x90, jump_to);

	src[0] = 0x48;
	memcpy_s(src + 1, 0x11, szbuff, 3);
	src[4] = 0xF0;
	src[5] = 0x49;
	memcpy_s(src + 6, 0x11, szbuff + 5, 4);
	src[10] = 0x48;
	memcpy_s(src + 11, 0x11, szbuff + 9, 2);
	src[13] = 0x48;
	memcpy_s(src + 14, 0x11, szbuff + 11, 2);
	memcpy_s(src + 16, 0x11, &refmask, sizeof(refmask));
	src[20] = 0xEB;
	src[21] = jump_to - 22;

	src = (uint8_t*)(sign + 2);
	src[0] = 0xEB;
	src[1] = -(jump_from + 4);

	patched += 2;
	
	_F4CKMSG("[F4CK]		BSHandleRefObject::DecRef");
	_F4CKMSG_FMT("[F4CK]			Patched: %d", patched);
}

VOID FIXAPI Fix_HandleRefPatch(VOID)
{
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

	// Conversion BSHandleRefObject::IncRef and BSHandleRefObject::DecRef to 64bit

	XUtil::UnlockWP(g_CodeBase, g_CodeEnd - g_CodeBase);

	F_IncRefPatch();
	F_DecRefPatch();

	XUtil::LockWP(g_CodeBase, g_CodeEnd - g_CodeBase);
}