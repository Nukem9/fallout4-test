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
	namespace Classes = Core::Classes::UI;

	extern DLGPROC OldDlgProc;

	HWND WINAPI GetWindow(VOID);
	Classes::CUICustomWindow& WINAPI GetWindowObj(VOID);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	LRESULT WINAPI hk_0x5669D8(VOID);
	int32_t WINAPI hk_7FF72F57F8F0(const int64_t ObjectListInsertData, TESForm_CK* Form);
}