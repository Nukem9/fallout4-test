#include "CellViewWindow.h"

namespace CellViewWindow
{
	Classes::CUICustomWindow CellViewWindow;

	struct CellViewWindowControls_t
	{
		BOOL Initialize = FALSE;

		Classes::CUIBaseControl LabelWorldSpace;
		Classes::CUIBaseControl NoCellSellected;
		Classes::CUIBaseControl Interiors;
		Classes::CUIBaseControl LoadedAtTop;
		Classes::CUIBaseControl FilteredOnly;
		Classes::CUIBaseControl VisibleOnly;
		Classes::CUIBaseControl SelectedOnly;
		Classes::CUIBaseControl LabelX;
		Classes::CUIBaseControl LabelY;
		Classes::CUIBaseControl EditX;
		Classes::CUIBaseControl EditY;
		Classes::CUIBaseControl EditCellFiltered;
		Classes::CUIBaseControl BtnGo;
		Classes::CUIBaseControl Lst1;
		Classes::CUIBaseControl Lst2;
		Classes::CUICheckbox ActiveOnly;
	} CellViewWindowControls;

	DLGPROC OldDlgProc;

	HWND WINAPI GetWindow(VOID)
	{
		return CellViewWindow.Handle;
	}

	Classes::CUICustomWindow& WINAPI GetWindowObj(VOID)
	{
		return CellViewWindow;
	}

	void SetCellWindowFilter(const BOOL actived)
	{
		CellViewWindowControls.ActiveOnly.Checked = actived;
		// Fake the dropdown list being activated
		CellViewWindow.Perform(WM_COMMAND, MAKEWPARAM(2083, 1), 0);
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			// This message is called a great many times, especially when updating World Space

			if (!CellViewWindowControls.Initialize)
			{
				CellViewWindowControls.Initialize = TRUE;
				CellViewWindow = DialogHwnd;

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

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}

	void WINAPI hk_7FF70C322BC0(HWND ListViewHandle, TESForm_CK* Form, bool UseImage, int32_t ItemIndex)
	{
		bool allowInsert = true;
		CellViewWindow.Perform(UI_CELL_WINDOW_ADD_ITEM, (WPARAM)Form, (LPARAM)& allowInsert);

		if (!allowInsert)
			return;

		return ((void(__fastcall*)(HWND, TESForm_CK*, bool, int32_t))OFFSET(0x562BC0, 0))(ListViewHandle, Form, UseImage, ItemIndex);
	}
}