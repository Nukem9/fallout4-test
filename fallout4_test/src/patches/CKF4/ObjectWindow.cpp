#include "ObjectWindow.h"
#include "EditorUI.h"

#define UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW		(UI_CUSTOM_MESSAGE + 4)

namespace ObjectWindow
{
	Core::Classes::UI::CUICustomWindow ObjectWindow;

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

	DLGPROC OldDlgProc;

	HWND GetWindow(void)
	{
		return ObjectWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& GetWindowObj(void)
	{
		return ObjectWindow;
	}

	LRESULT WINAPI hk_0x5669D8(void)
	{
		ObjectWindow.Perform(WM_COMMAND, UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW, 0);

		return 0;
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

	void SetObjectWindowFilter(const std::string & name, const BOOL SkipText, const BOOL actived)
	{
		if (!SkipText)
			ObjectWindowControls.EditFilter.Caption = name;

		ObjectWindowControls.ActiveOnly.Checked = actived;
		// Force the list items to update as if it was by timer
		ObjectWindow.Perform(WM_TIMER, 0x1B58, 0);
	}

	int32_t WINAPI hk_7FF72F57F8F0(const int64_t ObjectListInsertData, TESForm_CK * Form)
	{
		bool allowInsert = true;
		ObjectWindow.Perform(UI_OBJECT_WINDOW_ADD_ITEM, (WPARAM)Form, (LPARAM)& allowInsert);

		if (!allowInsert)
			return 1;

		return ((int32_t(__fastcall*)(int64_t, TESForm_CK*))OFFSET(0x40F8F0, 0))(ObjectListInsertData, Form);
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			ObjectWindow = DialogHwnd;

			// Set font default
			// This is the default value, but I need an object record to create the missing controls
//			ObjectWindow.Font = Core::Classes::UI::CFont("Microsoft Sans Serif", 8, {}, Core::Classes::UI::fqClearTypeNatural, Core::Classes::UI::fpVariable);

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
					*allowInsert = false;
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

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}