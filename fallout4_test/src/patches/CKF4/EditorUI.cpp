#include "../../common.h"
#include <CommCtrl.h>
#include <commdlg.h>
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
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING, (UINT_PTR)UI_EXTMENU_SHOWLOG, "Show Log");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING, (UINT_PTR)UI_EXTMENU_CLEARLOG, "Clear Log");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING | MF_CHECKED, (UINT_PTR)UI_EXTMENU_AUTOSCROLL, "Autoscroll Log");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_SEPARATOR, (UINT_PTR)UI_EXTMENU_SPACER, "");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING, (UINT_PTR)UI_EXTMENU_LOADEDESPINFO, "Dump Active Forms");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_SEPARATOR, (UINT_PTR)UI_EXTMENU_SPACER, "");
		result = result && InsertMenu(ExtensionMenuHandle, -1, MF_BYPOSITION | MF_STRING, (UINT_PTR)UI_EXTMENU_HARDCODEDFORMS, "Save Hardcoded Forms");

		MENUITEMINFO menuInfo;
		memset(&menuInfo, 0, sizeof(MENUITEMINFO));
		menuInfo.cbSize = sizeof(MENUITEMINFO);
		menuInfo.fMask = MIIM_SUBMENU | MIIM_ID | MIIM_STRING;
		menuInfo.hSubMenu = ExtensionMenuHandle;
		menuInfo.wID = UI_EXTMENU_ID;
		menuInfo.dwTypeData = "Extensions";
		menuInfo.cch = (uint32_t)strlen(menuInfo.dwTypeData);
		result = result && InsertMenuItem(MainMenu, -1, TRUE, &menuInfo);

		AssertMsg(result, "Failed to create extension submenu");
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
				MENUITEMINFO info;
				info.cbSize = sizeof(MENUITEMINFO);
				info.fMask = MIIM_STATE;
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
				char filePath[MAX_PATH];
				memset(filePath, 0, sizeof(filePath));

				OPENFILENAME ofnData;
				memset(&ofnData, 0, sizeof(OPENFILENAME));
				ofnData.lStructSize = sizeof(OPENFILENAME);
				ofnData.lpstrFilter = "Text Files (*.txt)\0*.txt\0\0";
				ofnData.lpstrFile = filePath;
				ofnData.nMaxFile = ARRAYSIZE(filePath);
				ofnData.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
				ofnData.lpstrDefExt = "txt";

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

					// Sort by: form id, then name, then file offset
					std::sort(formList.begin(), formList.end(),
						[](const VersionControlListItem& A, const VersionControlListItem& B) -> bool
					{
						if (A.FormId == B.FormId)
						{
							if (int ret = _stricmp(A.EditorId, B.EditorId); ret != 0)
								return ret < 0;

							return A.FileOffset > B.FileOffset;
						}

						return A.FormId > B.FormId;
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

	BOOL ListViewCustomSetItemState(HWND ListViewHandle, WPARAM Index, UINT Data, UINT Mask)
	{
		// Microsoft's implementation of this define is broken (ListView_SetItemState)
		LVITEMA lvi = {};
		lvi.mask = LVIF_STATE;
		lvi.state = Data;
		lvi.stateMask = Mask;

		return (BOOL)SendMessageA(ListViewHandle, LVM_SETITEMSTATE, Index, (LPARAM)&lvi);
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

		LVFINDINFOA findInfo = {};
		findInfo.flags = LVFI_PARAM;
		findInfo.lParam = (LPARAM)Parameter;

		int index = ListView_FindItem(ListViewHandle, -1, &findInfo);

		if (index != -1)
			ListViewSelectItem(ListViewHandle, index, KeepOtherSelections);
	}

	void ListViewDeselectItem(HWND ListViewHandle, void *Parameter)
	{
		LVFINDINFOA findInfo = {};
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