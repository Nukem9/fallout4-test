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

#include "DataWindow.h"
#include "EditorUI.h"
#include "UIThemeMode.h"
#include "UIImageList.h"
#include "../../../resource.h"

#include "CellViewWindow.h"
#include "ObjectWindow.h"

#include <windowsx.h>

#define SIZEBUF 1024

#define OWNCOLUMNNAME "Filename"
#define TWOCOLUMNNAME "Status"

#define WINDOW_MIN_WIDTH 950
#define WINDOW_MIN_HEIGHT 597

namespace DataWindow
{
	// need static (so not crash)
	NMITEMACTIVATE nmItemFake = { 0 };
	POINT pCurSize = { 0 };
	BOOL fileVerVis;

	Classes::CUICustomWindow DataWindow;

	struct DataWindowControls_t {
		Classes::CUIBaseControl LabelCreateOnPlugins;
		Classes::CUIBaseControl LabelLastModifiedPlugins;
		Classes::CUIBaseControl LabelCreateOnPluginsDate;
		Classes::CUIBaseControl LabelLastModifiedPluginsDate;
		Classes::CUIBaseControl LabelCreateOnPluginsTime;
		Classes::CUIBaseControl LabelLastModifiedPluginsTime;
		Classes::CUIBaseControl BtnSetAsActive;
		Classes::CUIBaseControl BtnDetails;
		Classes::CUIBaseControl BtnOk;
		Classes::CUIBaseControl BtnCancel;
		Classes::CUIBaseControl ListViewPlugins;
		Classes::CUIBaseControl ListViewPluginsResult;
		Classes::CUIBaseControl ListViewDependences;
		Classes::CUIBaseControl EditAuthor;
		Classes::CUIBaseControl EditDescription;
		Classes::CUIBaseControl EditSearch;
		Classes::CUIBaseControl LabelCreateBy;
		Classes::CUIBaseControl LabelSummary;
		Classes::CUIBaseControl LabelDependences;
		Classes::CUIBaseControl LabelFileVersionPlugins;
		Classes::CUIBaseControl LabelCurrentPlugins;
		Classes::CUIBaseControl LabelFileVersionPluginsValue;
		Classes::CUIBaseControl LabelCurrentPluginsValue;

		Classes::CUIImageList ImageList;
	} DataWindowControls;

	DLGPROC OldDlgProc;

