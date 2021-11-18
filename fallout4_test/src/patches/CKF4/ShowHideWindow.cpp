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

#include "ShowHideWindow.h"
#include "UICheckboxControl.h"
#include "Editor.h"

#include "../../StdAfx.h"

namespace ShowHideWindow
{
	namespace Classes = Core::Classes::UI;
	Classes::CUICustomWindow ShowHideWindow;
	static BOOL Initialize = FALSE;

	DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID)
	{
		return ShowHideWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return ShowHideWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			if (!Initialize)
			{
				Initialize = TRUE;
				ShowHideWindow = DialogHwnd;
			}			
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}