//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#pragma once

#include "../../common.h"
#include "../../api/CommIncAPI.h"

#include "UIMenus.h"
#include "UIBaseWindow.h"
#include "UICheckboxControl.h"

#include <CommCtrl.h>

#define UI_CELL_VIEW_FILTER_LABEL				1165	// See: resource.rc
#define UI_CELL_VIEW_ADD_CELL_ITEM				2579
#define UI_CELL_VIEW_CHECKBOX					2580	// See: resource.rc
#define UI_CELL_VIEW_CHECK_ACTIVE_CELL_OBJECTS	2582	// See: resource.rc
#define UI_CELL_VIEW_ADD_CELL_OBJECT_ITEM		2583	
#define UI_CELL_WINDOW_ADD_ITEM					2579
#define UI_CELL_VIEW_FILTER_EDITTEXT			2585	// See: resource.rc
#define UI_CELL_VIEW_WORLDSPACE					2083	// See: resource.rc
#define UI_CELL_VIEW_WORLDSPACE_COMBOBOX		5662	// See: resource.rc	
#define UI_TIMER_FILTER_CELL					7799

namespace CellViewWindow
{
	namespace Classes = Core::Classes::UI;
	using namespace api;

	struct CellViewWindowControls_t
	{
		BOOL Initialize = FALSE;
		BOOL NowFiltering = FALSE;
		BOOL LockFiltering = FALSE;
		std::string FilterCell;

		Classes::CUIBaseControl LabelWorldSpace;
		Classes::CUIBaseControl LabelFilter;
		Classes::CUIBaseControl NoCellSellected;
		Classes::CUIBaseControl WorldSpaceCombo;
		Classes::CUIBaseControl LoadedAtTop;
		Classes::CUIBaseControl FilteredOnly;
		Classes::CUIBaseControl VisibleOnly;
		Classes::CUIBaseControl SelectedOnly;
		Classes::CUIBaseControl LabelX;
		Classes::CUIBaseControl LabelY;
		Classes::CUIBaseControl EditX;
		Classes::CUIBaseControl EditY;
		Classes::CUIBaseControl EditCellObjsFiltered;
		Classes::CUIBaseControl BtnGo;
		Classes::CUIBaseControl Lst1;
		Classes::CUIBaseControl Lst2;
		Classes::CUIBaseControl EditFilterCell;
		Classes::CUICheckbox ActiveOnly;
		Classes::CUICheckbox ActiveOnlyObjs;
	};

	extern DLGPROC OldDlgProc;
	extern CellViewWindowControls_t CellViewWindowControls;

	HWND FIXAPI GetWindow(VOID);
	Classes::CUICustomWindow& FIXAPI GetWindowObj(VOID);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	VOID FIXAPI hk_7FF70C322BC0(HWND ListViewHandle, TESCell* Form, BOOL UseImage, int32_t ItemIndex);
	INT32 FIXAPI hk_call_5A43B5(HWND** ListViewHandle, TESObjectREFR** Form, INT64 a3);
}