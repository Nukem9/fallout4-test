#pragma once

#include "../../common.h"

namespace EditorUI
{
	void Initialize();

	BOOL ListViewCustomSetItemState(HWND ListViewHandle, WPARAM Index, UINT Data, UINT Mask);
	void ListViewSelectItem(HWND ListViewHandle, int ItemIndex, bool KeepOtherSelections);
	void ListViewFindAndSelectItem(HWND ListViewHandle, void *Parameter, bool KeepOtherSelections);
	void *ListViewGetSelectedItem(HWND ListViewHandle);
	void ListViewDeselectItem(HWND ListViewHandle, void *Parameter);
	void TabControlDeleteItem(HWND TabControlHandle, uint32_t TabIndex);

	LRESULT WINAPI hk_SetSettingsPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WINAPI hk_SetTextPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void hk_call_141CF03C9(__int64 a1, bool Enable);
}