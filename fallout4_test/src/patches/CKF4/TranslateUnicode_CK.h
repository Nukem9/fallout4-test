//////////////////////////////////////////
/*
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

//#define __DBG_TRAN_UNICODE_READ
//#define __DBG_TRAN_UNICODE_WRITE

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "..\..\Common.h"

#include <list>
#include <string>

namespace Core
{
	namespace Patch
	{
		namespace CreationKit
		{
			class CTranslateUnicodeMediator
			{
			private:
				BOOL m_Mode;
				LPSTR m_preaddres;
				std::list<std::string> heap;
#ifdef __DBG_TRAN_UNICODE_WRITE
				std::ofstream out;
#elif __DBG_TRAN_UNICODE_READ
				std::ofstream out;
#endif // __DBG_TRAN_UNICODE
			private:
				BOOL IsValid(LPCSTR addr, const BOOL no_ansi = FALSE);
				LPSTR Utf8ToWinCP(LPCSTR src);
				LPSTR WinCPToUtf8(LPCSTR src);
			public:
				VOID SetMode(const BOOL mode);
				LPSTR Translate(LPCSTR src);
			public:
				CTranslateUnicodeMediator(VOID);
				~CTranslateUnicodeMediator(VOID);
			};

			extern CTranslateUnicodeMediator Translator;
		}
	}
}

namespace Experimental
{
	INT FIXAPI BeginPluginSave(VOID);
	LPVOID FIXAPI EndPluginSave(LPVOID __arg1);
	LPSTR FIXAPI Translate(LPSTR str);

	BOOL WINAPI hk_SetDlgItemTextA(HWND hDlg, INT32 nIDDlgItem, LPCSTR lpString);
	LRESULT WINAPI hk_SendDlgItemMessageA(HWND hDlg, INT32 nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam);
}