#pragma once

#include "../../common.h"
#include "TESForm_CK.h"
#include "TESFile_CK.h"

#include "UIMenus.h"
#include "UIBaseWindow.h"
#include "UICheckboxControl.h"

#include <CommCtrl.h>

#define UI_CELL_VIEW_ADD_CELL_ITEM		2579
#define UI_CELL_VIEW_CHECKBOX			2580	// See: resource.rc

#define UI_CELL_WINDOW_ADD_ITEM			2579

namespace CellViewWindow
{
	namespace Classes = Core::Classes::UI;

	extern DLGPROC OldDlgProc;

	HWND WINAPI GetWindow(VOID);
	Classes::CUICustomWindow& WINAPI GetWindowObj(VOID);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	void WINAPI hk_7FF70C322BC0(HWND ListViewHandle, TESForm_CK* Form, bool UseImage, int32_t ItemIndex);
}