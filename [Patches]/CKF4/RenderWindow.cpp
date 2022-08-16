//////////////////////////////////////////
/*
* Copyright (c) 2020-2022 Perchik71 <email:perchik71@outlook.com>
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

#include "..\..\[Common]\StdAfx.h"
#include "..\..\[EditorAPI]\TESRender.h"

#include "UITheme/VarCommon.h"

extern BOOL IsRenderCaptureMouse;

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

	const BSTArray<TESObjectREFR*>* IPicker::GetForms(VOID) const {
		auto arr = const_cast<BSTArray<TESObjectREFR*>*>(&forms);
		auto listREFRhandle = CellViewWindow::CellViewWindowControls.Lst2.Handle;

		arr->Clear();

		// Get the first selected item
		int iPos = ListView_GetNextItem(listREFRhandle, -1, LVNI_SELECTED);
		while (iPos != -1) {
			// iPos is the index of a selected item
			
			TESObjectREFR* form = NULL;

			LVITEMA item = { 0 };
			item.mask = LVIF_PARAM;
			item.iItem = iPos;

			if (ListView_GetItem(listREFRhandle, &item))
				form = (TESObjectREFR*)item.lParam;

			if (form)
				arr->Push(form);

			// Get the next selected item
			iPos = ListView_GetNextItem(listREFRhandle, iPos, LVNI_SELECTED);
		}

		return arr;
	}

	/*VOID IPicker::hk_Add(HWND hCtlWnd, INT index, BOOL no_unselect) {
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
		
	}*/

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

	BGSRenderWindow* FIXAPI GetRender(VOID) {
		return BGSRenderWindow::GetInstance();
	}

	BOOL IsCollisionView(VOID) {
		return RenderWindowParams.IsCollisionView;
	}

	VOID SetCollisionView(const BOOL Value) {
		RenderWindowParams.IsCollisionView = Value;
	}

	VOID FIXAPI UpdatePositionAndSize(VOID) {
		auto MainWnd = MainWindow::GetWindowObj();
		constexpr auto toolY = 28;
		constexpr auto statY = 26;
		auto rcClient = MainWnd.ClientRect();

		SetWindowPos(RenderViewWindow.Handle, NULL,
			0, toolY, rcClient.Width, rcClient.Height - (statY + toolY), 0x0044);
	}

	VOID FIXAPI RNDWND_AttachMainWindow(HWND DialogHwnd) {
		auto MainWnd = MainWindow::GetWindowObj();
		if (!IsWindow(MainWnd.Handle))
			return;

		SetParent(DialogHwnd, MainWnd.Handle);
		SetWindowLongA(DialogHwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_OVERLAPPED);

		UpdatePositionAndSize();
	}

	constexpr static LONG TOPEXTENDWIDTH = 27;
	constexpr static LONG BOTTOMEXTENDWIDTH = 20;
	constexpr static LONG LEFTEXTENDWIDTH = 8;
	constexpr static LONG RIGHTEXTENDWIDTH = 8;

	// Paint the title on the custom frame.
	void PaintCustomCaption(HWND hWnd, HDC hdc)
	{
		Core::Classes::UI::CUIBaseWindow RenderWindow(hWnd);
		Core::Classes::UI::CUICanvas Canvas(hdc);
		
		auto rcClient = RenderWindow.ClientRect();

		Canvas.Fill(rcClient, Core::UI::Theme::ThemeColor_Default);
		Canvas.Frame(rcClient, Core::UI::Theme::GetThemeSysColor(Core::UI::Theme::ThemeColor_Divider_Color_Ver2));
	}

	// Hit test the frame for resizing and moving.
	LRESULT HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		// Get the point coordinates for the hit test.
		POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		// Get the window rectangle.
		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);

		// Get the frame rectangle, adjusted for the style without a caption.
		RECT rcFrame = { 0 };
		AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);

		// Determine if the hit test is for resizing. Default middle (1,1).
		USHORT uRow = 1;
		USHORT uCol = 1;
		BOOL fOnResizeBorder = FALSE;

		// Determine if the point is at the top or bottom of the window.
		if (ptMouse.y >= rcWindow.top && ptMouse.y < rcWindow.top + TOPEXTENDWIDTH)
		{
			fOnResizeBorder = (ptMouse.y < (rcWindow.top - rcFrame.top));
			uRow = 0;
		}
		else if (ptMouse.y < rcWindow.bottom && ptMouse.y >= rcWindow.bottom - BOTTOMEXTENDWIDTH)
			uRow = 2;

		// Determine if the point is at the left or right of the window.
		if (ptMouse.x >= rcWindow.left && ptMouse.x < rcWindow.left + LEFTEXTENDWIDTH)
			uCol = 0; // left side
		else if (ptMouse.x < rcWindow.right && ptMouse.x >= rcWindow.right - RIGHTEXTENDWIDTH)
			uCol = 2; // right side

		// Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
		LRESULT hitTests[3][3] =
		{
			{ HTTOPLEFT,    fOnResizeBorder ? HTTOP : HTCAPTION,    HTTOPRIGHT },
			{ HTLEFT,       HTNOWHERE,     HTRIGHT },
			{ HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
		};

		return hitTests[uRow][uCol];
	}

	//LRESULT CALLBACK CustomCaptionDlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam, BOOL& bCallDWP) {
	//	LRESULT lRet = 0;
	//	bCallDWP = TRUE;

	//	switch (Message)
	//	{
	//	case WM_NCCALCSIZE:
	//	{
	//		if (wParam)
	//		{
	//			Core::Classes::UI::CUIBaseWindow RenderWindow(DialogHwnd);



	//			bCallDWP = FALSE;
	//		}
	//	}
	//	break;
	//	case WM_CREATE:
	//	case WM_INITDIALOG:
	//	{
	//		Core::Classes::UI::CUIBaseWindow RenderWindow(DialogHwnd);

	//		



	//		bCallDWP = TRUE;
	//	}
	//	break;
	//	case WM_PAINT:
	//	{
	//		PAINTSTRUCT ps;
	//		HDC hdc = BeginPaint(DialogHwnd, &ps);

	//		Core::Classes::UI::CUIBaseWindow RenderWindow(DialogHwnd);
	//		Core::Classes::UI::CUICanvas Canvas(hdc);
	//		
	//		Canvas.Fill(ps.rcPaint, Core::UI::Theme::ThemeColor_Default);

	//		EndPaint(DialogHwnd, &ps);

	//		bCallDWP = TRUE;

	//		break;
	//	}
	//	break;
	//	default:
	//		bCallDWP = TRUE;
	//		break;
	//	}

	//	return lRet;
	//}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		if (Message == WM_INITDIALOG) {
			RenderViewWindow = DialogHwnd;
			RenderWindowParams.IsCollisionView = FALSE;

			INT_PTR Ret = OldDlgProc(DialogHwnd, Message, wParam, lParam);

			if (g_AttachRenderWnd)
				RNDWND_AttachMainWindow(DialogHwnd);
			//else
			//{
			//	auto size_rect = RenderViewWindow.WindowRect();


			//	SetWindowTheme(DialogHwnd, L"", L"");
			//	// Inform the application of the frame change to force redrawing with the new
			//	// client area that is extended into the title bar
			//	SetWindowPos(DialogHwnd, NULL,
			//		size_rect.Left, size_rect.Top, size_rect.Width, size_rect.Height,
			//		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
			//}

			return Ret;
		}
		//else if ((Message == WM_NCCALCSIZE) && (wParam == TRUE))
		//{
		//	if (RenderViewWindow.Handle != DialogHwnd)
		//		RenderViewWindow = DialogHwnd;

		//	UINT dpi = RenderViewWindow.DpiForWindow();
		//	INT frame_x = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
		//	INT frame_y = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
		//	INT padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

		//	NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;
		//	RECT* requested_client_rect = params->rgrc;

		//	requested_client_rect->right -= frame_x + padding;
		//	requested_client_rect->left += frame_x + padding;
		//	requested_client_rect->bottom -= frame_y + padding;

		//	if (RenderViewWindow.IsWindowMaximized())
		//		requested_client_rect->top += padding;

		//	return 0;
		//}
		//else if (Message == WM_PAINT)
		//{
		//	fff:
		//	PAINTSTRUCT ps;
		//	HDC hdc = GetWindowDC(DialogHwnd);//BeginPaint(DialogHwnd, &ps);

		//	// Paint Background
		//	//COLORREF bg_color = RGB(200, 250, 230);
		//	//HBRUSH bg_brush = CreateSolidBrush(bg_color);
		//	//FillRect(hdc, &ps.rcPaint, bg_brush);
		//	//DeleteObject(bg_brush);

		//	// Paint Title Bar
		//	HTHEME theme = OpenThemeData(DialogHwnd, L"WINDOW");

		//	COLORREF title_bar_color = RGB(150, 200, 180);
		//	HBRUSH title_bar_brush = CreateSolidBrush(title_bar_color);
		//	COLORREF title_bar_hover_color = RGB(130, 180, 160);
		//	HBRUSH title_bar_hover_brush = CreateSolidBrush(title_bar_hover_color);

		//	RECT title_bar_rect = { 0, 0, 2000, 2000 };

		//	// Title Bar Background
		//	FillRect(hdc, &title_bar_rect, title_bar_brush);


		//	DeleteObject(title_bar_hover_brush);
		//	DeleteObject(title_bar_brush);

		//	CloseThemeData(theme);
		//	//EndPaint(DialogHwnd, &ps);
		//}
		//else if (Message == WM_SIZE)
		//{
		//	auto lRet = OldDlgProc(DialogHwnd, Message, wParam, lParam);
		//	InvalidateRect(DialogHwnd, NULL, TRUE);
		//	return lRet;
		//}
		else {
			if (Message == WM_KEYUP) {
				auto ctrl = HIBYTE(GetKeyState(VK_CONTROL)) & 0x80;

				if (!g_HotkeyReplace)
				{
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
					}
				}
				else
				{
					auto alt = HIBYTE(GetKeyState(VK_MENU)) & 0x80;
					auto shift = HIBYTE(GetKeyState(VK_SHIFT)) & 0x80;

					for (auto it = MainWindow::UIKeybinds.begin(); it != MainWindow::UIKeybinds.end(); it++)
					{
						if (!_stricmp("HKFunc_ToggleFog", (*it).HKFuncName.c_str()))
						{
							if ((*it).Shift == (bool)shift && (*it).Ctrl == (bool)ctrl &&
								(*it).Alt == (bool)alt && (*it).Vk == (CHAR)wParam)
							{
								// Fake click fog
								MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(23).Click();
								break;
							}
						}

						if (!_stricmp("HKFunc_ToggleMarkers", (*it).HKFuncName.c_str()))
						{
							if ((*it).Shift == (bool)shift && (*it).Ctrl == (bool)ctrl &&
								(*it).Alt == (bool)alt && (*it).Vk == (CHAR)wParam)
							{
								// If you click on M, the menu will still have the previous state, we will fix this. 
								// However, in fact, there should be two requests to show or hide, but the second one is ignored and this is good.
								MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(15).Click();
								break;
							}
						}
					}
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
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}


// 4607CB place call resize
// 463450 function resize