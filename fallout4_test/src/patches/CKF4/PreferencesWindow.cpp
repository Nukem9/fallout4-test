#include "PreferencesWindow.h"
#include "UITheme/TimeOfDay.h"

#include <CommCtrl.h>

namespace PreferencesWindow
{
	namespace ToD = Core::UI::Theme::TimeOfDay;
	Core::Classes::UI::CUICustomWindow PreferencesWindow;
	FLOAT TimeOfDayValueChanged = 10.0f;

	DLGPROC OldDlgProc;
	static CHAR szTabName[MAX_PATH];

	HWND FIXAPI GetWindow(VOID)
	{
		return PreferencesWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return PreferencesWindow;
	}

	VOID FIXAPI SetNewValueTimeOfDay(VOID)
	{
		CHAR szBuf[24];
		sprintf_s(szBuf, "%.2f", TimeOfDayValueChanged);

		POINT Range = {
			(LONG)ToD::OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMIN, 0, 0),
			(LONG)ToD::OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMAX, 0, 0)
		};

		INT32 iPos = (INT32)(((Range.y - Range.x) * TimeOfDayValueChanged) / 24.0f);

		ToD::NewUITimeOfDayComponents.hWndTrackBar.Perform(TBM_SETPOS, TRUE, (LPARAM)iPos);
		ToD::NewUITimeOfDayComponents.hWndEdit.Caption = szBuf;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if ((Message == WM_INITDIALOG) && (!GetWindow()))
		{
			PreferencesWindow = DialogHwnd;
			PreferencesWindow.Caption = "Preferences: Movement";
		}
		else if (Message == WM_NCACTIVATE && (!wParam))
		{
			SetNewValueTimeOfDay();
			PreferencesWindow.Perform(WM_CLOSE, 0, 0);
			PreferencesWindow = NULL;

			return S_OK;
		}
		else if (Message == WM_COMMAND)
		{
			if (LOWORD(wParam) == 0x5C5)
				SetNewValueTimeOfDay();
		}
		else if (Message == WM_NOTIFY)
		{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;
			if ((lpnmhdr->idFrom == 0x6F1) && (lpnmhdr->code == TCN_SELCHANGE))
			{
				INT idPage = TabCtrl_GetCurSel(lpnmhdr->hwndFrom);
				if (idPage == -1)
					PreferencesWindow.Caption = "Preferences";
				else
				{
					TCITEMA item = {0};
					item.mask = TCIF_TEXT;
					item.pszText = szTabName;
					item.cchTextMax = MAX_PATH;

					if (TabCtrl_GetItem(lpnmhdr->hwndFrom, idPage, &item))
						PreferencesWindow.Caption = std::string("Preferences: ") + item.pszText;
				}
			}
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}

	VOID FIXAPI hk_SetInPreferencesToTimeOfDay(HWND hDlg, INT nIDDlgItem, FLOAT value, INT a4)
	{
		if (nIDDlgItem == 0x3E8)
			TimeOfDayValueChanged = value;

		((VOID(__fastcall*)(HWND, INT, FLOAT, INT))OFFSET(0x565160, 0))(hDlg, nIDDlgItem, value, a4);
	}
}