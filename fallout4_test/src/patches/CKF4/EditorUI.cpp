#include "../../common.h"
#include <CommCtrl.h>
#include "EditorUI.h"
#include "UIThemeMode.h"
#include "LogWindow.h"
#include "MainWindow.h"

namespace EditorUI
{
	WNDPROC OldWndProc;
	DLGPROC OldResponseWindowProc;

	LRESULT WINAPI hk_SetSettingsPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		int parts[4] = { 200, 300, 700, 5000 /* over 5000 for if -1, the text is erased */ };
		return SendMessageA(hWnd, SB_SETPARTS, sizeof(parts), (LPARAM)parts);
	}

	LRESULT WINAPI hk_SetTextPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		BYTE bId = LOBYTE(wParam);
		if (bId == 1) bId = 2;
		return SendMessageA(hWnd, SB_SETTEXTA, MAKEWORD(bId, 0), lParam);
	}

	void Initialize()
	{
		InitCommonControls();
		UITheme::InitializeThread();

		if (!LogWindow::Initialize())
			MessageBoxA(nullptr, "Failed to create console log window", "Error", MB_ICONERROR);
	}

	BOOL ListViewCustomSetItemState(HWND ListViewHandle, WPARAM Index, UINT Data, UINT Mask)
	{
		// Microsoft's implementation of this define is broken (ListView_SetItemState)
		LVITEMA item = { 0 };
		item.mask = LVIF_STATE;
		item.state = Data;
		item.stateMask = Mask;

		return (BOOL)SendMessageA(ListViewHandle, LVM_SETITEMSTATE, Index, (LPARAM)&item);
	}

	void ListViewSelectItem(HWND ListViewHandle, int ItemIndex, bool KeepOtherSelections)
	{
		if (!KeepOtherSelections)
			ListViewCustomSetItemState(ListViewHandle, -1, 0, LVIS_SELECTED);

		if (ItemIndex != -1)
		{
			ListView_EnsureVisible(ListViewHandle, ItemIndex, FALSE);
			ListViewCustomSetItemState(ListViewHandle, ItemIndex, LVIS_SELECTED, LVIS_SELECTED);
		}
	}

	void ListViewFindAndSelectItem(HWND ListViewHandle, void *Parameter, bool KeepOtherSelections)
	{
		if (!KeepOtherSelections)
			ListViewCustomSetItemState(ListViewHandle, -1, 0, LVIS_SELECTED);

		LVFINDINFOA findInfo = { 0 };
		findInfo.flags = LVFI_PARAM;
		findInfo.lParam = (LPARAM)Parameter;

		int index = ListView_FindItem(ListViewHandle, -1, &findInfo);

		if (index != -1)
			ListViewSelectItem(ListViewHandle, index, KeepOtherSelections);
	}

	void *ListViewGetSelectedItem(HWND ListViewHandle)
	{
		if (!ListViewHandle)
			return nullptr;

		int index = ListView_GetNextItem(ListViewHandle, -1, LVNI_SELECTED);

		if (index == -1)
			return nullptr;

		LVITEMA item = { 0 };
		item.mask = LVIF_PARAM;
		item.iItem = index;

		ListView_GetItem(ListViewHandle, &item);
		return (void *)item.lParam;
	}

	void ListViewDeselectItem(HWND ListViewHandle, void *Parameter)
	{
		LVFINDINFOA findInfo = { 0 };
		findInfo.flags = LVFI_PARAM;
		findInfo.lParam = (LPARAM)Parameter;

		int index = ListView_FindItem(ListViewHandle, -1, &findInfo);

		if (index != -1)
			ListViewCustomSetItemState(ListViewHandle, index, 0, LVIS_SELECTED);
	}

	void TabControlDeleteItem(HWND TabControlHandle, uint32_t TabIndex)
	{
		TCITEMA itemInfo = {};

		if (TabCtrl_GetItem(TabControlHandle, TabIndex, &itemInfo))
			TabCtrl_DeleteItem(TabControlHandle, TabIndex);
	}
}