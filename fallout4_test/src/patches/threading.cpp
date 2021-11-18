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

#include "..\StdAfx.h"
#include "CKF4\LogWindow.h"

#define FALLOUT4_THREADPATCH_DEBUG 0

static BOOL bEnabledMultithread = FALSE;
static WORD wActiveProcessorGroups = 0;
static DWORD wTotalActiveProcessors = 0;
static HANDLE hPrevThreadWichSetPriority = 0;

VOID FIXAPI SetAffinity(HANDLE thread, int affinity)
{
	struct TThreadInfo {
		WORD wProcessorGroup;
		DWORD wProcessorId;
	};

#if (_WIN32_WINNT >= 0x0601)
	TThreadInfo info = { 0 };
	DWORD totalProcessors = 0;
	for (WORD i = 0; i < wActiveProcessorGroups; i++) {
		DWORD processors = GetActiveProcessorCount(i);
		if (totalProcessors + processors > (DWORD)affinity) {
			info.wProcessorGroup = i;
			info.wProcessorId = (DWORD)(affinity - totalProcessors);
			break;
		}
		totalProcessors += processors;
	}

#if FALLOUT4_THREADPATCH_DEBUG
	_MESSAGE_FMT("Select processor group %d and processor %d", info.wProcessorGroup, info.wProcessorId);
#endif

	GROUP_AFFINITY groupAffinity = { 0 };
	groupAffinity.Group = (WORD)info.wProcessorGroup;
	groupAffinity.Mask = (KAFFINITY)(uint64_t(1) << info.wProcessorId);
	if (!SetThreadGroupAffinity(thread, &groupAffinity, NULL)) {
		if (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4)
			throw std::runtime_error("cannot set thread group affinity");
		else
			_MESSAGE("Cannot set thread group affinity");
	}

	PROCESSOR_NUMBER processorNumber = { 0 };
	processorNumber.Group = info.wProcessorGroup;
	processorNumber.Number = info.wProcessorId;
	if (!SetThreadIdealProcessorEx(thread, &processorNumber, NULL)) {
		if (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4)
			throw std::runtime_error("cannot set thread ideal processor");
		else
			_MESSAGE("Cannot set thread group affinity");
	}
#else
	if (!SetThreadAffinityMask(thread, DWORD_PTR(uint64(1) << affinity))) {
		if (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4)
			throw std::runtime_error("cannot set thread affinity mask");
		else
			_MESSAGE("Cannot set thread affinity mask");
	}
	if (SetThreadIdealProcessor(thread, (DWORD)affinity) == (DWORD)-1) {
		if (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4)
			throw std::runtime_error("cannot set thread ideal processor");
		else
			_MESSAGE("Cannot set thread ideal processor");
	}
#endif
}


/*
==================
hk_SetThreadPriority

Replacement WINAPI SetThreadPriority
When calling the function, the default value is set to THREAD_PRIORITY_BELOW_NORMAL.
==================
*/
BOOL WINAPI hk_SetThreadPriority(HANDLE hThread, int nPriority)
{
	// For some reason, the developers call this twice.... why?
	if (hPrevThreadWichSetPriority == hThread)
		return TRUE;
	hPrevThreadWichSetPriority = hThread;

#if FALLOUT4_THREADPATCH_DEBUG
	_MESSAGE("DEBUG: Set thread priority");
	_MESSAGE_FMT("	Handle: %d", (int64_t)hThread);
	_MESSAGE_FMT("	Default value: %d", nPriority);
#endif

	// Don't allow a priority below normal
	auto result = SetThreadPriority(hThread, std::max(THREAD_PRIORITY_NORMAL, nPriority));

#if FALLOUT4_THREADPATCH_DEBUG
	_MESSAGE((result) ? "	Success" : "	Failed");
#endif

	return result;
}


/*
==================
hk_SetThreadAffinityMask

Replacement WINAPI SetThreadAffinityMask
According to the indications, it function is not used, but it is present in the code.
==================
*/
DWORD_PTR WINAPI hk_SetThreadAffinityMask(HANDLE hThread, DWORD_PTR dwThreadAffinityMask)
{
	// Don't change anything
	return 0xFFFFFFFF;
}


