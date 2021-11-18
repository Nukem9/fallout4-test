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

#include "ObjectWindow.h"
#include "EditorUI.h"
#include "MainWindow.h"

#include "../../StdAfx.h"

#define UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW		(UI_CUSTOM_MESSAGE + 4)

namespace ObjectWindow
{
	OBJWNDS ObjectWindows;
	DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(UINT uId)
	{
		UINT i = 0;

		for (auto it = ObjectWindows.begin(); ObjectWindows.end() != it; it++, i++) 
		{
			if (i == uId)
				return (*it).first;
		}

		return NULL;
	}

	OBJWNDS& FIXAPI GetAllWindowObj(VOID)
	{
		return ObjectWindows;
	}

	BOOL WINAPI hk_MoveWindow(HWND hWindow, INT32 X, INT32 Y, INT32 nWidth, INT32 nHeight, BOOL bRepaint)
	{
		BOOL bResult = MoveWindow(hWindow, X, Y, nWidth, nHeight, bRepaint);

		if (auto iterator = ObjectWindows.find(GetParent(hWindow)); iterator != ObjectWindows.end())
		{
			LPOBJWND lpObjWnd = (*iterator).second;
			if (lpObjWnd) lpObjWnd->ObjectWindow.Perform(WM_COMMAND, UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW, 0);
		}

		return bResult;
	}

	VOID FIXAPI ResizeObjectWndChildControls(LPOBJWND lpObjWnd)
	{
		// The perfectionist in me is dying....

		lpObjWnd->Controls.TreeList.LockUpdate();
		lpObjWnd->Controls.ItemList.LockUpdate();
		lpObjWnd->Controls.EditFilter.LockUpdate();
		lpObjWnd->Controls.ToggleDecompose.LockUpdate();
		lpObjWnd->Controls.BtnObjLayout.LockUpdate();

		LONG w_tree = lpObjWnd->Controls.TreeList.Width;
		LONG w_left = w_tree - lpObjWnd->Controls.BtnObjLayout.Width + 1;
		lpObjWnd->Controls.BtnObjLayout.Left = w_left;
		lpObjWnd->Controls.ToggleDecompose.Left = w_left;
		lpObjWnd->Controls.ActiveOnly.Width = w_tree;

		w_left = w_left - lpObjWnd->Controls.EditFilter.Left - 3;
		lpObjWnd->Controls.EditFilter.Width = w_left;
		lpObjWnd->Controls.ComboLayout.Width = w_left;

		lpObjWnd->Controls.ItemList.Left = w_tree + 5;
		lpObjWnd->Controls.ItemList.Width = lpObjWnd->ObjectWindow.ClientWidth() - (w_tree + 5);

		// fix bad pic
		RECT r = { 0, 0, lpObjWnd->Controls.ItemList.Left, lpObjWnd->Controls.TreeList.Top };
		InvalidateRect(lpObjWnd->ObjectWindow.Handle, &r, TRUE);
		UpdateWindow(lpObjWnd->ObjectWindow.Handle);

		lpObjWnd->Controls.BtnObjLayout.UnlockUpdate();
		lpObjWnd->Controls.ToggleDecompose.UnlockUpdate();
		lpObjWnd->Controls.EditFilter.UnlockUpdate();
		lpObjWnd->Controls.ItemList.UnlockUpdate();
		lpObjWnd->Controls.TreeList.UnlockUpdate();
		lpObjWnd->Controls.BtnObjLayout.Repaint();
		lpObjWnd->Controls.ToggleDecompose.Repaint();
		lpObjWnd->Controls.EditFilter.Repaint();
		lpObjWnd->Controls.ItemList.Repaint();
		lpObjWnd->Controls.TreeList.Repaint();
	}

	VOID FIXAPI SetObjectWindowFilter(LPOBJWND lpObjWnd, const std::string & name, const BOOL SkipText, const BOOL actived)
	{
		if (!SkipText)
			lpObjWnd->Controls.EditFilter.Caption = name;

		lpObjWnd->Controls.ActiveOnly.Checked = actived;
		// Force the list items to update as if it was by timer
		//lpObjWnd->ObjectWindow.Perform(WM_TIMER, 0x1B58, 0);
		// This is a bit slower but works in multi windows
		lpObjWnd->Controls.EditFilter.Caption = lpObjWnd->Controls.EditFilter.Caption;
	}

