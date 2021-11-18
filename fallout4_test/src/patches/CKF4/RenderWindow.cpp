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
#include "CellViewWindow.h"
#include "MainWindow.h"

#include "../../StdAfx.h"

namespace RenderWindow
{
	IPicker Picker;
	DLGPROC OldDlgProc;
	Core::Classes::UI::CUICustomWindow RenderViewWindow;

	BOOL IPicker::QAdd(TESObjectREFR* form) {
		return forms.Push(form);
	}

	BOOL IPicker::QRemove(TESObjectREFR* form) {
		return forms.Remove(forms.GetItemIndex(form));
	}

	VOID IPicker::QDump(VOID) {
		DWORD count = forms.QSize();
		CHAR szBuf[2048];

		for (DWORD i = 0; i < count; i++) {
			forms[i]->DebugInfo(szBuf, 2048);
			LogWindow::Log("%i: %s", i, szBuf);
		}
	}

	VOID IPicker::hk_Add(HWND hCtlWnd, INT index, BOOL no_unselect) {
		if (CellViewWindow::CellViewWindowControls.Lst2.Handle == hCtlWnd) {
			TESObjectREFR* form = NULL;

			LVITEMA item = { 0 };
			item.mask = LVIF_PARAM | LVFIF_TEXT;
			item.iItem = index;

			if (ListView_GetItem(hCtlWnd, &item))
				form = (TESObjectREFR*)item.lParam;
			
			if (!no_unselect)
				ListView_SetItemState(hCtlWnd, index, 0, LVIS_SELECTED);

			if (index > -1) {
				if (form && Picker.QAdd(form)) {
					ListView_EnsureVisible(hCtlWnd, index, FALSE);
					ListView_SetItemState(hCtlWnd, index, LVIS_SELECTED, LVIS_SELECTED);
				}
			}
		}
		else {
			if (!no_unselect)
				ListView_SetItemState(hCtlWnd, index, 0, LVIS_SELECTED);
			
			if (index > -1) {
				ListView_EnsureVisible(hCtlWnd, index, FALSE);
				ListView_SetItemState(hCtlWnd, index, LVIS_SELECTED, LVIS_SELECTED);
			}
		}			
	}

	VOID IPicker::hk_Remove(HWND hCtlWnd, TESObjectREFR* form) {
		if (CellViewWindow::CellViewWindowControls.Lst2.Handle == hCtlWnd) {
			if (Picker.QRemove(form))
				goto l_find_form;
		}
		else {
l_find_form:
			LV_FINDINFOA find = { 0 };
			find.flags = LVFI_PARAM;
			find.lParam = (LPARAM)form;

			auto ind = ListView_FindItem(hCtlWnd, -1, &find);
			if (ind != -1)
				ListView_SetItemState(hCtlWnd, ind, 0, LVIS_SELECTED);
		}
		
	}

	struct RenderWindowParams_t {
		BOOL IsCollisionView;

		// Some important variables for determining the size of the drawing area
		DWORD dw_A5B7AF8;
		DWORD dw_A5B7AFC;
		DWORD dw_A5B7B00;
		DWORD dw_A5B7B04;
	} RenderWindowParams;

	HWND GetWindow(VOID) {
		return RenderViewWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& GetWindowObj(VOID) {
		return RenderViewWindow;
	}

	BOOL IsCollisionView(VOID) {
		return RenderWindowParams.IsCollisionView;
	}

	VOID SetCollisionView(const BOOL Value) {
		RenderWindowParams.IsCollisionView = Value;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		if (Message == WM_INITDIALOG) {
			RenderViewWindow = DialogHwnd;
			RenderWindowParams.IsCollisionView = FALSE;
		}
		else if (Message == WM_KEYUP) {
			auto ctrl = HIBYTE(GetKeyState(VK_CONTROL)) & 0x80;

			if (ctrl) {
				if (wParam == '5')
					// Fake click fog
					MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(23).Click();
			}
			else {
				if (wParam == 'M')
					// If you click on M, the menu will still have the previous state, we will fix this. 
					// However, in fact, there should be two requests to show or hide, but the second one is ignored and this is good.
					MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(15).Click();
				else if (wParam == 'S')
					// Fix that only worked with the menu
					MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(17).Click();
			}
		}
		else if (Message == WM_ACTIVATE) {
			WORD fActive = (WORD)LOWORD(wParam);
			if (fActive == WA_INACTIVE) {
				RenderWindowParams.dw_A5B7AF8 = *(PDWORD)(OFFSET(0xA5B7AF8, 0));
				RenderWindowParams.dw_A5B7AFC = *(PDWORD)(OFFSET(0xA5B7AFC, 0));
				RenderWindowParams.dw_A5B7B00 = *(PDWORD)(OFFSET(0xA5B7B00, 0));
				RenderWindowParams.dw_A5B7B04 = *(PDWORD)(OFFSET(0xA5B7B04, 0));
			}
			else {
				*(PDWORD)(OFFSET(0xA5B7AF8, 0)) = RenderWindowParams.dw_A5B7AF8;
				*(PDWORD)(OFFSET(0xA5B7AFC, 0)) = RenderWindowParams.dw_A5B7AFC;
				*(PDWORD)(OFFSET(0xA5B7B00, 0)) = RenderWindowParams.dw_A5B7B00;
				*(PDWORD)(OFFSET(0xA5B7B04, 0)) = RenderWindowParams.dw_A5B7B04;
			}
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}