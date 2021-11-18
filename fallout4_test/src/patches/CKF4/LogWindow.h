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

#pragma once

#include <minwindef.h>

namespace LogWindow
{
	HWND FIXAPI GetWindow(VOID);
	HANDLE FIXAPI GetStdoutListenerPipe(VOID);

	BOOL FIXAPI Initialize(VOID);
	BOOL FIXAPI CreateStdoutListener(VOID);
	VOID FIXAPI LoadWarningBlacklist(VOID);
	BOOL FIXAPI SaveRichTextToFile(LPCSTR _filename);

	LRESULT CALLBACK WndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	VOID FIXAPI LogVa(LPCSTR Format, va_list Va);
	VOID FIXAPI Log(LPCSTR Format, ...);
	VOID FIXAPI LogWarningVa(INT32 Type, LPCSTR Format, va_list& Va);
	VOID FIXAPI LogWarning(INT32 Type, LPCSTR Format, ...);
	VOID FIXAPI LogWarningUnknown1(LPCSTR Format, ...);
	VOID FIXAPI LogWarningUnknown2(INT64 Unused, LPCSTR Format, ...);
	VOID FIXAPI LogAssert(LPCSTR File, INT32 Line, LPCSTR Message, ...);
	VOID FIXAPI LogInsteadOfMsgBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

	VOID FIXAPI LogWcVa(LPCWSTR Format, va_list Va);
	VOID FIXAPI LogWc(LPCWSTR Format, ...);
}