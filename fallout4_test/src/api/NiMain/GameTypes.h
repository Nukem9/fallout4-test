//////////////////////////////////////////
/*
* Fallout 4 Script Extender (F4SE)
* by Ian Patterson, Stephen Abel, and Brendan Borthwick (ianpatt, behippo, and purplelunchbox)
* 
* Contact the F4SE Team
*
* Entire Team
* Send email to team [at] f4se [dot] silverlock [dot] org
*
* Ian (ianpatt)
* Send email to ianpatt+f4se [at] gmail [dot] com
*
* Stephen (behippo)
* Send email to gamer [at] silverlock [dot] org
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

//////////////////////////////////////////
/*
* Copyright (c) 2021 Perchik71 <email:perchik71@outlook.com>
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

#include "GameAPI.h"
#include <string>

#pragma pack(push, 1)

// 04 or 08 depending alignment
struct BSIntrusiveRefCounted {
public:
	volatile DWORD m_refCount;	// 00
};

// 04
struct BSNonReentrantSpinLock {
public:
	volatile DWORD uiLock;		// 00
};

class SimpleLock {
	enum { kFastSpinThreshold = 10000 };

	volatile DWORD threadID;	// 00
	volatile DWORD lockCount;	// 04

public:
	SimpleLock(VOID) : threadID(0), lockCount(0) {}

	VOID Lock(DWORD pauseAttempts = 0);
	VOID Release(VOID);
};

// 08
class BSReadWriteLock {
	enum {
		kFastSpinThreshold	= 10000,
		kLockWrite			= 0x80000000,
		kLockCountMask		= 0xFFFFFFF
	};

	volatile DWORD threadID;	// 00
	volatile DWORD lockValue;	// 04
public:
	BSReadWriteLock(VOID) : threadID(0), lockValue(0) {}

	VOID LockForRead(VOID);					
	VOID LockForWrite(VOID);	
	VOID LockForReadAndWrite(VOID);
	BOOL TryLockForWrite(VOID);
	BOOL TryLockForRead(VOID);
	VOID Unlock(VOID);
};

class BSReadLocker {
public:
	BSReadLocker(BSReadWriteLock* lock) { m_lock = lock; m_lock->LockForRead(); }
	~BSReadLocker(VOID) { m_lock->Unlock(); }
protected:
	BSReadWriteLock* m_lock;
};

class BSWriteLocker {
public:
	BSWriteLocker(BSReadWriteLock* lock) { m_lock = lock; m_lock->LockForWrite(); }
	~BSWriteLocker(VOID) { m_lock->Unlock(); }

protected:
	BSReadWriteLock* m_lock;
};

class BSReadAndWriteLocker {
public:
	BSReadAndWriteLocker(BSReadWriteLock* lock) { m_lock = lock; m_lock->LockForReadAndWrite(); }
	~BSReadAndWriteLocker(VOID) { m_lock->Unlock(); }

protected:
	BSReadWriteLock* m_lock;
};

class SimpleLocker {
public:
	SimpleLocker(SimpleLock* dataHolder) { m_lock = dataHolder; m_lock->Lock(); }
	~SimpleLocker(VOID) { m_lock->Release(); }

protected:
	SimpleLock* m_lock;
};

class StringCache {
public:
	struct Entry {
		enum {
			kState_RefcountMask =	0x3FFF,
			kState_External =		0x4000,
			kState_Wide =			0x8000
		};

		Entry*	next;		// 00
		DWORD64	state;		// 08 - refcount, hash, flags
		// with 4000 flag, is 0
		DWORD64	length;		// 10
		// with 4000 flag, is NULL
		LPSTR	data;		// 18
		// Only with the 0x4000 flag, otherwise there is none.
		Entry*	externData;	// 20

		BOOL IsWide(VOID) const {
			const Entry* iter = this;

			while(iter->state & kState_External)
				iter = iter->externData;

			if((iter->state & kState_Wide) == kState_Wide)
				return TRUE;

			return FALSE;
		}

		template<typename T>
		const T* Get(VOID) const {
			const Entry* iter = this;

			while(iter->state & kState_External)
				iter = iter->externData;

			return (const T*)iter->data;
		}
	};

	struct Ref {
		Entry* data;

		virtual ~Ref(VOID);					// 000
		virtual UINT ToUInt(VOID) const;	// 008	
		virtual VOID VT_Func2(VOID);		// 010	
		virtual VOID VT_Func3(VOID);		// 018	<=ID ??? and FFFFFF3
		// Returns and converts a string if conversion is allowed
		virtual LPCSTR ToStr(VOID) const;	// 020

		BOOL operator==(const Ref& lhs) const { return data == lhs.data; }
		BOOL operator<(const Ref& lhs) const { return data < lhs.data; }

		//
		// It is better not to use it....
		// Does not convert the string, just returns the string as is, but it may already be converted
		//
		LPCSTR c_str(VOID) const { return operator LPCSTR(); }
		operator LPCSTR(VOID) const { return data ? data->Get<CHAR>() : NULL; }
	};

	struct Lock {
		DWORD unk00;	// 00 - set to 80000000 when locked
		DWORD pad04;	// 04
		DWORD64 pad08;	// 08
	};

	Entry*	lut[0x10000];	// 00000
	Lock	lock[0x80];		// 80000
	BYTE	isInit;			// 80800
};

typedef StringCache::Entry BSEntryString;
typedef StringCache::Ref BSFixedString;

class BSString {
public:
	BSString(VOID);
	~BSString(VOID);
public:
	LPCSTR Get(VOID) const;
private:
	LPCSTR	m_data;		// 00
	WORD	m_dataLen;	// 08
	WORD	m_bufLen;	// 0A
	DWORD	pad0C;		// 0C
};

template <typename T>
class SafeDataHolder {
protected:
	SimpleLock m_lock;
public:
	T m_data;
public:
	inline VOID Lock(VOID) { m_lock.Lock(); }
	inline VOID Release(VOID) { m_lock.Release(); }
};

#pragma pack(pop)