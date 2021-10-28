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

#include "../../patches/CKF4/LogWindow.h"
#include "GameAPI.h"

LPVOID FIXAPI Heap_Allocate(DWORD64 size) {
	((Heap*)OFFSET(0x9EE7E88, 0))->Allocate(size, 0);
}

VOID FIXAPI Heap_Free(LPVOID ptr) {
	((Heap*)OFFSET(0x9EE7E88, 0))->Deallocate(ptr);
}

VOID ConsoleManager::VPrint(LPCSTR fmt, va_list args) {
	LogWindow::LogVa(fmt, args);
}

VOID ConsoleManager::Print(LPCSTR str) {
	LogWindow::Log(str);
}

VOID Console_Print(LPCSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	ConsoleManager::VPrint(fmt, args);
	va_end(args);
}

INT64 GetPerfCounter(VOID)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}
