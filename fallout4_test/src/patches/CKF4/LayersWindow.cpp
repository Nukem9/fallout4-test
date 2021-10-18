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

#include "LayersWindow.h"
#include "LogWindow.h"

#include <windowsx.h>

namespace LayersWindow
{
	DLGPROC OldDlgProc;
	Classes::CUICustomWindow LayersWindow;

	struct LayersWindowParams_t
	{
		Classes::CUIBaseControl PushbuttonNewLayer;
		Classes::CUIBaseControl PushbuttonAddSelection;
		Classes::CUIBaseControl EdittextFilter;
		Classes::CUIBaseControl PushbuttonPlus;
	} LayersWindowParams;

	BOOL WINAPI MoveWindowBody(HWND hWnd, INT X, INT Y, INT nWidth, INT nHeight, BOOL bRepaint) {
		auto cRect = LayersWindow.ClientRect();

		return ::MoveWindow(hWnd, 8, Y - 8, cRect.Width - 16, cRect.Height - Y, bRepaint);
	}

	BOOL WINAPI MoveWindowHeader(HWND hWnd, INT X, INT Y, INT nWidth, INT nHeight, BOOL bRepaint) {
		auto cRect = LayersWindow.ClientRect();
		auto cBounds = LayersWindowParams.PushbuttonPlus.BoundsRect;

		return ::MoveWindow(hWnd, 8, cBounds.Bottom + 8, cRect.Width - 16, nHeight, bRepaint);
	}

	HWND FIXAPI GetWindow(VOID) {
		return LayersWindow.Handle;
	}

	Classes::CUICustomWindow& FIXAPI GetWindowObj(VOID) {
		return LayersWindow; 
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		switch (Message)
		{
		case WM_INITDIALOG:
			LayersWindow = DialogHwnd;
			LayersWindowParams.PushbuttonNewLayer = LayersWindow.GetControl(1124);
			LayersWindowParams.PushbuttonAddSelection = LayersWindow.GetControl(3682);
			LayersWindowParams.EdittextFilter = LayersWindow.GetControl(2581);
			LayersWindowParams.PushbuttonPlus = LayersWindow.GetControl(5593);
			break;
		case WM_GETMINMAXINFO: {
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 350;
			lpMMI->ptMinTrackSize.y = 250;
			}
			return S_OK;
		case WM_SIZE: {
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			auto pbw_1 = (width - 24) >> 1;

			LayersWindowParams.PushbuttonNewLayer.Move(8, 8);
			LayersWindowParams.PushbuttonNewLayer.Width = pbw_1;

			LayersWindowParams.PushbuttonAddSelection.Move(16 + pbw_1, 8);
			LayersWindowParams.PushbuttonAddSelection.Width = pbw_1;

			LayersWindowParams.EdittextFilter.Width = width - (LayersWindowParams.EdittextFilter.Left + 8);
			}
			break;
		default:
			break;
		}	

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}