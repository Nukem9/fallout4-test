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

#pragma once

#include "..\..\[Common]\StdAfx.h"

namespace EditorUI
{
	extern BOOL bReplaceTips;

	VOID FIXAPI Initialize(VOID);

	BOOL FIXAPI ListViewCustomSetItemState(HWND ListViewHandle, WPARAM Index, UINT Data, UINT Mask);
	VOID FIXAPI ListViewSelectItem(HWND ListViewHandle, INT32 ItemIndex, bool KeepOtherSelections);
	VOID FIXAPI ListViewFindAndSelectItem(HWND ListViewHandle, LPVOID Parameter, bool KeepOtherSelections);
	LPVOID FIXAPI ListViewGetSelectedItem(HWND ListViewHandle);
	VOID FIXAPI ListViewDeselectItem(HWND ListViewHandle, LPVOID Parameter);
	VOID FIXAPI TabControlDeleteItem(HWND TabControlHandle, uint32_t TabIndex);
	VOID FIXAPI ListView_SetSelectItem(HWND hLV, INT idx);
	INT FIXAPI ListView_GetSelectedItemIndex(HWND hLV);
	INT FIXAPI ListView_FindItemByString(HWND hLV, const std::string str, INT start_idx = 0);
	VOID FIXAPI ComboBox_Redraw(HWND hCB, BOOL redraw);
	LRESULT FIXAPI hk_SetSettingsPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	VOID FIXAPI hk_SpamFPSToStatusBar(INT64 a1);
	VOID FIXAPI RegisterHotkeyFunction(LPVOID Thisptr, VOID(*Callback)(VOID), LPCSTR* HotkeyFunction, LPCSTR* DisplayText, CHAR VirtualKey,
		BOOL Alt, BOOL Ctrl, BOOL Shift);
	VOID FIXAPI hkAddRefrToLayer(LPVOID* active_layer);
}