	VOID FIXAPI CKF4Fixes_ListViewResultInitialize(VOID) {
		HWND hWnd = DataWindowControls.ListViewPluginsResult.Handle;

		// add list columns
		LVCOLUMN lvc = { 0 };
		// Initialize the LVCOLUMN structure.
		// The mask specifies that the format, width, text, and
		// subitem members of the structure are valid.
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cchTextMax = 255;
		// width of column in pixels
		lvc.cx = 150;

		lvc.pszText = OWNCOLUMNNAME;
		ListView_InsertColumn(hWnd, 0, &lvc);

		lvc.iSubItem = 1;
		lvc.pszText = TWOCOLUMNNAME;
		ListView_InsertColumn(hWnd, 1, &lvc);

		ListView_SetExtendedListViewStyle(hWnd, LVS_OWNERDATA | LVS_LIST | LVS_SMALLICON | LVS_SORTDESCENDING);
		ListView_SetExtendedListViewStyleEx(hWnd, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
	}

	VOID FIXAPI CKF4Fixes_ToggleListView(const BOOL bShowResultListView) {
		HWND hWnd = DataWindowControls.ListViewPlugins.Handle;
		HWND hWndResult = DataWindowControls.ListViewPluginsResult.Handle;

		DataWindowControls.ListViewPlugins.Visible = !bShowResultListView;
		DataWindowControls.ListViewPluginsResult.Visible = bShowResultListView;

		if (bShowResultListView) {
			// The width could be changed, will repeat for list
			ListView_SetColumnWidth(hWndResult, 0, ListView_GetColumnWidth(hWnd, 0));
			ListView_SetColumnWidth(hWndResult, 1, ListView_GetColumnWidth(hWnd, 1));
		}
	}

	BOOL FIXAPI CKF4Fixes_AppendToListViewResult(const std::string sFileName, const std::string sType, const BOOL bCheck) {
		HWND hWnd = DataWindowControls.ListViewPluginsResult.Handle;
		INT iLastIndex = ListView_GetItemCount(hWnd);

		LVITEMA lvi = { 0 };
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.cchTextMax = sFileName.length() + 1;
		lvi.iItem = iLastIndex;
		lvi.pszText = (LPSTR)(&sFileName[0]);
		lvi.iSubItem = 0;
		lvi.iImage = (INT)bCheck;

		if (ListView_InsertItem(hWnd, &lvi) == -1)
			return FALSE;

		ListView_SetItemText(hWnd, iLastIndex, 1, (LPSTR)(&sType[0]));

		return TRUE;
	}

	VOID FIXAPI CKF4Fixes_UpdateListViewResult(VOID) {
		INT idx, idx_safe;
		LVITEMA lvi = { 0 };
		HWND hSrchEdit, hListView, hListViewResult;
		CHAR szStrs[2][SIZEBUF];

		hSrchEdit = DataWindowControls.EditSearch.Handle;
		hListView = DataWindowControls.ListViewPlugins.Handle;
		hListViewResult = DataWindowControls.ListViewPluginsResult.Handle;

		ListView_DeleteAllItems(hListViewResult);

		if (Edit_GetTextLength(hSrchEdit) < 2) {
			CKF4Fixes_ToggleListView(FALSE);
			return;
		}

		idx_safe = -1;
		idx = -1;

		while ((idx = EditorUI::ListView_FindItemByString(hListView, DataWindowControls.EditSearch.Caption, idx + 1)) != -1) {
			if (idx_safe == -1) idx_safe = idx;
			ListView_GetItemText(hListView, idx, 0, szStrs[0], SIZEBUF);
			ListView_GetItemText(hListView, idx, 1, szStrs[1], SIZEBUF);

			lvi.mask = LVIF_IMAGE;
			lvi.iItem = idx;
			ListView_GetItem(hListView, &lvi);

			CKF4Fixes_AppendToListViewResult(szStrs[0], szStrs[1], lvi.iImage != 0);
		}

		if (!ListView_GetItemCount(hListViewResult)) {
			EditorUI::ListView_SetSelectItem(hListView, 0);
			CKF4Fixes_ToggleListView(FALSE);
			return;
		}

		EditorUI::ListView_SetSelectItem(hListView, (idx_safe >= 0) ? idx_safe : 0);
		CKF4Fixes_ToggleListView(TRUE);
		EditorUI::ListView_SetSelectItem(hListViewResult, 0);

		SendMessageA(hSrchEdit, WM_SETCURSOR, 0, 0);
	}

	BOOL FIXAPI CKF4Fixes_LoadImageFromFileA(HBITMAP &bitmap, const std::string& filename) {
		bitmap = (HBITMAP)LoadImageA(NULL, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		return bitmap != NULL;
	}

	BOOL FIXAPI CKF4Fixes_LoadImageFromResourceA(HBITMAP &bitmap, const std::string& name) {
		bitmap = (HBITMAP)LoadImageA(NULL, name.c_str(), IMAGE_BITMAP, 0, 0, 0);
		return bitmap != NULL;
	}

	BOOL FIXAPI CKF4Fixes_ImageListReplaceA(HIMAGELIST imglst, INT idx, const std::string& filename) {
		HBITMAP bmp;
		if (!CKF4Fixes_LoadImageFromFileA(bmp, filename))
			return FALSE;

		BOOL bRes = ImageList_Replace(imglst, idx, NULL, bmp);
		DeleteObject(bmp);

		return bRes;
	}

	HWND FIXAPI GetWindow(VOID) {
		return DataWindow.Handle;
	}

	Classes::CUICustomWindow& FIXAPI GetWindowObj(VOID) {
		return DataWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG) {
			DataWindow = DialogHwnd;

			auto rClient = DataWindow.ClientRect();
			pCurSize.x = rClient.Width;
			pCurSize.y = rClient.Height;

			DataWindowControls.ListViewPlugins = DataWindow.GetControl(UI_LISTVIEW_PLUGINS);
			DataWindowControls.ListViewPluginsResult = DataWindow.GetControl(UI_NEW_LISTVIEW_CONTROL_TO_RESULT);
			DataWindowControls.ListViewDependences = DataWindow.GetControl(1057);
			DataWindowControls.EditSearch = DataWindow.GetControl(UI_EDIT_SEARCH_PLUGIN_BY_NAME);
			DataWindowControls.EditDescription = DataWindow.GetControl(1024);
			DataWindowControls.EditAuthor = DataWindow.GetControl(1025);
			DataWindowControls.BtnSetAsActive = DataWindow.GetControl(1121);
			DataWindowControls.BtnDetails = DataWindow.GetControl(1185);
			DataWindowControls.BtnOk = DataWindow.GetControl(1);
			DataWindowControls.BtnCancel = DataWindow.GetControl(2);
			DataWindowControls.LabelCreateBy = DataWindow.GetControl(52006);
			DataWindowControls.LabelSummary = DataWindow.GetControl(52007);
			DataWindowControls.LabelDependences = DataWindow.GetControl(52008);
			DataWindowControls.LabelCreateOnPlugins = DataWindow.GetControl(52009);
			DataWindowControls.LabelLastModifiedPlugins = DataWindow.GetControl(52010);
			DataWindowControls.LabelFileVersionPlugins = DataWindow.GetControl(2406);
			DataWindowControls.LabelCurrentPlugins = DataWindow.GetControl(2407);
			DataWindowControls.LabelFileVersionPluginsValue = DataWindow.GetControl(1681);
			DataWindowControls.LabelCurrentPluginsValue = DataWindow.GetControl(1682);
			DataWindowControls.LabelCreateOnPluginsDate = DataWindow.GetControl(1026);
			DataWindowControls.LabelLastModifiedPluginsDate = DataWindow.GetControl(1027);
			DataWindowControls.LabelCreateOnPluginsTime = DataWindow.GetControl(1028);
			DataWindowControls.LabelLastModifiedPluginsTime = DataWindow.GetControl(1029);

			// Eliminate the flicker when changing cells
			ListView_SetExtendedListViewStyleEx(DataWindowControls.ListViewPlugins.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
			ListView_SetExtendedListViewStyleEx(DataWindowControls.ListViewDependences.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);

			// Initialize listview for result search
			CKF4Fixes_ListViewResultInitialize();

			INT_PTR nRes = OldDlgProc(DialogHwnd, Message, wParam, lParam);

			fileVerVis = DataWindowControls.LabelFileVersionPlugins.Visible;

			// Bethesda probably doesn't know about the existence of Check. 
			// They have created icons that mimic pictorially for the user.
			// I completely take everything from there, although I'm not happy about it, but this is a ready-made mechanism, and I'm just trying to make a search in it.
			HIMAGELIST hImageList = ListView_GetImageList(DataWindowControls.ListViewPlugins.Handle, LVSIL_SMALL);
			ImageList_Destroy(hImageList);

			DataWindowControls.ImageList.ReCreate(16, 16, TRUE, Core::Classes::UI::ilct24Bit);

			if (UITheme::IsEnabledMode() && ((UITheme::Theme::GetTheme() == UITheme::Theme::Theme_Dark) ||
				(UITheme::Theme::GetTheme() == UITheme::Theme::Theme_DarkGray))) {
				DataWindowControls.ImageList.AddFromResource(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP4), RGB(32, 32, 32));
				DataWindowControls.ImageList.AddFromResource(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP2), RGB(32, 32, 32));
			}
			else {
				DataWindowControls.ImageList.AddFromResource(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP3), RGB(255, 255, 255));
				DataWindowControls.ImageList.AddFromResource(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP1), RGB(255, 255, 255));
			}

