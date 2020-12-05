#include "../../common.h"
#include <CommCtrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include "EditorUI.h"
#include "EditorUIDarkMode.h"
#include "LogWindow.h"

#pragma comment(lib, "comctl32.lib")



namespace EditorUI
{
	// Since very little is described, it is easier for me to implement some of the developments

	Core::Classes::UI::CUIMainWindow MainWindow;
	Core::Classes::UI::CUICustomWindow ObjectWindow;
	Core::Classes::UI::CUICustomWindow CellViewWindow;

	struct ObjectWindowControls_t
	{
		BOOL StartResize = FALSE;

		Core::Classes::UI::CUIBaseControl TreeList;
		Core::Classes::UI::CUIBaseControl ItemList;
		Core::Classes::UI::CUIBaseControl ToggleDecompose;
		Core::Classes::UI::CUIBaseControl BtnObjLayout;
		Core::Classes::UI::CUIBaseControl ComboLayout;
		Core::Classes::UI::CUIBaseControl EditFilter;
		Core::Classes::UI::CUIBaseControl Spliter;
		Core::Classes::UI::CUICheckbox ActiveOnly;
	} ObjectWindowControls;

	struct CellViewWindowControls_t
	{
		BOOL Initialize = FALSE;

		Core::Classes::UI::CUIBaseControl LabelWorldSpace;
		Core::Classes::UI::CUIBaseControl NoCellSellected;
		Core::Classes::UI::CUIBaseControl Interiors;
		Core::Classes::UI::CUIBaseControl LoadedAtTop;
		Core::Classes::UI::CUIBaseControl FilteredOnly;
		Core::Classes::UI::CUIBaseControl VisibleOnly;
		Core::Classes::UI::CUIBaseControl SelectedOnly;
		Core::Classes::UI::CUIBaseControl LabelX;
		Core::Classes::UI::CUIBaseControl LabelY;
		Core::Classes::UI::CUIBaseControl EditX;
		Core::Classes::UI::CUIBaseControl EditY;
		Core::Classes::UI::CUIBaseControl EditCellFiltered;
		Core::Classes::UI::CUIBaseControl BtnGo;
		Core::Classes::UI::CUIBaseControl Lst1;
		Core::Classes::UI::CUIBaseControl Lst2;
		Core::Classes::UI::CUICheckbox ActiveOnly;
	} CellViewWindowControls;

	struct RenderWindowControls_t
	{
		BOOL IsCollisionView;
	} RenderWindowControls;

	WNDPROC OldWndProc;
	DLGPROC OldObjectWindowProc;
	DLGPROC OldCellViewProc;
	DLGPROC OldResponseWindowProc;
	DLGPROC OldRenderWindowProc;

	HWND GetWindow()
	{
		return MainWindow.Handle;
	}

	HWND GetObjectWindow()
	{
		return ObjectWindow.Handle;
	}

	HWND GetCellViewWindow()
	{
		return CellViewWindow.Handle;
	}

	Core::Classes::UI::CUIMainWindow& GetMainWindowObj()
	{
		return MainWindow;
	}

	Core::Classes::UI::CUICustomWindow& GetObjectWindowObj()
	{
		return ObjectWindow;
	}

	Core::Classes::UI::CUICustomWindow& GetCellViewWindowObj()
	{
		return CellViewWindow;
	}

	Core::Classes::UI::CUIMenu& GetMainMenuObj()
	{
		return MainWindow.MainMenu;
	}

	LRESULT WINAPI hk_0x5669D8(void)
	{
		ObjectWindow.Perform(WM_COMMAND, UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW, 0);

		return 0;
	}

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
		EditorUIDarkMode::InitializeThread();

