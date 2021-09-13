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

#include "..\common.h"

namespace kernel
{
	// Qirix/embree project
	/*! set the affinity of a given thread */
	void setAffinity(HANDLE thread, int affinity)
	{
#if (_WIN32_WINNT >= 0x0601)
		int groups = GetActiveProcessorGroupCount();
		int totalProcessors = 0, group = 0, number = 0;
		for (int i = 0; i < groups; i++) {
			int processors = GetActiveProcessorCount(i);
			if (totalProcessors + processors > affinity) {
				group = i;
				number = (int)affinity - totalProcessors;
				break;
			}
			totalProcessors += processors;
		}

		GROUP_AFFINITY groupAffinity;
		groupAffinity.Group = (WORD)group;
		groupAffinity.Mask = (KAFFINITY)(uint64_t(1) << number);
		groupAffinity.Reserved[0] = 0;
		groupAffinity.Reserved[1] = 0;
		groupAffinity.Reserved[2] = 0;
		if (!SetThreadGroupAffinity(thread, &groupAffinity, NULL))
			throw std::runtime_error("cannot set thread group affinity");

		PROCESSOR_NUMBER processorNumber;
		processorNumber.Group = group;
		processorNumber.Number = number;
		processorNumber.Reserved = 0;
		if (!SetThreadIdealProcessorEx(thread, &processorNumber, NULL))
			throw std::runtime_error("cannot set thread ideal processor");
#else
		if (!SetThreadAffinityMask(thread, DWORD_PTR(uint64(1) << affinity)))
			throw std::runtime_error("cannot set thread affinity mask");
		if (SetThreadIdealProcessor(thread, (DWORD)affinity) == (DWORD)-1)
			throw std::runtime_error("cannot set thread ideal processor");
#endif
	}
}


/*
==================
hk_SetThreadPriority

Replacement WINAPI SetThreadPriority
==================
*/
BOOL WINAPI hk_SetThreadPriority(HANDLE hThread, int nPriority)
{
	// Don't allow a priority below normal - Skyrim doesn't have many "idle" threads
	return SetThreadPriority(hThread, std::max(THREAD_PRIORITY_NORMAL, nPriority));
}


/*
==================
hk_SetThreadAffinityMask

Replacement WINAPI SetThreadAffinityMask
==================
*/
DWORD_PTR WINAPI hk_SetThreadAffinityMask(HANDLE hThread, DWORD_PTR dwThreadAffinityMask)
{
//	kernel::setAffinity(hThread, (rand() % 16) + 1);
	// Don't change anything
	return 0xFFFFFFFF;
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
	PatchIAT(hk_SetThreadPriority, "kernel32.dll", "SetThreadPriority");
	PatchIAT(hk_SetThreadAffinityMask, "kernel32.dll", "SetThreadAffinityMask");
	// I removed the interception of the Sleep function it caused overloads

	// no abort/retry/fail errors
	SetErrorMode(SEM_FAILCRITICALERRORS);

	// Process that has priority above NORMAL_PRIORITY_CLASS but below HIGH_PRIORITY_CLASS.
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
}
