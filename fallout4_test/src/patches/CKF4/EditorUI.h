#pragma once

#include "../../common.h"
#include "TESForm_CK.h"
#include "TESFile_CK.h"

#include "UIMenus.h"
#include "UIBaseWindow.h"
#include "UICheckboxControl.h"

#define UI_EDITOR_TOOLBAR				1

#define UI_OBJECT_WINDOW_ADD_ITEM		2579
#define UI_OBJECT_WINDOW_CHECKBOX		2580	// See: resource.rc

#define UI_CELL_VIEW_ADD_CELL_ITEM		2579
#define UI_CELL_VIEW_CHECKBOX			2580	// See: resource.rc

#define UI_LOG_CMD_ADDTEXT				(WM_APP + 1)
#define UI_LOG_CMD_CLEARTEXT			(WM_APP + 2)
#define UI_LOG_CMD_AUTOSCROLL			(WM_APP + 3)

#define UI_EXTMENU_ID					51001
#define UI_EXTMENU_SHOWLOG				51002
#define UI_EXTMENU_CLEARLOG				51003
#define UI_EXTMENU_AUTOSCROLL			51004
#define UI_EXTMENU_SPACER				51005
#define UI_EXTMENU_LOADEDESPINFO		51006
#define UI_EXTMENU_HARDCODEDFORMS		51007

#define UI_EXTMENU_LINKS_ID				51010
#define UI_EXTMENU_LINKS_WIKI			51011

#define UI_CUSTOM_MESSAGE						52000
#define UI_EDITOR_OPENFORMBYID					(UI_CUSTOM_MESSAGE + 1)	// Sent from the LogWindow on double click
#define UI_CMD_SHOWHIDE_OBJECTWINDOW			(UI_CUSTOM_MESSAGE + 2)
#define UI_CMD_SHOWHIDE_CELLVIEWWINDOW			(UI_CUSTOM_MESSAGE + 3)
#define UI_CMD_CHANGE_SPLITTER_OBJECTWINDOW		(UI_CUSTOM_MESSAGE + 4)
#define UI_CELL_WINDOW_ADD_ITEM					UI_OBJECT_WINDOW_ADD_ITEM
#define UI_PROGRESS_CMD_STEPIT					(UI_CUSTOM_MESSAGE + 5)

namespace EditorUI
{
	extern WNDPROC OldWndProc;
	extern DLGPROC OldObjectWindowProc;
	extern DLGPROC OldCellViewProc;
	extern DLGPROC OldResponseWindowProc;

	HWND GetWindow();
	HWND GetObjectWindow();
	HWND GetCellViewWindow();
	Core::Classes::UI::CUIMainWindow& GetMainWindowObj();
	Core::Classes::UI::CUICustomWindow& GetObjectWindowObj();
	Core::Classes::UI::CUICustomWindow& GetCellViewWindowObj();
	Core::Classes::UI::CUIMenu& GetMainMenuObj();

	void Initialize();
	//bool CreateExtensionMenu(HWND MainWindow, HMENU MainMenu);

	LRESULT CALLBACK WndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	INT_PTR CALLBACK ObjectWindowProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	INT_PTR CALLBACK CellViewProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	INT_PTR CALLBACK ResponseWindowProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);

	BOOL ListViewCustomSetItemState(HWND ListViewHandle, WPARAM Index, UINT Data, UINT Mask);
	void ListViewSelectItem(HWND ListViewHandle, int ItemIndex, bool KeepOtherSelections);
	void ListViewFindAndSelectItem(HWND ListViewHandle, void *Parameter, bool KeepOtherSelections);
	void *ListViewGetSelectedItem(HWND ListViewHandle);
	void ListViewDeselectItem(HWND ListViewHandle, void *Parameter);
	void TabControlDeleteItem(HWND TabControlHandle, uint32_t TabIndex);

	LRESULT WINAPI hk_0x5669D8(void);
	int32_t WINAPI hk_7FF72F57F8F0(const int64_t ObjectListInsertData, TESForm_CK* Form);
	void WINAPI hk_7FF70C322BC0(HWND ListViewHandle, TESForm_CK* Form, bool UseImage, int32_t ItemIndex);
	LRESULT WINAPI hk_SetSettingsPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WINAPI hk_SetTextPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}