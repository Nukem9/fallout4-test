#pragma once

#include "../../common.h"
#include "TESForm_CK.h"
#include "TESFile_CK.h"

#include "UIMenus.h"
#include "UIBaseWindow.h"
#include "UICheckboxControl.h"

#include <CommCtrl.h>

#define UI_OBJECT_WINDOW_ADD_ITEM		2579
#define UI_OBJECT_WINDOW_CHECKBOX		2580	// See: resource.rc

namespace ObjectWindow
{
	extern DLGPROC OldDlgProc;

	HWND GetWindow(void);
	Core::Classes::UI::CUICustomWindow& GetWindowObj(void);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	LRESULT WINAPI hk_0x5669D8(void);
	int32_t WINAPI hk_7FF72F57F8F0(const int64_t ObjectListInsertData, TESForm_CK* Form);
}