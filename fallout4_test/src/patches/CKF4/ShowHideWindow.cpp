#include "ShowHideWindow.h"
#include "UICheckboxControl.h"
#include "Editor.h"

namespace ShowHideWindow
{
	namespace Classes = Core::Classes::UI;
	Classes::CUICustomWindow ShowHideWindow;
	static BOOL Initialize = FALSE;

	DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID)
	{
		return ShowHideWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return ShowHideWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			if (!Initialize)
			{
				Initialize = TRUE;
				ShowHideWindow = DialogHwnd;
			}			
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}