//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

#include "EditorUI.h"
#include "UIThemeMode.h"
#include "LogWindow.h"
#include "MainWindow.h"
#include "RenderWindow.h"

extern __hotkey_t HK_RefreshT;

#define LVIS_SELFLAG LVIS_FOCUSED | LVIS_SELECTED

namespace EditorUI
{
	WNDPROC OldWndProc;
	DLGPROC OldResponseWindowProc;
	BOOL bReplaceTips = FALSE;

	static UINT64 timeInterval = 0;
	static UINT64 timeStartTick = 0;
	static UINT64 timeCurrentTick = 0;

	LRESULT FIXAPI hk_SetSettingsPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// https://github.com/Nukem9/SkyrimSETest/commit/d1181a346ccfe6a655fa9da6544ee1b808f89d2f
		// Scale the status bar segments to fit the window size

		auto scale = [&](INT32 Width)
		{
			return static_cast<INT32>(Width * (LOWORD(lParam) / 1024.0f));
		};

		std::array<INT32, 4> spacing
		{
			scale(150),
			scale(300),
			scale(600),
			-1,
		};

		return SendMessageA(hWnd, SB_SETPARTS, spacing.size(), (LPARAM)spacing.data());
	}

	VOID FIXAPI hk_SpamFPSToStatusBar(INT64 a1)
	{
		timeCurrentTick = GetTickCount64();
		if ((timeCurrentTick - timeStartTick) >= timeInterval)
		{
			((VOID(__fastcall*)(INT64))OFFSET(0x45EBB0, 0))(a1);
			
			timeStartTick = GetTickCount64();
			timeCurrentTick = timeStartTick;
		}
	}

	VOID FIXAPI Initialize(VOID)
	{
		InitCommonControls();
		bReplaceTips = g_INI->GetBoolean("CreationKit", "ReplacingTipsWithProgressBar", FALSE);
		timeInterval = std::min(std::max(g_INI->GetInteger("CreationKit", "FPSSpamInterval", 1000), (INI_INT)42), (INI_INT)1000);
		timeStartTick = GetTickCount64();
		UITheme::InitializeThread();

		if (!LogWindow::Initialize())
			MessageBoxA(NULL, "Failed to create console log window", "Error", MB_ICONERROR);
	}

	BOOL FIXAPI ListViewCustomSetItemState(HWND ListViewHandle, WPARAM Index, UINT Data, UINT Mask)
	{
		// Microsoft's implementation of this define is broken (ListView_SetItemState)
		LVITEMA item = { 0 };
		item.mask = LVIF_STATE;
		item.state = Data;
		item.stateMask = Mask;

		return (BOOL)SendMessageA(ListViewHandle, LVM_SETITEMSTATE, Index, (LPARAM)&item);
	}

	VOID FIXAPI ListView_SetSelectItem(HWND hLV, INT idx) {
		ListView_SetItemState(hLV, -1, 0, LVIS_SELFLAG);
		ListView_SetItemState(hLV, idx, LVIS_SELFLAG, LVIS_SELFLAG);
		ListView_EnsureVisible(hLV, idx, FALSE);
	}

	INT FIXAPI ListView_GetSelectedItemIndex(HWND hLV) {
		// Get the first focused item
		return ListView_GetNextItem(hLV, -1, LVNI_FOCUSED);
	}

	INT FIXAPI ListView_FindItemByString(HWND hLV, const std::string str, INT start_idx) {
		// The standard search engine is too weak. 
		// Mine allows you to find the first match in the list even if the word is somewhere in the middle. 
		// Standard will only find if there is a match first.

		if (str.length() == 0)
			return -1;

		CHAR szBuf[1024] = { 0 };
		INT nRows = ListView_GetItemCount(hLV);

		if (nRows > start_idx) {
			for (INT idx = start_idx; idx < nRows; idx++) {
				ListView_GetItemText(hLV, idx, 0, szBuf, sizeof(szBuf));

				if (XUtil::Str::findCaseInsensitive(szBuf, str.c_str()) != std::string::npos)
					return idx;
			}
		}

		return -1;
	}

	VOID FIXAPI ListViewSelectItem(HWND ListViewHandle, INT32 ItemIndex, bool KeepOtherSelections)
	{
		if (!KeepOtherSelections)
			ListViewCustomSetItemState(ListViewHandle, -1, 0, LVIS_SELECTED);

		if (ItemIndex != -1)
		{
			ListView_EnsureVisible(ListViewHandle, ItemIndex, FALSE);
			ListViewCustomSetItemState(ListViewHandle, ItemIndex, LVIS_SELECTED, LVIS_SELECTED);
		}
	}

	VOID FIXAPI ListViewFindAndSelectItem(HWND ListViewHandle, LPVOID Parameter, bool KeepOtherSelections)
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

	LPVOID FIXAPI ListViewGetSelectedItem(HWND ListViewHandle)
	{
		if (!ListViewHandle)
			return NULL;

		int index = ListView_GetNextItem(ListViewHandle, -1, LVNI_SELECTED);

		if (index == -1)
			return NULL;

		LVITEMA item = { 0 };
		item.mask = LVIF_PARAM;
		item.iItem = index;

		ListView_GetItem(ListViewHandle, &item);
		return (LPVOID)item.lParam;
	}

	VOID FIXAPI ListViewDeselectItem(HWND ListViewHandle, LPVOID Parameter)
	{
		LVFINDINFOA findInfo = { 0 };
		findInfo.flags = LVFI_PARAM;
		findInfo.lParam = (LPARAM)Parameter;

		int index = ListView_FindItem(ListViewHandle, -1, &findInfo);

		if (index != -1)
			ListViewCustomSetItemState(ListViewHandle, index, 0, LVIS_SELECTED);
	}

	VOID FIXAPI ComboBox_Redraw(HWND hCB, BOOL redraw) {
		COMBOBOXINFO info = { 0 };
		info.cbSize = sizeof(COMBOBOXINFO);

		if (!GetComboBoxInfo(hCB, &info))
			return;

		if (redraw) {
			SendMessageA(info.hwndList, WM_SETREDRAW, TRUE, 0);
			SendMessageA(hCB, CB_SETMINVISIBLE, 30, 0);
			SendMessageA(hCB, WM_SETREDRAW, TRUE, 0);
		}
		else {
			// Prevent repainting until finished
			SendMessageA(hCB, WM_SETREDRAW, FALSE, 0);
			// Possible optimization for older libraries (source: MSDN forums)
			SendMessageA(hCB, CB_SETMINVISIBLE, 1, 0);
			SendMessageA(info.hwndList, WM_SETREDRAW, FALSE, 0);
		}
	}

	VOID FIXAPI TabControlDeleteItem(HWND TabControlHandle, uint32_t TabIndex)
	{
		TCITEMA itemInfo = {};

		if (TabCtrl_GetItem(TabControlHandle, TabIndex, &itemInfo))
			TabCtrl_DeleteItem(TabControlHandle, TabIndex);
	}

	VOID FIXAPI RegisterHotkeyFunction(LPVOID Thisptr, VOID(*Callback)(VOID), BSEntryString** HotkeyFunction, BSEntryString** DisplayText, CHAR VirtualKey,
		bool Alt, bool Ctrl, bool Shift) {

#if 0
		std::string debugInfoKeybind;

		if (Shift)
			debugInfoKeybind = "SHIFT";

		if (Ctrl)
		{
			if(!debugInfoKeybind.empty())
				debugInfoKeybind += "+CTRL";
			else
				debugInfoKeybind = "CTRL";
		}

		if (Alt)
		{
			if (!debugInfoKeybind.empty())
				debugInfoKeybind += "+ALT";
			else
				debugInfoKeybind = "ALT";
		}

		std::string keybind;

		if (VirtualKey >= VK_F1 && VirtualKey <= VK_F12)
		{
			char szBuf[5] = { 0 };
			_itoa_s((VirtualKey - VK_F1) + 1, szBuf, 10);

			keybind = "F";
			keybind += szBuf;
		}
		else if (VirtualKey == VK_ESCAPE)
			keybind = "ESC";
		else if (VirtualKey == VK_SPACE)
			keybind = "SPACE";
		else if (VirtualKey == VK_RETURN)
			keybind = "ENTER";
		else if (VirtualKey == VK_DELETE)
			keybind = "DEL";
		else if (VirtualKey == VK_TAB)
			keybind = "TAB";
		else if (VirtualKey == VK_BACK)
			keybind = "BACK";
		else
		{
			char szBuf[2] = { 0 };
			szBuf[0] = VirtualKey;
			keybind = strupr(szBuf);
		}

		if (!debugInfoKeybind.empty()) 
		{
			debugInfoKeybind += "+";
			debugInfoKeybind += keybind;
		}
		else
			debugInfoKeybind = keybind;

		//_MESSAGE_FMT("HOTKEY: %s -> %s (%s)", (*HotkeyFunction)->Get<CHAR>(TRUE), (*DisplayText)->Get<CHAR>(TRUE), debugInfoKeybind.c_str());
		//formating string for ini file
		_MESSAGE_FMT("; %s", (*DisplayText)->Get<CHAR>(TRUE));
		_MESSAGE_FMT("%s=%s", (*HotkeyFunction)->Get<CHAR>(TRUE), debugInfoKeybind.c_str());
#endif

		// Read the setting, strip spaces/quotes, then split by each '+' modifier
		std::string newKeybind = g_INI->Get("CreationKit_Hotkeys", (*HotkeyFunction)->Get<CHAR>(TRUE), "");
		if (!newKeybind.length()) {
			_MESSAGE_FMT("Can't find this hotkeys ""%s"" in the mod settings. (%X), %d, %d, %d, %s)",
				(*HotkeyFunction)->Get<CHAR>(TRUE), VirtualKey, (int)Alt, (int)Ctrl, (int)Shift, (*DisplayText)->Get<CHAR>(TRUE));
		}

		for (size_t i; (i = newKeybind.find("\"")) != std::string::npos;)
			newKeybind.replace(i, 1, "");

		for (size_t i; (i = newKeybind.find(" ")) != std::string::npos;)
			newKeybind.replace(i, 1, "");

		if (!newKeybind.empty()) {
			std::transform(newKeybind.begin(), newKeybind.end(), newKeybind.begin(), toupper);

			VirtualKey = 0;
			Alt = false;
			Ctrl = false;
			Shift = false;

			LPSTR context = NULL;
			LPCSTR t = strtok_s(newKeybind.data(), "+", &context);

			do {
				if (!strcmp(t, "CTRL"))
					Ctrl = true;
				else if (!strcmp(t, "SHIFT"))
					Shift = true;
				else if (!strcmp(t, "ALT"))
					Alt = true;
				else if (!strcmp(t, "ESC"))
					VirtualKey = VK_ESCAPE;
				else if (!strcmp(t, "SPACE"))
					VirtualKey = VK_SPACE;
				else if (!strcmp(t, "ENTER"))
					VirtualKey = VK_RETURN;
				else if (!strcmp(t, "DEL"))
					VirtualKey = VK_DELETE;
				else if (!strcmp(t, "TAB"))
					VirtualKey = VK_TAB;
				else if (strlen(t) > 1 && t[0] == 'F') {
					// Parse function keys F1 to F12
					INT index = atoi(&t[1]);

					AssertMsgVa(index >= 1 && index <= 12, "Invalid function key index '%s' for hotkey function '%s'", t, *HotkeyFunction);

					VirtualKey = VK_F1 + index - 1;
				}
				else {
					// Parse a regular character
					AssertMsgVa(strlen(t) == 1, "Invalid or unknown key binding '%s' for hotkey function '%s'", t, *HotkeyFunction);

					// This should be translated with VkKeyScan but virtual keys make things difficult...
					VirtualKey = t[0];
				}
			} while (t = strtok_s(NULL, "+", &context));

			LPCSTR lpStrF = (*HotkeyFunction)->Get<CHAR>(TRUE);
			for (auto it = MainWindow::UIKeybinds.begin(); it != MainWindow::UIKeybinds.end(); it++)
			{
				if (!_stricmp(lpStrF, (*it).HKFuncName.c_str()))
				{
					(*it).Vk = VirtualKey;
					(*it).Alt = Alt;
					(*it).Ctrl = Ctrl;
					(*it).Shift = Shift;

					(*it).UpdateMenuShortcut();
					break;
				}
			}
		}

		((decltype(&RegisterHotkeyFunction))OFFSET(0x4575A0, 0))(Thisptr, Callback, HotkeyFunction, DisplayText, VirtualKey, Alt, Ctrl, Shift);
	}

	VOID FIXAPI hkAddRefrToLayer(LPVOID* active_layer) {
		BOOL Handle = TRUE;
		auto picker = TESPICKER2;
		if (picker) {
			auto itemList = picker->Forms;
			if (itemList) {
				auto item = itemList->QBuffer(); 
				for (DWORD i = 0; i < picker->SelectCount; i++, item++) {
					if (*item) {
						if ((*item)->GetPrimitive()) {
							Handle = FALSE;
							break;
						}
					}
				}
			}
		}

		if (Handle)
			fastCall<VOID>(0x3C5960, active_layer);
	}

	VOID FIXAPI EncounterZoneAnalyzeLoot(LPVOID _Class, HWND _Dialog, BYTE _Level)
	{
		if (!_Class)
		{
			Core::Classes::UI::CUIMainWindow::MessageWarningDlg(L"You are making mistakes!\n\nSet the ID and location and click Ok, only after that you can open and click on this button.");
			return;
		}

		fastCall<VOID>(0xD29910, _Class, _Dialog, _Level);
	}
}