		if (!LogWindow::Initialize())
			MessageBoxA(nullptr, "Failed to create console log window", "Error", MB_ICONERROR);
	}

	bool CreateExtensionMenu()
	{
		BOOL result = TRUE;

		// I tend to write object-oriented

		Core::Classes::UI::CUIMenu* ExtensionSubMenu = new Core::Classes::UI::CUIMenu(Core::Classes::UI::CUIMenu::CreateSubMenu());
		Core::Classes::UI::CUIMenu* LinksSubMenu = new Core::Classes::UI::CUIMenu(Core::Classes::UI::CUIMenu::CreateSubMenu());

		Assert(ExtensionSubMenu);
		Assert(LinksSubMenu);

		// I'll add hiding and showing the log window 

		result = result && ExtensionSubMenu->Append("Show/Hide Log", UI_EXTMENU_SHOWLOG, TRUE, IsWindowVisible(LogWindow::GetWindow()));
		result = result && ExtensionSubMenu->Append("Clear Log", UI_EXTMENU_CLEARLOG);
		result = result && ExtensionSubMenu->Append("Autoscroll Log", UI_EXTMENU_AUTOSCROLL, TRUE, TRUE);
		result = result && ExtensionSubMenu->AppendSeparator();
		result = result && ExtensionSubMenu->Append("Dump Active Forms", UI_EXTMENU_LOADEDESPINFO);
		result = result && ExtensionSubMenu->AppendSeparator();
		result = result && ExtensionSubMenu->Append("Save Hardcoded Forms", UI_EXTMENU_HARDCODEDFORMS);
		result = result && MainWindow.MainMenu.Append("Extensions", *ExtensionSubMenu);

		result = result && LinksSubMenu->Append("Cascadia Wiki", UI_EXTMENU_LINKS_WIKI);
		result = result && MainWindow.MainMenu.Append("Links", *LinksSubMenu);

		// I don't use DeleteMenu when destroying, I don't need to store a pointer and all that.

		delete LinksSubMenu;
		delete ExtensionSubMenu;

		AssertMsg(result, "Failed to create extension submenus");
		return result;
	}

	LRESULT CALLBACK WndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		Core::Classes::UI::CUIMenuItem MenuItem;

		if (Message == WM_CREATE)
		{
			auto createInfo = (const CREATESTRUCT *)lParam;

			if (!_stricmp(createInfo->lpszName, "Creation Kit") && !_stricmp(createInfo->lpszClass, "Creation Kit"))
			{
				// Initialize the original window before adding anything

				// The entire API is in the Creation Kit ANSI, so it's better to use A
				LRESULT status = CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);
				MainWindow = Hwnd;

				// Set font default
				// This is the default value, but I need an object record to create the missing controls
				MainWindow.Font = Core::Classes::UI::CFont("Microsoft Sans Serif", 8, {}, Core::Classes::UI::fqClearTypeNatural, Core::Classes::UI::fpVariable);

				// Getting additional child Windows
				MainWindow.FindToolWindow();

				// Create custom menu controls
				MainWindow.MainMenu = createInfo->hMenu;
				CreateExtensionMenu();

				// All main menus change to uppercase letters
				for (UINT i = 0; i < MainWindow.MainMenu.Count(); i++)
				{
					MenuItem = MainWindow.MainMenu.GetItemByPos(i);
					MenuItem.Text = XUtil::Str::UpperCase(MenuItem.Text);
				}

				Core::Classes::UI::CUIMenu ViewMenu = MainWindow.MainMenu.GetSubMenuItem(2);

				// How annoying is this window Warnings, delete from the menu.
				ViewMenu.RemoveByPos(34);

				MenuItem = ViewMenu.GetItem(UI_FOG_CMD);
				MenuItem.Text = "Fog (No worked, need fix)";
				MenuItem.Enabled = FALSE;

				// Fix show/hide object & cell view windows
				MenuItem = ViewMenu.GetItemByPos(2);
				MenuItem.ID = UI_CMD_SHOWHIDE_OBJECTWINDOW;
				MenuItem.Checked = TRUE;
				MenuItem = ViewMenu.GetItemByPos(3);
				MenuItem.ID = UI_CMD_SHOWHIDE_CELLVIEWWINDOW;
				MenuItem.Checked = TRUE;

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
				// I already have a class that describes the basic functions of Windows, 
				// but there is no point in rewriting it too much, so I'll add hiding and showing the log window.

				if (IsWindowVisible(LogWindow::GetWindow()))
					ShowWindow(LogWindow::GetWindow(), SW_HIDE);
				else
				{
					ShowWindow(LogWindow::GetWindow(), SW_SHOW);
					SetForegroundWindow(LogWindow::GetWindow());
				}

				// Change the checkbox
				MenuItem = MainWindow.MainMenu.GetItem(UI_EXTMENU_SHOWLOG);
				MenuItem.Checked = !MenuItem.Checked;
			}
			return 0;

			case UI_EXTMENU_CLEARLOG:
			{
				PostMessageA(LogWindow::GetWindow(), UI_LOG_CMD_CLEARTEXT, 0, 0);
			}
			return 0;

			case UI_EXTMENU_AUTOSCROLL:
			{
				// Change the checkbox
				MenuItem = MainWindow.MainMenu.GetItem(UI_EXTMENU_AUTOSCROLL);
				MenuItem.Checked = !MenuItem.Checked;

				PostMessageA(LogWindow::GetWindow(), UI_LOG_CMD_AUTOSCROLL, (WPARAM)MenuItem.Checked, 0);
			}
			return 0;

			case UI_EXTMENU_LOADEDESPINFO:
			{
				char filePath[MAX_PATH] = {};
				OPENFILENAME ofnData = { 0 };
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
				ShellExecuteA(nullptr, "open", "https://wiki.falloutcascadia.com/index.php?title=Main_Page", "", "", SW_SHOW);
			}
			return 0;

			case UI_CMD_SHOWHIDE_OBJECTWINDOW:
			{
				ObjectWindow.Visible = !ObjectWindow.Visible;
				if (ObjectWindow.Visible)
					ObjectWindow.Foreground();

				// Change the checkbox
				MenuItem = MainWindow.MainMenu.GetItem(UI_CMD_SHOWHIDE_OBJECTWINDOW);
				MenuItem.Checked = !MenuItem.Checked;
			}
			return 0;

			case UI_CMD_SHOWHIDE_CELLVIEWWINDOW:
			{
				CellViewWindow.Visible = !CellViewWindow.Visible;
				if (CellViewWindow.Visible)
					CellViewWindow.Foreground();

				// Change the checkbox
				MenuItem = MainWindow.MainMenu.GetItem(UI_CMD_SHOWHIDE_CELLVIEWWINDOW);
				MenuItem.Checked = !MenuItem.Checked;
			}
			return 0;

			case UI_COLLISION_GEOM_CMD:
			{
				// CheckMenuItem is called, however, it always gets zero, but eight is written on top, which is equal to MFS_CHECKED.
				// So I'll create a variable. By the way, when loading a new process, the flag is not set, as is the function itself.

				RenderWindowControls.IsCollisionView = !RenderWindowControls.IsCollisionView;
				MainWindow.MainMenu.GetSubMenuItem(2).GetItemByPos(24).Checked = RenderWindowControls.IsCollisionView;
			}
			return CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);

			case UI_FOG_CMD:
			{
			//	MenuItem = MainWindow.MainMenu.GetSubMenuItem(2).GetItem(wParam);
			//	MenuItem.Checked = !MenuItem.Checked;
			}
			return CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);
			}
		}
		else if (Message == WM_SETTEXT && Hwnd == GetWindow())
		{
			// Continue normal execution but with a custom string
			char customTitle[1024];
			sprintf_s(customTitle, "%s [CK64Fixes Rev. F4-%s]", (const char *)lParam, g_GitVersion);

			return CallWindowProcA(OldWndProc, Hwnd, Message, wParam, (LPARAM)customTitle);
		}

		return CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);
	}

	void ResizeObjectWndChildControls(void)
	{
		// The perfectionist in me is dying....

		ObjectWindowControls.TreeList.LockUpdate();
		ObjectWindowControls.ItemList.LockUpdate();
		ObjectWindowControls.EditFilter.LockUpdate();
		ObjectWindowControls.ToggleDecompose.LockUpdate();
		ObjectWindowControls.BtnObjLayout.LockUpdate();

		LONG w_tree = ObjectWindowControls.TreeList.Width;
		LONG w_left = w_tree - ObjectWindowControls.BtnObjLayout.Width + 1;
		ObjectWindowControls.BtnObjLayout.Left = w_left;
		ObjectWindowControls.ToggleDecompose.Left = w_left;
		ObjectWindowControls.ActiveOnly.Width = w_tree;

		w_left = w_left - ObjectWindowControls.EditFilter.Left - 3;
		ObjectWindowControls.EditFilter.Width = w_left;
		ObjectWindowControls.ComboLayout.Width = w_left;

		ObjectWindowControls.ItemList.Left = w_tree + 5;
		ObjectWindowControls.ItemList.Width = ObjectWindow.ClientWidth() - (w_tree + 5);

		RedrawWindow(ObjectWindow.Handle, NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_NOCHILDREN);
		ObjectWindowControls.BtnObjLayout.UnlockUpdate();
		ObjectWindowControls.ToggleDecompose.UnlockUpdate();
		ObjectWindowControls.EditFilter.UnlockUpdate();
		ObjectWindowControls.ItemList.UnlockUpdate();
		ObjectWindowControls.TreeList.UnlockUpdate();
		ObjectWindowControls.BtnObjLayout.Repaint();
		ObjectWindowControls.ToggleDecompose.Repaint();
		ObjectWindowControls.EditFilter.Repaint();
		ObjectWindowControls.ItemList.Repaint();
		ObjectWindowControls.TreeList.Repaint();
	}

	void SetObjectWindowFilter(const std::string& name, const BOOL SkipText, const BOOL actived)
	{
		if (!SkipText)
			ObjectWindowControls.EditFilter.Caption = name;

		ObjectWindowControls.ActiveOnly.Checked = actived;
		// Force the list items to update as if it was by timer
		ObjectWindow.Perform(WM_TIMER, 0x1B58, 0);
	}

	int32_t WINAPI hk_7FF72F57F8F0(const int64_t ObjectListInsertData, TESForm_CK* Form)
	{
		bool allowInsert = true;
		ObjectWindow.Perform(UI_OBJECT_WINDOW_ADD_ITEM, (WPARAM)Form, (LPARAM)& allowInsert);

		if (!allowInsert)
			return 1;

		return ((int32_t(__fastcall*)(int64_t, TESForm_CK*))OFFSET(0x40F8F0, 0))(ObjectListInsertData, Form);
	}

	INT_PTR CALLBACK ObjectWindowProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			ObjectWindow = DialogHwnd;

			// Set font default
			// This is the default value, but I need an object record to create the missing controls
			ObjectWindow.Font = Core::Classes::UI::CFont("Microsoft Sans Serif", 8, {}, Core::Classes::UI::fqClearTypeNatural, Core::Classes::UI::fpVariable);

			ObjectWindowControls.TreeList = ObjectWindow.GetControl(2093);
			ObjectWindowControls.ItemList = ObjectWindow.GetControl(1041);
			ObjectWindowControls.ToggleDecompose = ObjectWindow.GetControl(6027);
			ObjectWindowControls.BtnObjLayout = ObjectWindow.GetControl(6025);
			ObjectWindowControls.ComboLayout = ObjectWindow.GetControl(6024);
			ObjectWindowControls.EditFilter = ObjectWindow.GetControl(2581);
			ObjectWindowControls.Spliter = ObjectWindow.GetControl(2157);
			ObjectWindowControls.ActiveOnly.CreateWnd(ObjectWindow, ObjectWindow.GetControl(UI_OBJECT_WINDOW_ADD_ITEM), UI_OBJECT_WINDOW_ADD_ITEM);

			// Eliminate the flicker when resizing
			ObjectWindowControls.TreeList.Perform(TVM_SETEXTENDEDSTYLE, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);

			// Eliminate the flicker when changing categories
			ListView_SetExtendedListViewStyleEx(ObjectWindowControls.ItemList.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);

			// Erase Icon and SysMenu
			ObjectWindow.Style = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME;

			ObjectWindowControls.BtnObjLayout.Top = ObjectWindowControls.ComboLayout.Top - 1;
			ObjectWindowControls.BtnObjLayout.Height = ObjectWindowControls.ComboLayout.Height + 2;
			ObjectWindowControls.ToggleDecompose.Top = ObjectWindowControls.EditFilter.Top - 1;
			ObjectWindowControls.ToggleDecompose.Height = ObjectWindowControls.EditFilter.Height + 2;
		}
		// Don't let us reduce the window too much
		else if (Message == WM_GETMINMAXINFO)
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 350;
			lpMMI->ptMinTrackSize.y = 500;

			return 0;
		}
		else if (Message == WM_SIZE)
		{
			if (!ObjectWindowControls.StartResize)
			{
				ObjectWindowControls.StartResize = TRUE;
				ResizeObjectWndChildControls();
			}
		}
		else if (Message == UI_OBJECT_WINDOW_ADD_ITEM)
		{
			auto form = reinterpret_cast<const TESForm_CK*>(wParam);
			auto allowInsert = reinterpret_cast<bool*>(lParam);
			*allowInsert = true;

			if (ObjectWindowControls.ActiveOnly.Checked)
			{
				if (form && !form->Active)
					* allowInsert = false;
			}

			return 0;
		}
		else if (Message == WM_COMMAND)
		{
			switch (wParam)
			{
			case UI_OBJECT_WINDOW_ADD_ITEM:
				SetObjectWindowFilter("", TRUE, !ObjectWindowControls.ActiveOnly.Checked);
				return 0;
			case UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW:
				ResizeObjectWndChildControls();
				return 0;
			}
		}

		return OldObjectWindowProc(DialogHwnd, Message, wParam, lParam);
	}

	void SetCellWindowFilter(const BOOL actived)
	{
		CellViewWindowControls.ActiveOnly.Checked = actived;
		// Fake the dropdown list being activated
		CellViewWindow.Perform(WM_COMMAND, MAKEWPARAM(2083, 1), 0);
	}

	void WINAPI hk_7FF70C322BC0(HWND ListViewHandle, TESForm_CK* Form, bool UseImage, int32_t ItemIndex)
	{
		bool allowInsert = true;
		CellViewWindow.Perform(UI_CELL_WINDOW_ADD_ITEM, (WPARAM)Form, (LPARAM)& allowInsert);

		if (!allowInsert)
			return;

		return ((void(__fastcall*)(HWND, TESForm_CK*, bool, int32_t))OFFSET(0x562BC0, 0))(ListViewHandle, Form, UseImage, ItemIndex);
	}
	
	INT_PTR CALLBACK CellViewProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			// This message is called a great many times, especially when updating World Space

			if (!CellViewWindowControls.Initialize)
			{
				CellViewWindow = DialogHwnd;

				// Set font default
				// This is the default value, but I need an object record to create the missing controls
				CellViewWindow.Font = Core::Classes::UI::CFont("Microsoft Sans Serif", 8, {}, Core::Classes::UI::fqClearTypeNatural, Core::Classes::UI::fpVariable);

				CellViewWindowControls.LabelWorldSpace = CellViewWindow.GetControl(1164);
				CellViewWindowControls.NoCellSellected = CellViewWindow.GetControl(1163);
				CellViewWindowControls.Interiors = CellViewWindow.GetControl(2083);
				CellViewWindowControls.LoadedAtTop = CellViewWindow.GetControl(5662);
				CellViewWindowControls.FilteredOnly = CellViewWindow.GetControl(5664);
				CellViewWindowControls.VisibleOnly = CellViewWindow.GetControl(5666);
				CellViewWindowControls.SelectedOnly = CellViewWindow.GetControl(5665);
				CellViewWindowControls.LabelX = CellViewWindow.GetControl(5281);
				CellViewWindowControls.LabelY = CellViewWindow.GetControl(5282);
				CellViewWindowControls.EditX = CellViewWindow.GetControl(5283);
				CellViewWindowControls.EditY = CellViewWindow.GetControl(5099);
				CellViewWindowControls.EditCellFiltered = CellViewWindow.GetControl(2581);
				CellViewWindowControls.BtnGo = CellViewWindow.GetControl(3681);
				CellViewWindowControls.Lst1 = CellViewWindow.GetControl(1155);
				CellViewWindowControls.Lst2 = CellViewWindow.GetControl(1156);
				CellViewWindowControls.ActiveOnly.CreateWnd(CellViewWindow, CellViewWindow.GetControl(UI_CELL_WINDOW_ADD_ITEM), UI_CELL_WINDOW_ADD_ITEM);

				// Eliminate the flicker when changing cells
				ListView_SetExtendedListViewStyleEx(CellViewWindowControls.Lst1.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
				ListView_SetExtendedListViewStyleEx(CellViewWindowControls.Lst2.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);

				ShowWindow(GetDlgItem(DialogHwnd, 1007), SW_HIDE);

				CellViewWindowControls.Initialize = TRUE;
			}
		}
		else if (Message == UI_CELL_WINDOW_ADD_ITEM)
		{
			auto form = reinterpret_cast<const TESForm_CK*>(wParam);
			auto allowInsert = reinterpret_cast<bool*>(lParam);

			*allowInsert = true;

			// Skip the entry if "Show only active forms" is checked
			if (CellViewWindowControls.ActiveOnly.Checked)
			{
				if (form && !form->Active)
					*allowInsert = false;
			}

			return 0;
		}
		// Don't let us reduce the window too much
		else if (Message == WM_GETMINMAXINFO)
		{
			if (lParam)
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
				lpMMI->ptMinTrackSize.x = 700;
				lpMMI->ptMinTrackSize.y = 315;
			}

			return 0;
		}
		else if (Message == WM_COMMAND)
		{
			switch (wParam)
			{
			case UI_CELL_WINDOW_ADD_ITEM:
				SetCellWindowFilter(!CellViewWindowControls.ActiveOnly.Checked);
				return 0;
			}
		}
		else if (Message == WM_SIZE)
		{
			// Fix the "World Space" label positioning on window resize
			CellViewWindowControls.LabelWorldSpace.Width = CellViewWindowControls.Lst1.Width;
		}

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

	INT_PTR CALLBACK RenderWindowProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			RenderWindowControls.IsCollisionView = FALSE;
		}
		else if (Message == WM_KEYUP)
		{
			if (wParam == 'M')
			{
				// If you click on M, the menu will still have the previous state, we will fix this. 
				// However, in fact, there should be two requests to show or hide, but the second one is ignored and this is good.

				MainWindow.MainMenu.GetSubMenuItem(2).GetItemByPos(15).Click();
			}
			else if (wParam == 'S')
			{
				// Fix that only worked with the menu

				MainWindow.MainMenu.GetSubMenuItem(2).GetItemByPos(17).Click();
			}
		}

		return OldRenderWindowProc(DialogHwnd, Message, wParam, lParam);
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