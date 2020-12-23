#pragma once

#include "../../common.h"
#include "TESForm_CK.h"
#include "TESFile_CK.h"

#include "UIMenus.h"
#include "UIBaseWindow.h"

namespace ResponseWindow
{
	extern DLGPROC OldDlgProc;

	HWND GetWindow(void);
	Core::Classes::UI::CUICustomWindow& GetWindowObj(void);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);
}