#include "DataWindow.h"

#include <windowsx.h>

#define LVIS_SELFLAG LVIS_FOCUSED | LVIS_SELECTED

namespace DataWindow
{
	VOID WINAPI ListView_SetSelectItem(HWND hLV, INT idx)
	{
		ListView_SetItemState(hLV, -1, 0, LVIS_SELFLAG);
		ListView_SetItemState(hLV, idx, LVIS_SELFLAG, LVIS_SELFLAG);
		ListView_EnsureVisible(hLV, idx, FALSE);
	}

	VOID WINAPI ComboBox_ClearItems(HWND hCombo)
	{
		auto nCount = ComboBox_GetCount(hCombo);
		for (auto idx = 0; idx < nCount; idx++)
		{
			ComboBox_DeleteString(hCombo, 0);
		}
	}

	INT WINAPI ListView_FindItemByString(HWND hLV, const std::string str, INT start_idx = 0)
	{
		// The standard search engine is too weak. 
		// Mine allows you to find the first match in the list even if the word is somewhere in the middle. 
		// Standard will only find if there is a match first.

		if (str.length() == 0)
			return -1;

		CHAR szBuf[1024] = { 0 };
		int nRows = ListView_GetItemCount(hLV);

		if (nRows > start_idx)
		{
			for (INT idx = start_idx; idx < nRows; idx++)
			{
				ListView_GetItemText(hLV, idx, 0, szBuf, sizeof(szBuf));

				if (XUtil::Str::findCaseInsensitive(szBuf, str.c_str()) != std::string::npos)
					return idx;
			}
		}

		return -1;
	}

	Core::Classes::UI::CUICustomWindow DataWindow;

	struct DataWindowControls_t
	{
		Core::Classes::UI::CUIBaseControl ListViewPlugins;
		Core::Classes::UI::CUIBaseControl ListViewDependences;
		Core::Classes::UI::CUIBaseControl EditSearch;
	} DataWindowControls;

	DLGPROC OldDlgProc;

	HWND GetWindow(void)
	{
		return DataWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& GetWindowObj(void)
	{
		return DataWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		INT idx, idx_safe;
		HWND hSrchEdit, hListView;
		CHAR szStrBuf[1024] = { 0 };

		if (Message == WM_INITDIALOG)
		{
			DataWindow = DialogHwnd;

			// Set font default
			// This is the default value, but I need an object record to create the missing controls
			DataWindow.Font = Core::Classes::UI::CFont("Microsoft Sans Serif", 8, {}, Core::Classes::UI::fqClearTypeNatural, Core::Classes::UI::fpVariable);

			DataWindowControls.ListViewPlugins = DataWindow.GetControl(UI_LISTVIEW_PLUGINS);
			DataWindowControls.ListViewDependences = DataWindow.GetControl(1057);
			DataWindowControls.EditSearch = DataWindow.GetControl(UI_EDIT_SEARCH_PLUGIN_BY_NAME);

			// Eliminate the flicker when changing cells
			ListView_SetExtendedListViewStyleEx(DataWindowControls.ListViewPlugins.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
			ListView_SetExtendedListViewStyleEx(DataWindowControls.ListViewDependences.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
		}
		else if (Message == WM_COMMAND)
		{
			switch (LOWORD(wParam))
			{
			case UI_EDIT_SEARCH_PLUGIN_BY_NAME:
			{
				if (HIWORD(wParam) == CBN_EDITCHANGE)
				{
					hSrchEdit = DataWindowControls.EditSearch.Handle;
					hListView = DataWindowControls.ListViewPlugins.Handle;

					ComboBox_ClearItems(hSrchEdit);

					if (ComboBox_GetTextLength(hSrchEdit) <= 2)
					{
						ComboBox_ShowDropdown(hSrchEdit, FALSE);
						return 0;
					}

					idx_safe = -1;
					idx = -1;

					while ((idx = ListView_FindItemByString(hListView, DataWindowControls.EditSearch.Caption, idx + 1)) != -1)
					{
						if (idx_safe == -1) idx_safe = idx;
						ListView_GetItemText(DataWindowControls.ListViewPlugins.Handle, idx, 0, szStrBuf, sizeof(szStrBuf));
						ComboBox_AddString(hSrchEdit, szStrBuf);
					}

					if (!ComboBox_GetCount(hSrchEdit))
					{
						ComboBox_ShowDropdown(hSrchEdit, FALSE);
						return 0;
					}

					ListView_SetSelectItem(hListView, (idx_safe >= 0) ? idx_safe : 0);
					ComboBox_ShowDropdown(hSrchEdit, TRUE);
					SendMessageA(hSrchEdit, WM_SETCURSOR, 0, 0);
				}
				else if (HIWORD(wParam) == CBN_SELENDOK)
				{
					hSrchEdit = DataWindowControls.EditSearch.Handle;
					hListView = DataWindowControls.ListViewPlugins.Handle;
		
					ComboBox_GetLBText(hSrchEdit, ComboBox_GetCurSel(hSrchEdit), szStrBuf);

					idx = ListView_FindItemByString(hListView, szStrBuf);
					ListView_SetSelectItem(hListView, (idx >= 0) ? idx : 0);

					SetFocus(hListView);
				}
			}
			return 0;
			}
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}