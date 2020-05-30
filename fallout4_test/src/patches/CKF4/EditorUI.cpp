#include "../../common.h"
#include <CommCtrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include "EditorUI.h"
#include "EditorUIDarkMode.h"
#include "LogWindow.h"
#include "TESForm_CK.h"

#pragma comment(lib, "comctl32.lib")

namespace EditorUI
{
	HWND MainWindowHandle;
	HMENU ExtensionMenuHandle;

	WNDPROC OldWndProc;
	DLGPROC OldObjectWindowProc;
	DLGPROC OldCellViewProc;
	DLGPROC OldResponseWindowProc;

	HWND GetWindow()
	{
		return MainWindowHandle;
	}

	void Initialize()
	{
		InitCommonControls();
		EditorUIDarkMode::InitializeThread();

		if (!LogWindow::Initialize())
			MessageBoxA(nullptr, "Failed to create console log window", "Error", MB_ICONERROR);
	}

	bool CreateExtensionMenu(HWND MainWindow, HMENU MainMenu)
	{
		// Create extended menu options
		ExtensionMenuHandle = CreateMenu();

		BOOL result = TRUE;
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING, UI_EXTMENU_SHOWLOG, "Show Log");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING, UI_EXTMENU_CLEARLOG, "Clear Log");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING | MF_CHECKED, UI_EXTMENU_AUTOSCROLL, "Autoscroll Log");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_SEPARATOR, UI_EXTMENU_SPACER, "");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING, UI_EXTMENU_LOADEDESPINFO, "Dump Active Forms");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_SEPARATOR, UI_EXTMENU_SPACER, "");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING, UI_EXTMENU_HARDCODEDFORMS, "Save Hardcoded Forms");

		MENUITEMINFO menuInfo
		{
			.cbSize = sizeof(MENUITEMINFO),
			.fMask = MIIM_SUBMENU | MIIM_ID | MIIM_STRING,
			.wID = UI_EXTMENU_ID,
			.hSubMenu = ExtensionMenuHandle,
			.dwTypeData = "Extensions",
			.cch = (uint32_t)strlen(menuInfo.dwTypeData)
		};
		result = result && InsertMenuItem(MainMenu, -1, TRUE, &menuInfo);

		// Links
		auto linksMenuHandle = CreateMenu();

		result = result && InsertMenu(linksMenuHandle, -1, MF_BYPOSITION | MF_STRING, UI_EXTMENU_LINKS_WIKI, "Cascadia Wiki");

		menuInfo = {};
		menuInfo.cbSize = sizeof(MENUITEMINFO);
		menuInfo.fMask = MIIM_SUBMENU | MIIM_ID | MIIM_STRING;
		menuInfo.hSubMenu = linksMenuHandle;
		menuInfo.wID = UI_EXTMENU_LINKS_ID;
		menuInfo.dwTypeData = "Links";
		menuInfo.cch = (uint32_t)strlen(menuInfo.dwTypeData);
		result = result && InsertMenuItem(MainMenu, -1, TRUE, &menuInfo);

		AssertMsg(result, "Failed to create extension submenus");
		return result != FALSE;
	}

	LRESULT CALLBACK WndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_CREATE)
		{
			auto createInfo = (const CREATESTRUCT *)lParam;

			if (!_stricmp(createInfo->lpszName, "Creation Kit") && !_stricmp(createInfo->lpszClass, "Creation Kit"))
			{
				// Initialize the original window before adding anything
				LRESULT status = CallWindowProc(OldWndProc, Hwnd, Message, wParam, lParam);
				MainWindowHandle = Hwnd;

				// Create custom menu controls
				CreateExtensionMenu(Hwnd, createInfo->hMenu);
				return status;
			}
		}
		else if (Message == WM_COMMAND)
		{
			const uint32_t param = LOWORD(wParam);

			switch (param)
			{
			case UI_EDITOR_OPENFORMBYID:
			{
				auto form = TESForm_CK::GetFormByNumericID((uint32_t)lParam);

				if (form)
					(*(void(__fastcall **)(TESForm_CK *, HWND, __int64, __int64))(*(__int64 *)form + 0x340))(form, Hwnd, 0, 1);
			}
			return 0;

			case UI_EXTMENU_SHOWLOG:
			{
				ShowWindow(LogWindow::GetWindow(), SW_SHOW);
				SetForegroundWindow(LogWindow::GetWindow());
			}
			return 0;

			case UI_EXTMENU_CLEARLOG:
			{
				PostMessageA(LogWindow::GetWindow(), UI_LOG_CMD_CLEARTEXT, 0, 0);
			}
			return 0;

			case UI_EXTMENU_AUTOSCROLL:
			{
				MENUITEMINFO info
				{
					.cbSize = sizeof(MENUITEMINFO),
					.fMask = MIIM_STATE
				};
				GetMenuItemInfo(ExtensionMenuHandle, param, FALSE, &info);

				bool check = !((info.fState & MFS_CHECKED) == MFS_CHECKED);

				if (!check)
					info.fState &= ~MFS_CHECKED;
				else
					info.fState |= MFS_CHECKED;

				PostMessageA(LogWindow::GetWindow(), UI_LOG_CMD_AUTOSCROLL, (WPARAM)check, 0);
				SetMenuItemInfo(ExtensionMenuHandle, param, FALSE, &info);
			}
			return 0;

			case UI_EXTMENU_LOADEDESPINFO:
			{
				char filePath[MAX_PATH] = {};
				OPENFILENAME ofnData
				{
					.lStructSize = sizeof(OPENFILENAME),
					.lpstrFilter = "Text Files (*.txt)\0*.txt\0\0",
					.lpstrFile = filePath,
					.nMaxFile = ARRAYSIZE(filePath),
					.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
					.lpstrDefExt = "txt"
				};

				if (FILE *f; GetSaveFileName(&ofnData) && fopen_s(&f, filePath, "w") == 0)
				{
					struct VersionControlListItem
					{
						const char *EditorId;
						uint32_t FileOffset;
						char Type[4];
						uint32_t FileLength;
						char GroupType[4];
						uint32_t FormId;
						uint32_t VersionControlId;
						char _pad0[0x8];
					};
					static_assert_offset(VersionControlListItem, EditorId, 0x0);
					static_assert_offset(VersionControlListItem, FileOffset, 0x8);
					static_assert_offset(VersionControlListItem, Type, 0xC);
					static_assert_offset(VersionControlListItem, FileLength, 0x10);
					static_assert_offset(VersionControlListItem, GroupType, 0x14);
					static_assert_offset(VersionControlListItem, FormId, 0x18);
					static_assert_offset(VersionControlListItem, VersionControlId, 0x1C);
					static_assert(sizeof(VersionControlListItem) == 0x28);

					static std::vector<VersionControlListItem> formList;

					// Invoke the dialog, building form list
					void(*callback)(void *, void *, void *, __int64) = [](void *, void *, void *, __int64 Item)
					{
						auto data = *(VersionControlListItem **)(Item + 0x28);

						formList.push_back(*data);
						formList.back().EditorId = _strdup(data->EditorId);
					};

					XUtil::PatchMemoryNop(OFFSET(0x5A5D51, 0), 6);
					XUtil::DetourCall(OFFSET(0x5A5D51, 0), callback);
					CallWindowProcA((WNDPROC)OFFSET(0x5A8250, 0), Hwnd, WM_COMMAND, 1185, 0);

					// Sort by: type, editor id, form id, then file offset
					std::sort(formList.begin(), formList.end(),
						[](const auto& A, const auto& B) -> bool
					{
						int ret = memcmp(A.Type, B.Type, sizeof(VersionControlListItem::Type));

						if (ret != 0)
							return ret < 0;

						ret = _stricmp(A.EditorId, B.EditorId);

						if (ret != 0)
							return ret < 0;

						if (A.FormId != B.FormId)
							return A.FormId > B.FormId;

						return A.FileOffset > B.FileOffset;
					});

					// Dump it to the log
					fprintf(f, "Type, Editor Id, Form Id, File Offset, File Length, Version Control Id\n");

					for (auto& item : formList)
					{
						fprintf(f, "%c%c%c%c,\"%s\",%08X,%u,%u,-%08X-\n",
							item.Type[0], item.Type[1], item.Type[2], item.Type[3],
							item.EditorId,
							item.FormId,
							item.FileOffset,
							item.FileLength,
							item.VersionControlId);

						free((void *)item.EditorId);
					}

					formList.clear();
					fclose(f);
				}
			}
			return 0;

			case UI_EXTMENU_HARDCODEDFORMS:
			{
				for (uint32_t i = 0; i < 2048; i++)
				{
					auto form = TESForm_CK::GetFormByNumericID(i);

					if (form)
					{
						(*(void(__fastcall **)(TESForm_CK *, __int64))(*(__int64 *)form + 0x1A0))(form, 1);
						LogWindow::Log("SetFormModified(%08X)", i);
					}
				}

				// Fake the click on "Save"
				PostMessageA(Hwnd, WM_COMMAND, 40127, 0);
			}
			return 0;

			case UI_EXTMENU_LINKS_WIKI:
			{
				ShellExecute(nullptr, "open", "https://wiki.falloutcascadia.com/index.php?title=Main_Page", "", "", SW_SHOW);
			}
			return 0;
			}
		}
		else if (Message == WM_SETTEXT && Hwnd == GetWindow())
		{
			// Continue normal execution but with a custom string
			char customTitle[1024];
			sprintf_s(customTitle, "%s [CK64Fixes Rev. F4-%s]", (const char *)lParam, g_GitVersion);

			return CallWindowProc(OldWndProc, Hwnd, Message, wParam, (LPARAM)customTitle);
		}

		return CallWindowProc(OldWndProc, Hwnd, Message, wParam, lParam);
	}

	INT_PTR CALLBACK ObjectWindowProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			// Eliminate the flicker when changing categories
			ListView_SetExtendedListViewStyleEx(GetDlgItem(DialogHwnd, 1041), LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
		}
		/*
		else if (Message == WM_COMMAND)
		{
			const uint32_t param = LOWORD(wParam);

			if (param == UI_OBJECT_WINDOW_CHECKBOX)
			{
				bool enableFilter = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED;
				SetPropA(DialogHwnd, "ActiveOnly", (HANDLE)enableFilter);

				// Force the list items to update as if it was by timer
				SendMessageA(DialogHwnd, WM_TIMER, 0x4D, 0);
				return 1;
			}
		}
		else if (Message == UI_OBJECT_WINDOW_ADD_ITEM)
		{
			const bool onlyActiveForms = (bool)GetPropA(DialogHwnd, "ActiveOnly");
			const auto form = (TESForm_CK *)wParam;
			bool *allowInsert = (bool *)lParam;

			*allowInsert = true;

			if (onlyActiveForms)
			{
				if (form && !form->GetActive())
					*allowInsert = false;
			}

			return 1;
		}
		*/
		return OldObjectWindowProc(DialogHwnd, Message, wParam, lParam);
	}
	
	INT_PTR CALLBACK CellViewProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			// Eliminate the flicker when changing cells
			ListView_SetExtendedListViewStyleEx(GetDlgItem(DialogHwnd, 1155), LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
			ListView_SetExtendedListViewStyleEx(GetDlgItem(DialogHwnd, 1156), LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);

			ShowWindow(GetDlgItem(DialogHwnd, 1007), SW_HIDE);
		}
		/*
		else if (Message == WM_SIZE)
		{
			auto *labelRect = (RECT *)OFFSET(0x3AFB570, 1530);

			// Fix the "World Space" label positioning on window resize
			RECT label;
			GetClientRect(GetDlgItem(DialogHwnd, 1164), &label);

			RECT rect;
			GetClientRect(GetDlgItem(DialogHwnd, 2083), &rect);

			int ddMid = rect.left + ((rect.right - rect.left) / 2);
			int labelMid = (label.right - label.left) / 2;

			SetWindowPos(GetDlgItem(DialogHwnd, 1164), nullptr, ddMid - (labelMid / 2), labelRect->top, 0, 0, SWP_NOSIZE);

			// Force the dropdown to extend the full length of the column
			labelRect->right = 0;
		}
		else if (Message == WM_COMMAND)
		{
			const uint32_t param = LOWORD(wParam);

			if (param == UI_CELL_VIEW_CHECKBOX)
			{
				bool enableFilter = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED;
				SetPropA(DialogHwnd, "ActiveOnly", (HANDLE)enableFilter);

				// Fake the dropdown list being activated
				SendMessageA(DialogHwnd, WM_COMMAND, MAKEWPARAM(2083, 1), 0);
				return 1;
			}
		}
		else if (Message == UI_CELL_VIEW_ADD_CELL_ITEM)
		{
			const bool onlyActiveForms = (bool)GetPropA(DialogHwnd, "ActiveOnly");
			const auto form = (TESForm_CK *)wParam;
			bool *allowInsert = (bool *)lParam;

			*allowInsert = true;

			if (onlyActiveForms)
			{
				if (form && !form->GetActive())
					*allowInsert = false;
			}

			return 1;
		}
		*/
		return OldCellViewProc(DialogHwnd, Message, wParam, lParam);
	}

	INT_PTR CALLBACK ResponseWindowProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		bool& enableLipGeneration = *(bool *)OFFSET(0x455E2C8, 0);

		if (Message == WM_INITDIALOG)
		{
			if (enableLipGeneration)
			{
				if (GetFileAttributes("CreationKit32.exe") == INVALID_FILE_ATTRIBUTES ||
					GetFileAttributes("GFSDK_GodraysLib.Win32.dll") == INVALID_FILE_ATTRIBUTES || 
					GetFileAttributes("ssce5532.dll") == INVALID_FILE_ATTRIBUTES)
					enableLipGeneration = false;

				if (!enableLipGeneration)
					LogWindow::LogWarning(7, "'CreationKit32.exe', 'GFSDK_GodraysLib.Win32.dll', or 'ssce5532.dll' is missing from your game directory. LIP generation will be disabled.");
			}
		}
		else if (Message == WM_COMMAND)
		{
			const uint32_t param = LOWORD(wParam);

			switch (param)
			{
			case 1016:// "Generate Lip File"
			case 2379:// "From WAV"
			case 2380:// "From LTF"
				auto item = (__int64)ListViewGetSelectedItem(GetDlgItem(DialogHwnd, 2168));

				if (!enableLipGeneration || !item)
					return 1;

				if (param == 1016)
				{
					char audioFilePath[MAX_PATH];
					strcpy_s(audioFilePath, (const char *)(item + 0xC));

					auto data = *(__int64 *)OFFSET(0x6D7B480, 0);
					auto topic = ((__int64(__fastcall *)(__int64, uint32_t))OFFSET(0x0B99420, 0))(*(__int64 *)(data + 0x28), *(uint8_t *)(*(__int64 *)(data + 0x18) + 0x1A));

					// The sound file must exist on disk, not in archives
					if (GetFileAttributes(audioFilePath) == INVALID_FILE_ATTRIBUTES)
					{
						LogWindow::LogWarning(7, "'%s' was not found on disk. Trying WAV extension fallback.", audioFilePath);

						// Try .wav as a fallback
						*strrchr(audioFilePath, '.') = '\0';
						strcat_s(audioFilePath, ".wav");

						if (GetFileAttributes(audioFilePath) == INVALID_FILE_ATTRIBUTES)
						{
							MessageBoxA(DialogHwnd, audioFilePath, "Unable to find audio file on disk", MB_ICONERROR);
							return 1;
						}
					}

					// Run the 32-bit CK
					auto inputText = ((const char *(__fastcall *)(__int64))OFFSET(0x0B56920, 0))(topic);
					((bool(__fastcall *)(HWND, const char *, const char *))OFFSET(0x0B66BF0, 0))(DialogHwnd, audioFilePath, inputText);

					char lipFileTarget[MAX_PATH];
					strcpy_s(lipFileTarget, audioFilePath);
					*strrchr(lipFileTarget, '.') = '\0';
					strcat_s(lipFileTarget, ".lip");

					if (GetFileAttributes(lipFileTarget) == INVALID_FILE_ATTRIBUTES)
						LogWindow::LogWarning(7, "LIP generation failed", lipFileTarget);
					else
						*(uint32_t *)(item + 0x114) = 1;
				}
				else
				{
					bool enableButton =
						IsDlgButtonChecked(DialogHwnd, 2379) && *(uint32_t *)(item + 0x110) ||
						IsDlgButtonChecked(DialogHwnd, 2380) && *(uint32_t *)(item + 0x118);

					EnableWindow(GetDlgItem(DialogHwnd, 1016), enableButton);
				}

				return 1;
			}
		}
		else if (Message == WM_NOTIFY)
		{
			auto notify = (LPNMHDR)lParam;

			if (notify->code == LVN_ITEMCHANGED && notify->idFrom == 2168)
				return ResponseWindowProc(DialogHwnd, WM_COMMAND, 2379, 0);
		}

		return OldResponseWindowProc(DialogHwnd, Message, wParam, lParam);
	}

	BOOL ListViewCustomSetItemState(HWND ListViewHandle, WPARAM Index, UINT Data, UINT Mask)
	{
		// Microsoft's implementation of this define is broken (ListView_SetItemState)
		LVITEMA item
		{
			.mask = LVIF_STATE,
			.state = Data,
			.stateMask = Mask
		};

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

		LVFINDINFOA findInfo
		{
			.flags = LVFI_PARAM,
			.lParam = (LPARAM)Parameter
		};

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

		LVITEMA item
		{
			.mask = LVIF_PARAM,
			.iItem = index
		};

		ListView_GetItem(ListViewHandle, &item);
		return (void *)item.lParam;
	}

	void ListViewDeselectItem(HWND ListViewHandle, void *Parameter)
	{
		LVFINDINFOA findInfo
		{
			.flags = LVFI_PARAM,
			.lParam = (LPARAM)Parameter
		};

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