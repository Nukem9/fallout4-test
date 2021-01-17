#include "DataWindow.h"

#define LVIS_SELFLAG LVIS_FOCUSED | LVIS_SELECTED
#define USER_CUSTOM_FIND

namespace DataWindow
{
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

	void SetSelectPluginItem(int idx)
	{
		ListView_SetItemState(DataWindowControls.ListViewPlugins.Handle, -1, 0, LVIS_SELFLAG);
		ListView_SetItemState(DataWindowControls.ListViewPlugins.Handle, idx, LVIS_SELFLAG, LVIS_SELFLAG);
		ListView_EnsureVisible(DataWindowControls.ListViewPlugins.Handle, idx, FALSE);
	}

	int FindItem(const std::string findstr)
	{
#ifndef USER_CUSTOM_FIND
		LVFINDINFOA FindInfo = { 0 };
		FindInfo.flags = LVFI_PARTIAL | LVFI_STRING;
		FindInfo.vkDirection = VK_DOWN;
		FindInfo.psz = findstr.c_str();

		return ListView_FindItem(DataWindowControls.ListViewPlugins.Handle, -1, &FindInfo);
#else
		// The standard search engine is too weak. 
		// Mine allows you to find the first match in the list even if the word is somewhere in the middle. 
		// Standard will only find if there is a match first.

		if (findstr.length() == 0)
			return -1;

		char szBuf[1024] = {0};
		HWND listHandle = DataWindowControls.ListViewPlugins.Handle;
		int nRows = ListView_GetItemCount(listHandle);
		
		if (nRows > 0)
		{
			for (int idx = 0; idx < nRows; idx++)
			{
				ListView_GetItemText(listHandle, idx, 0, szBuf, sizeof(szBuf));
		
				if(XUtil::Str::findCaseInsensitive(szBuf, findstr.c_str()) != std::string::npos)
					return idx;
			}
		}

		return -1;
#endif // !USER_CUSTOM_FIND
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		int idx = 0;

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
				if (HIWORD(wParam) == EN_CHANGE)
				{
					idx = FindItem(DataWindowControls.EditSearch.Caption);
					SetSelectPluginItem((idx >= 0) ? idx : 0);
				}
			}
			return 0;
			}
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}