/*
==================
hk_CreateThread

Replacement WINAPI CreateThread
Creation Kit hangs mainly on two processors, I want to fix it.
==================
*/
HANDLE WINAPI hk_CreateThread(
	LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	SIZE_T                  dwStackSize,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	__drv_aliasesMem LPVOID lpParameter,
	DWORD                   dwCreationFlags,
	LPDWORD                 lpThreadId
) {
	auto thread = CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
	if (thread) {
		if (bEnabledMultithread) {
			auto threadId = GetThreadId(thread);
			// generate random id from threadId
			auto processorId = ((threadId + 1) / wTotalActiveProcessors) % wTotalActiveProcessors;

#if FALLOUT4_THREADPATCH_DEBUG
			_MESSAGE_FMT("	Handle: %d", (int64_t)thread);
			_MESSAGE_FMT("	ID: %d", threadId);
			_MESSAGE_FMT("	Processor ID: %d", processorId);
#endif

			// Sets a preferred processor for a thread. The system schedules threads on their preferred processors whenever possible.
			SetAffinity(thread, processorId);
			// Not all threads are given priority.
			// This will trigger even more function calls, but this is controlled.
			hk_SetThreadPriority(thread, THREAD_PRIORITY_NORMAL);
		}
	}

	return thread;
}


/*
==================
hk_Sleep

Replacement WINAPI Sleep
Bethesda's spinlock calls Sleep(0) every iteration until 10,000. Then it
uses Sleep(1). Even with 0ms waits, there's a tiny performance penalty.
PS: Nukem9 was right, it is necessary and does not seem to lead to failures and SpinTime according to Intel VTune.
==================
*/
VOID WINAPI hk_Sleep(DWORD dwMilliseconds)
{
	if (dwMilliseconds == 0)
		return;

	SleepEx(dwMilliseconds, FALSE);
}


/*
==================
Fix_PatchThreading

Implements the code in the process
Changes the behavior of the main thread of the application and the OS
==================
*/
VOID FIXAPI Fix_PatchThreading(VOID)
{
	if (bEnabledMultithread = (g_LoadType == GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4) &&
		g_INI->GetBoolean("CreationKit", "EnableThreadAffinity", FALSE); bEnabledMultithread) {
		wActiveProcessorGroups = GetActiveProcessorGroupCount();
		wTotalActiveProcessors = 0;
		for (auto i = 0; i < wActiveProcessorGroups; i++)
			wTotalActiveProcessors += GetActiveProcessorCount(i);

#if FALLOUT4_THREADPATCH_DEBUG
		_MESSAGE("DEBUG: Create thread");
		_MESSAGE_FMT("	Active processor groups: %d", wActiveProcessorGroups);
		_MESSAGE_FMT("	Total active processors: %d", wTotalActiveProcessors);
#endif

		// only for quad-core processor or higher
		if (wTotalActiveProcessors < 4)
			bEnabledMultithread = FALSE;
		else {
			auto currentThread = GetCurrentThread();
			// The main thread is already declared as time critical 
			//hk_SetThreadPriority(currentThread, THREAD_PRIORITY_TIME_CRITICAL);
			// Bind the main thread to the third processor
			SetAffinity(currentThread, 2);
		}
	}

	PatchIAT(hk_SetThreadPriority, "kernel32.dll", "SetThreadPriority");
	PatchIAT(hk_CreateThread, "kernel32.dll", "CreateThread");
	PatchIAT(hk_SetThreadAffinityMask, "kernel32.dll", "SetThreadAffinityMask");
	PatchIAT(hk_Sleep, "kernel32.dll", "Sleep");

	// Instead of 500 ms, I set 1000 ms, this removes SpinTime and reduces the time for waiting for threads.
	// However, this is on my processor, well, I will proceed from the readings at myself.
	XUtil::PatchMemory(OFFSET(0x20437A2, 0), { 0xE8, 0x03 });

	// The system does not display the critical-error-handler message box. 
	// Instead, the system sends the error to the calling process.
	// Best practice is that all applications call the process - wide SetErrorMode function with a parameter of SEM_FAILCRITICALERRORS at startup.
	// This is to prevent error mode dialogs from hanging the application.
	SetErrorMode(SEM_FAILCRITICALERRORS);

	// Sets the priority class for the specified process.
	// This value together with the priority value of each thread of the process determines each thread's base priority level.
	if (g_LoadType == GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4) {
		// Process that has priority above NORMAL_PRIORITY_CLASS but below HIGH_PRIORITY_CLASS.
		SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
	}
	else
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
}