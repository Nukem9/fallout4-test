#include "ActorWindow.h"

namespace ActorWindow
{
	Core::Classes::UI::CUICustomWindow RenderViewWindow;

	DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID)
	{
		return RenderViewWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return RenderViewWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			RenderViewWindow = DialogHwnd;
		}
		else if (Message == WM_MOVE)
		{
			// fix flick
			return S_OK;
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}