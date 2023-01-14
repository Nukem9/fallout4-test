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
#pragma pack(push, 1)

#include "..\[Common]\CommInc.h"

class NiBinaryStream {
public:
	virtual ~NiBinaryStream();
public:
	virtual BOOL IsValid(VOID) = 0;
};

class NiFile : public NiBinaryStream {
public:
	virtual ~NiFile();
public:
	enum FileModes {
		kFileMode_ReadOnly = 0,
		kFileMode_WriteOnly = 1,
		kFileMode_AppendOnly = 2,
	};
protected:
	// members
	/*000*/ // --vtbls
	/*008*/ DWORD64 _Pos;				// init to 0
	/*010*/ DWORD64 _BufferAllocSize;	// init to 0x800	(sent for reading/writing)
	/*018*/ DWORD64 _BufferReadSize;	// init to 0		(answered by the function)
	/*020*/ DWORD64 _BufferOffset;		// init to 0
	/*028*/ DWORD64 _SizeBuffer;		// init to 0
	/*030*/ LPVOID _Buffer;				// allocated/deallocated on form heap by constructor/destructor
	/*038*/ DWORD64 _dwUnk38;			
	/*040*/ HANDLE _FileHandle;
	/*048*/ CHAR _FileName[MAX_PATH];
	/*14C*/ BOOL _Good;					// true if file is exist or no error
public:
	INLINE DWORD64 GetPosition(VOID) const { return _Pos; }
	INLINE DWORD64 GetOffsetOfTheBuffer(VOID) const { return _BufferOffset; }
	INLINE DWORD64 GetSizeBuffer(VOID) const { return _SizeBuffer; }
	INLINE HANDLE GetHandle(VOID) const { return _FileHandle; }
	INLINE LPCSTR GetFileName(VOID) const { return _FileName; }
	INLINE BOOL IsGood(VOID) const { return _Good; }
public:
	virtual BOOL IsValid(VOID) { return IsGood(); }
};

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

	VOID LockForReadAndWrite(VOID);
	BOOL TryLockForWrite(VOID);
	BOOL TryLockForRead(VOID);
	VOID Unlock(VOID);
};

class BSReadLocker {
public:
	BSReadLocker(BSReadWriteLock* lock) { m_lock = lock; m_lock->TryLockForRead(); }
	~BSReadLocker(VOID) { m_lock->Unlock(); }
protected:
	BSReadWriteLock* m_lock;
};

class BSWriteLocker {
public:
	BSWriteLocker(BSReadWriteLock* lock) { m_lock = lock; m_lock->TryLockForWrite(); }
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

template <typename T>
class SafeDataHolder {
protected:
	SimpleLock m_lock;
public:
	T m_data;
public:
	INLINE VOID Lock(VOID) { m_lock.Lock(); }
	INLINE VOID Release(VOID) { m_lock.Release(); }
};

#pragma pack(pop)