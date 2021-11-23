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

#include "Editor.h"
#include "ActorWindow.h"
#include "../../StdAfx.h"

namespace ActorWindow
{
	Core::Classes::UI::CUICustomWindow ActorWindow;

	DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID)
	{
		return ActorWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return ActorWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		if (Message == WM_INITDIALOG) {
			ActorWindow = DialogHwnd;
			
			// no resize window
			LONG lStyle = GetWindowLongPtrA(DialogHwnd, GWL_STYLE) | WS_BORDER | WS_SYSMENU;
			if ((lStyle & WS_THICKFRAME) == WS_THICKFRAME)
				SetWindowLongPtrA(DialogHwnd, GWL_STYLE, lStyle & ~WS_THICKFRAME);
		}
		else if (Message == WM_CLOSE) {
			hk_SendMessageA(DialogHwnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
			return S_OK;
		}
		else if (Message == WM_MOVE) {
			// fix flick
			return S_OK;
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}