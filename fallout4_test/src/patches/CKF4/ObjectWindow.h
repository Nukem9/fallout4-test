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

	typedef struct tagOBJWND_CONTROLS
	{
		Classes::CUIBaseControl TreeList;
		Classes::CUIBaseControl ItemList;
		Classes::CUIBaseControl ToggleDecompose;
		Classes::CUIBaseControl BtnObjLayout;
		Classes::CUIBaseControl ComboLayout;
		Classes::CUIBaseControl EditFilter;
		Classes::CUIBaseControl Spliter;
		Classes::CUICheckbox ActiveOnly;
	} OBJWND_CONTROLS, *POBJWND_CONTROLS, *LPOBJWND_CONTROLS;

	typedef struct tagOBJWND
	{
		BOOL StartResize;
		OBJWND_CONTROLS Controls;
		Classes::CUICustomWindow ObjectWindow;
	} OBJWND, *POBJWND, *LPOBJWND;

	extern DLGPROC OldDlgProc;
	typedef std::unordered_map<HWND, LPOBJWND> OBJWNDS;

	HWND FIXAPI GetWindow(UINT uId);
	OBJWNDS& FIXAPI GetAllWindowObj(VOID);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	LRESULT FIXAPI hk_0x5669D8(VOID);
	int32_t FIXAPI hk_7FF72F57F8F0(const int64_t ObjectListInsertData, TESForm_CK* Form);
}