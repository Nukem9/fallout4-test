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

#include "RenderWindow.h"
#include "MainWindow.h"
#include "LogWindow.h"

#include <windowsx.h>

namespace RenderWindow
{
	Core::Classes::UI::CUICustomWindow RenderViewWindow;

	struct RenderWindowParams_t
	{
		BOOL IsCollisionView;

		// Some important variables for determining the size of the drawing area
		DWORD dw_A5B7AF8;
		DWORD dw_A5B7AFC;
		DWORD dw_A5B7B00;
		DWORD dw_A5B7B04;
	} RenderWindowParams;

	DLGPROC OldDlgProc;

	HWND GetWindow(void)
	{
		return RenderViewWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& GetWindowObj(void)
	{
		return RenderViewWindow;
	}

	BOOL IsCollisionView(void)
	{
		return RenderWindowParams.IsCollisionView;
	}

	void SetCollisionView(const BOOL Value)
	{
		RenderWindowParams.IsCollisionView = Value;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			RenderViewWindow = DialogHwnd;
			RenderWindowParams.IsCollisionView = FALSE;
		}
		else if (Message == WM_KEYUP)
		{
			auto ctrl = HIBYTE(GetKeyState(VK_CONTROL)) & 0x80;

			if (ctrl)
			{
				if (wParam == '5')
				{
					// Fake click fog

					MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(23).Click();
				}
			}
			else
			{
				if (wParam == 'M')
				{
					// If you click on M, the menu will still have the previous state, we will fix this. 
					// However, in fact, there should be two requests to show or hide, but the second one is ignored and this is good.

					MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(15).Click();
				}
				else if (wParam == 'S')
				{
					// Fix that only worked with the menu

					MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(17).Click();
				}
			}
		}
		else if (Message == WM_ACTIVATE)
		{
			WORD fActive = (WORD)LOWORD(wParam);
			if (fActive == WA_INACTIVE)
			{
				RenderWindowParams.dw_A5B7AF8 = *(PDWORD)(OFFSET(0xA5B7AF8, 0));
				RenderWindowParams.dw_A5B7AFC = *(PDWORD)(OFFSET(0xA5B7AFC, 0));
				RenderWindowParams.dw_A5B7B00 = *(PDWORD)(OFFSET(0xA5B7B00, 0));
				RenderWindowParams.dw_A5B7B04 = *(PDWORD)(OFFSET(0xA5B7B04, 0));
			}
			else
			{
				*(PDWORD)(OFFSET(0xA5B7AF8, 0)) = RenderWindowParams.dw_A5B7AF8;
				*(PDWORD)(OFFSET(0xA5B7AFC, 0)) = RenderWindowParams.dw_A5B7AFC;
				*(PDWORD)(OFFSET(0xA5B7B00, 0)) = RenderWindowParams.dw_A5B7B00;
				*(PDWORD)(OFFSET(0xA5B7B04, 0)) = RenderWindowParams.dw_A5B7B04;
			}
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}