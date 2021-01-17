#pragma once

#include "../../common.h"
#include "TESForm_CK.h"
#include "TESFile_CK.h"

#include "UIMenus.h"
#include "UIBaseWindow.h"
#include "UICheckboxControl.h"

#include <CommCtrl.h>

#define UI_LISTVIEW_PLUGINS				1056	// See: resource.rc
#define UI_EDIT_SEARCH_PLUGIN_BY_NAME	(UI_CUSTOM_MESSAGE + 4)	

namespace DataWindow
{
	extern DLGPROC OldDlgProc;

	HWND GetWindow(void);
	Core::Classes::UI::CUICustomWindow& GetWindowObj(void);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);
}