			ListView_SetImageList(DataWindowControls.ListViewPluginsResult.Handle, DataWindowControls.ImageList.Handle, LVSIL_SMALL);
			ListView_SetImageList(DataWindowControls.ListViewPlugins.Handle, DataWindowControls.ImageList.Handle, LVSIL_SMALL);

			// fix no checked in list 
			RedrawWindow(DataWindowControls.ListViewPlugins.Handle, NULL, NULL, RDW_UPDATENOW | RDW_NOCHILDREN);

			MoveWindow(DialogHwnd,
				g_INI_CK->GetInteger("General", "Data X", 100),
				g_INI_CK->GetInteger("General", "Data Y", 100),
				g_INI_CK->GetInteger("General", "Data W", WINDOW_MIN_WIDTH),
				g_INI_CK->GetInteger("General", "Data H", WINDOW_MIN_HEIGHT),
				TRUE);

			ListView_SetColumnWidth(
				DataWindowControls.ListViewPlugins.Handle,
				0,
				g_INI_CK->GetInteger("General", "Data Column 0 width", 300));

			return nRes;
		}

		// Don't let us reduce the window too much
		else if (Message == WM_GETMINMAXINFO)
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = WINDOW_MIN_WIDTH;
			lpMMI->ptMinTrackSize.y = WINDOW_MIN_HEIGHT;

			return S_OK;
		}
		else if (Message == WM_MOVE) {
			auto rWnd = DataWindow.WindowRect();

			g_INI_CK->SetInteger("General", "Data X", rWnd.Left);
			g_INI_CK->SetInteger("General", "Data Y", rWnd.Top);
		}
		else if (Message == WM_SIZE) {
			POINT p_offset = {
				LOWORD(lParam) - pCurSize.x,
				HIWORD(lParam) - pCurSize.y
			};

			pCurSize = POINT{ LOWORD(lParam), HIWORD(lParam) };

			DataWindowControls.BtnSetAsActive.LockUpdate();
			DataWindowControls.BtnDetails.LockUpdate();
			DataWindowControls.BtnOk.LockUpdate();
			DataWindowControls.BtnCancel.LockUpdate();
			DataWindowControls.LabelCreateBy.LockUpdate();
			DataWindowControls.LabelSummary.LockUpdate();
			DataWindowControls.LabelDependences.LockUpdate();
			DataWindowControls.EditAuthor.LockUpdate();
			DataWindowControls.ListViewDependences.LockUpdate();
			DataWindowControls.EditDescription.LockUpdate();
			DataWindowControls.LabelCreateOnPluginsDate.LockUpdate();
			DataWindowControls.LabelLastModifiedPluginsDate.LockUpdate();
			DataWindowControls.LabelCreateOnPluginsTime.LockUpdate();
			DataWindowControls.LabelLastModifiedPluginsTime.LockUpdate();
			DataWindowControls.LabelCreateOnPlugins.LockUpdate();
			DataWindowControls.LabelLastModifiedPlugins.LockUpdate();
			DataWindowControls.ListViewPlugins.LockUpdate();
			DataWindowControls.ListViewPluginsResult.LockUpdate();
			DataWindowControls.EditSearch.LockUpdate();

			if (fileVerVis) {
				DataWindowControls.LabelFileVersionPlugins.LockUpdate();
				DataWindowControls.LabelCurrentPlugins.LockUpdate();
				DataWindowControls.LabelFileVersionPluginsValue.LockUpdate();
				DataWindowControls.LabelCurrentPluginsValue.LockUpdate();
			}

			DataWindowControls.BtnSetAsActive.Top += p_offset.y;
			DataWindowControls.BtnDetails.Top += p_offset.y;
			DataWindowControls.BtnDetails.Left += p_offset.x;
			DataWindowControls.BtnOk.Top += p_offset.y;
			DataWindowControls.BtnCancel.Top += p_offset.y;
			DataWindowControls.BtnOk.Left += p_offset.x;
			DataWindowControls.BtnCancel.Left += p_offset.x;
			DataWindowControls.LabelCreateBy.Left += p_offset.x;
			DataWindowControls.LabelSummary.Left += p_offset.x;
			DataWindowControls.LabelDependences.Left += p_offset.x;
			DataWindowControls.EditAuthor.Left += p_offset.x;
			DataWindowControls.ListViewDependences.Left += p_offset.x;
			DataWindowControls.EditDescription.Left += p_offset.x;
			DataWindowControls.ListViewDependences.Height += p_offset.y;
			DataWindowControls.EditDescription.Height += p_offset.y;
			DataWindowControls.LabelCreateOnPluginsDate.Left += p_offset.x;
			DataWindowControls.LabelCreateOnPluginsDate.Top += p_offset.y;
			DataWindowControls.LabelLastModifiedPluginsDate.Left += p_offset.x;
			DataWindowControls.LabelLastModifiedPluginsDate.Top += p_offset.y;
			DataWindowControls.LabelCreateOnPluginsTime.Left += p_offset.x;
			DataWindowControls.LabelCreateOnPluginsTime.Top += p_offset.y;
			DataWindowControls.LabelLastModifiedPluginsTime.Left += p_offset.x;
			DataWindowControls.LabelLastModifiedPluginsTime.Top += p_offset.y;
			DataWindowControls.LabelCreateOnPlugins.Left += p_offset.x;
			DataWindowControls.LabelCreateOnPlugins.Top += p_offset.y;
			DataWindowControls.LabelLastModifiedPlugins.Left += p_offset.x;
			DataWindowControls.LabelLastModifiedPlugins.Top += p_offset.y;
			DataWindowControls.ListViewPlugins.Width += p_offset.x;
			DataWindowControls.ListViewPlugins.Height += p_offset.y;
			DataWindowControls.ListViewPluginsResult.Width += p_offset.x;
			DataWindowControls.ListViewPluginsResult.Height += p_offset.y;
			DataWindowControls.EditSearch.Width += p_offset.x;

			if (fileVerVis) {
				DataWindowControls.LabelFileVersionPlugins.Left += p_offset.x;
				DataWindowControls.LabelFileVersionPlugins.Top += p_offset.y;
				DataWindowControls.LabelCurrentPlugins.Left += p_offset.x;
				DataWindowControls.LabelCurrentPlugins.Top += p_offset.y;
				DataWindowControls.LabelFileVersionPluginsValue.Left += p_offset.x;
				DataWindowControls.LabelFileVersionPluginsValue.Top += p_offset.y;
				DataWindowControls.LabelCurrentPluginsValue.Left += p_offset.x;
				DataWindowControls.LabelCurrentPluginsValue.Top += p_offset.y;
			}
			
			DataWindowControls.BtnSetAsActive.UnlockUpdate();
			DataWindowControls.BtnDetails.UnlockUpdate();
			DataWindowControls.BtnOk.UnlockUpdate();
			DataWindowControls.BtnCancel.UnlockUpdate();
			DataWindowControls.LabelCreateBy.UnlockUpdate();
			DataWindowControls.LabelSummary.UnlockUpdate();
			DataWindowControls.LabelDependences.UnlockUpdate();
			DataWindowControls.EditAuthor.UnlockUpdate();
			DataWindowControls.ListViewDependences.UnlockUpdate();
			DataWindowControls.EditDescription.UnlockUpdate();
			DataWindowControls.LabelCreateOnPluginsDate.UnlockUpdate();
			DataWindowControls.LabelLastModifiedPluginsDate.UnlockUpdate();
			DataWindowControls.LabelCreateOnPluginsTime.UnlockUpdate();
			DataWindowControls.LabelLastModifiedPluginsTime.UnlockUpdate();
			DataWindowControls.LabelCreateOnPlugins.UnlockUpdate();
			DataWindowControls.LabelLastModifiedPlugins.UnlockUpdate();
			DataWindowControls.ListViewPlugins.UnlockUpdate();
			DataWindowControls.ListViewPluginsResult.UnlockUpdate();
			DataWindowControls.EditSearch.UnlockUpdate();

			if (fileVerVis) {
				DataWindowControls.LabelFileVersionPlugins.UnlockUpdate();
				DataWindowControls.LabelCurrentPlugins.UnlockUpdate();
				DataWindowControls.LabelFileVersionPluginsValue.UnlockUpdate();
				DataWindowControls.LabelCurrentPluginsValue.UnlockUpdate();
			}
			
			DataWindowControls.BtnSetAsActive.Repaint();
			DataWindowControls.BtnDetails.Repaint();
			DataWindowControls.BtnOk.Repaint();
			DataWindowControls.BtnCancel.Repaint();
			DataWindowControls.LabelCreateBy.Repaint();
			DataWindowControls.LabelSummary.Repaint();
			DataWindowControls.LabelDependences.Repaint();
			DataWindowControls.EditAuthor.Repaint();
			DataWindowControls.ListViewDependences.Repaint();
			DataWindowControls.EditDescription.Repaint();
			DataWindowControls.LabelCreateOnPluginsDate.Repaint();
			DataWindowControls.LabelLastModifiedPluginsDate.Repaint();
			DataWindowControls.LabelCreateOnPluginsTime.Repaint();
			DataWindowControls.LabelLastModifiedPluginsTime.Repaint();
			DataWindowControls.LabelCreateOnPlugins.Repaint();
			DataWindowControls.LabelLastModifiedPlugins.Repaint();
			DataWindowControls.ListViewPlugins.Repaint();
			DataWindowControls.ListViewPluginsResult.Repaint();
			DataWindowControls.EditSearch.Repaint();

			if (fileVerVis) {
				DataWindowControls.LabelFileVersionPlugins.Repaint();
				DataWindowControls.LabelCurrentPlugins.Repaint();
				DataWindowControls.LabelFileVersionPluginsValue.Repaint();
				DataWindowControls.LabelCurrentPluginsValue.Repaint();
			}
			
			InvalidateRect(DialogHwnd, NULL, TRUE);
			UpdateWindow(DialogHwnd);

			auto rWnd = DataWindow.WindowRect();

			g_INI_CK->SetInteger("General", "Data X", rWnd.Left);
			g_INI_CK->SetInteger("General", "Data Y", rWnd.Top);
			g_INI_CK->SetInteger("General", "Data W", rWnd.Width);
			g_INI_CK->SetInteger("General", "Data H", rWnd.Height);	

			return S_OK;
		}
		else if (Message == WM_DESTROY) {
			g_INI_CK->SetInteger("General", 
				"Data Column 0 width", 
					ListView_GetColumnWidth(
					DataWindowControls.ListViewPlugins.Handle,
					0));
		}
		else if (Message == WM_COMMAND) {
			switch (LOWORD(wParam)) {
			case UI_EDIT_SEARCH_PLUGIN_BY_NAME: {
				if (HIWORD(wParam) == EN_CHANGE)
					CKF4Fixes_UpdateListViewResult();
			}
			return 0;
			case 1: {
				if (CellViewWindow::CellViewWindowControls.Initialize) {
					CellViewWindow::CellViewWindowControls.EditFilterCell.Caption = "";
					CellViewWindow::CellViewWindowControls.ActiveOnly.Checked = FALSE;
					CellViewWindow::CellViewWindowControls.ActiveOnlyObjs.Checked = FALSE;
					
					for (auto i : ObjectWindow::ObjectWindows)
						i.second->Controls.ActiveOnly.Checked = FALSE;
				}
			}
			case UI_SETACTIVEPLUGIN_BUTTON:
			{
				if (!DataWindowControls.ListViewPluginsResult.Visible)
					return OldDlgProc(DialogHwnd, Message, wParam, lParam);
				else {
					INT_PTR nRes = 0;
					CHAR szStrs[SIZEBUF] = { 0 };
					HWND hListView = DataWindowControls.ListViewPlugins.Handle;
					HWND hListViewResult = DataWindowControls.ListViewPluginsResult.Handle;

					INT idx = EditorUI::ListView_GetSelectedItemIndex(hListViewResult);
					if (idx != -1) {
						ListView_GetItemText(hListViewResult, idx, 0, szStrs, SIZEBUF);
						EditorUI::ListView_SetSelectItem(hListView, EditorUI::ListView_FindItemByString(hListView, szStrs));

						nRes = OldDlgProc(DialogHwnd, Message, wParam, lParam);
						CKF4Fixes_UpdateListViewResult();

						EditorUI::ListView_SetSelectItem(hListViewResult, idx);
					}

					return nRes;
				}
			}
			return OldDlgProc(DialogHwnd, Message, wParam, lParam);
			}
		}
		else if (Message == WM_NOTIFY) {
			if (LOWORD(wParam) == UI_NEW_LISTVIEW_CONTROL_TO_RESULT) {
				switch (((LPNMHDR)lParam)->code) {
				// Select
				case NM_CLICK: {
					LPNMITEMACTIVATE lpnmItem = (LPNMITEMACTIVATE)lParam;
					HWND hWnd = DataWindowControls.ListViewPluginsResult.Handle;
					HWND hWndOld = DataWindowControls.ListViewPlugins.Handle;
					CHAR szStrs[SIZEBUF] = { 0 };

					if (lpnmItem->iItem != -1) {
						ListView_GetItemText(hWnd, lpnmItem->iItem, 0, szStrs, SIZEBUF);
						EditorUI::ListView_SetSelectItem(hWndOld, EditorUI::ListView_FindItemByString(hWndOld, szStrs));
					}
				}
				break;
				// Double-click the mouse, just check the box, do the same
				case NM_DBLCLK:
				// By double - clicking the right mouse button, 
				// I get the active mod installed in the original list of mods.
				// I model this behavior with a short list.
				case NM_RDBLCLK: {
					LPNMITEMACTIVATE lpnmItem = (LPNMITEMACTIVATE)lParam;
					HWND hWnd = DataWindowControls.ListViewPluginsResult.Handle;
					HWND hWndOld = DataWindowControls.ListViewPlugins.Handle;
					RECT rRectItem;
					CHAR szStrs[SIZEBUF] = { 0 };
					auto code = ((LPNMHDR)lParam)->code;

					// fix bug (can click in an empty space)
					if (lpnmItem->iItem != -1) {
						ListView_GetItemText(hWnd, lpnmItem->iItem, 0, szStrs, SIZEBUF);
						INT idx = EditorUI::ListView_FindItemByString(hWndOld, szStrs);

						Assert(idx != -1);
						Assert(ListView_GetItemRect(hWndOld, idx, &rRectItem, LVIR_BOUNDS));

						memset(&nmItemFake, 0, sizeof(nmItemFake));
						nmItemFake.hdr.code = code;
						nmItemFake.hdr.hwndFrom = hWndOld;
						nmItemFake.hdr.idFrom = UI_LISTVIEW_PLUGINS;
						nmItemFake.iItem = idx;
						nmItemFake.ptAction.x = rRectItem.left + 5;
						nmItemFake.ptAction.y = rRectItem.top + 5;

						// fake click
						SendMessageA(DialogHwnd, WM_NOTIFY, UI_LISTVIEW_PLUGINS, (LPARAM)&nmItemFake);

						// update short list
						CKF4Fixes_UpdateListViewResult();

						EditorUI::ListView_SetSelectItem(hWnd, lpnmItem->iItem);
						EditorUI::ListView_SetSelectItem(hWndOld, idx);
					}
				}
				break;
				}

				return 0;
			}
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}