	int32_t FIXAPI hk_7FF72F57F8F0(const int64_t ObjectListInsertData, TESForm* Form)
	{
		if (ObjectListInsertData && FileHandler->IsLoaded())
		{
			LPOBJWND lpObjWnd = ObjectWindows.at(GetParent((*(HWND*)(ObjectListInsertData + 0x18))));
			if (lpObjWnd)
			{
				BOOL allowInsert = TRUE;

				lpObjWnd->ObjectWindow.Perform(UI_OBJECT_WINDOW_ADD_ITEM, (WPARAM)Form, (LPARAM)&allowInsert);

				if (!allowInsert)
					return 1;
			}
		}

		return ((int32_t(__fastcall*)(int64_t, TESForm*))OFFSET(0x40F8F0, 0))(ObjectListInsertData, Form);
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			LPOBJWND lpObjWnd = new OBJWND;

			lpObjWnd->ObjectWindow = DialogHwnd;
				
			lpObjWnd->Controls.TreeList = lpObjWnd->ObjectWindow.GetControl(2093);
			lpObjWnd->Controls.ItemList = lpObjWnd->ObjectWindow.GetControl(1041);
			lpObjWnd->Controls.ToggleDecompose = lpObjWnd->ObjectWindow.GetControl(6027);
			lpObjWnd->Controls.BtnObjLayout = lpObjWnd->ObjectWindow.GetControl(6025);
			lpObjWnd->Controls.ComboLayout = lpObjWnd->ObjectWindow.GetControl(6024);
			lpObjWnd->Controls.EditFilter = lpObjWnd->ObjectWindow.GetControl(2581);
			lpObjWnd->Controls.Spliter = lpObjWnd->ObjectWindow.GetControl(2157);
			lpObjWnd->Controls.ActiveOnly.CreateWnd(lpObjWnd->ObjectWindow, lpObjWnd->ObjectWindow.GetControl(UI_OBJECT_WINDOW_ADD_ITEM), UI_OBJECT_WINDOW_ADD_ITEM);

			// Eliminate the flicker when resizing
			lpObjWnd->Controls.TreeList.Perform(TVM_SETEXTENDEDSTYLE, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);
			// Eliminate the flicker when changing categories
			ListView_SetExtendedListViewStyleEx(lpObjWnd->Controls.ItemList.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);

			// Erase Icon and SysMenu
			if (!ObjectWindows.size())
				lpObjWnd->ObjectWindow.Style = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME;
			else
				lpObjWnd->ObjectWindow.Style = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;

			lpObjWnd->Controls.BtnObjLayout.Top = lpObjWnd->Controls.ComboLayout.Top - 1;
			lpObjWnd->Controls.BtnObjLayout.Height = lpObjWnd->Controls.ComboLayout.Height + 2;
			lpObjWnd->Controls.ToggleDecompose.Top = lpObjWnd->Controls.EditFilter.Top - 1;
			lpObjWnd->Controls.ToggleDecompose.Height = lpObjWnd->Controls.EditFilter.Height + 2;

			ObjectWindows.emplace(DialogHwnd, lpObjWnd);
		}
		// Don't let us reduce the window too much
		else if (Message == WM_GETMINMAXINFO)
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 350;
			lpMMI->ptMinTrackSize.y = 200;

			return S_OK;
		}
		else if (Message == WM_SIZE)
		{
			if (auto iterator = ObjectWindows.find(DialogHwnd);  iterator != ObjectWindows.end())
			{
				LPOBJWND lpObjWnd = (*iterator).second;
				if (lpObjWnd && !lpObjWnd->StartResize)
				{
					lpObjWnd->StartResize = TRUE;
					ResizeObjectWndChildControls(lpObjWnd);
				}
			}
		}
		else if (Message == UI_OBJECT_WINDOW_ADD_ITEM)
		{
			auto form = reinterpret_cast<const TESForm*>(wParam);
			auto allowInsert = reinterpret_cast<BOOL*>(lParam);
			*allowInsert = TRUE;

			if (auto iterator = ObjectWindows.find(DialogHwnd); iterator != ObjectWindows.end())
			{
				LPOBJWND lpObjWnd = (*iterator).second;
				if (lpObjWnd && lpObjWnd->Controls.ActiveOnly.Checked)
				{
					if (form && !form->Active)
						*allowInsert = FALSE;
				}
			}

			return S_OK;
		}
		else if (Message == WM_COMMAND)
		{
			switch (wParam)
			{
			case UI_OBJECT_WINDOW_ADD_ITEM:
			{
				if (auto iterator = ObjectWindows.find(DialogHwnd); iterator != ObjectWindows.end())
				{
					LPOBJWND lpObjWnd = (*iterator).second;
					if (lpObjWnd) SetObjectWindowFilter(lpObjWnd, "", TRUE, !lpObjWnd->Controls.ActiveOnly.Checked);
				}
			}
			return S_OK;
				
			case UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW:
			{
				if (auto iterator = ObjectWindows.find(DialogHwnd); iterator != ObjectWindows.end())
				{
					LPOBJWND lpObjWnd = (*iterator).second;
					if (lpObjWnd) ResizeObjectWndChildControls(lpObjWnd);
				}
			}
			return S_OK;

			}
		}
		else if (Message == WM_SHOWWINDOW)
		{
			if (auto iterator = ObjectWindows.find(DialogHwnd); iterator != ObjectWindows.end())
			{
				LPOBJWND lpObjWnd = (*iterator).second;
				if (lpObjWnd) ResizeObjectWndChildControls(lpObjWnd);
			}
		}
		else if (Message == WM_DESTROY)
		{
			LPOBJWND lpObjWnd = ObjectWindows.at(DialogHwnd);
			if (lpObjWnd)
			{
				ObjectWindows.erase(DialogHwnd);

				delete lpObjWnd;
				lpObjWnd = NULL;
			}
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}