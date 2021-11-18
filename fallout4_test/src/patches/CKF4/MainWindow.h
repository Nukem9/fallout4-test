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

#include "UIMenus.h"
#include "UIBaseWindow.h"
#include "../../api/EditorAPI.h"

#define UI_EDITOR_TOOLBAR					1
#define UI_LOG_CMD_ADDTEXT					(WM_APP + 1)
#define UI_LOG_CMD_CLEARTEXT				(WM_APP + 2)
#define UI_LOG_CMD_AUTOSCROLL				(WM_APP + 3)
#define UI_EXTMENU_ID						51001
#define UI_EXTMENU_SHOWLOG					51002
#define UI_EXTMENU_CLEARLOG					51003
#define UI_EXTMENU_AUTOSCROLL				51004
#define UI_EXTMENU_SPACER					51005
#define UI_EXTMENU_LOADEDESPINFO			51006
#define UI_EXTMENU_HARDCODEDFORMS			51007
#define UI_EXTMENU_LINKS_ID					51010
#define UI_EXTMENU_LINKS_HOME				51011
#define UI_EXTMENU_LINKS_MATERIALEDITOR		51012
#define UI_EXTMENU_LINKS_WIKI				51013
#define UI_EXTMENU_DEVMODE_DEBUGINFOFORM	51014
#define UI_SHOW_MATERIALEDITOR				41514
#define UI_COLLISION_GEOM_CMD				0x9CFD
#define UI_FOG_CMD							0x9FE9
#define UI_SKY_TOGGLE_CMD					0x9D1F
#define UI_EDITOR_OPENFORMBYID				(UI_CUSTOM_MESSAGE + 1)	// Sent from the LogWindow on double click
#define UI_CMD_SHOWHIDE_OBJECTWINDOW		(UI_CUSTOM_MESSAGE + 2)
#define UI_CMD_SHOWHIDE_CELLVIEWWINDOW		(UI_CUSTOM_MESSAGE + 3)

namespace MainWindow
{
	namespace Classes = Core::Classes::UI;
	using namespace api;

	extern WNDPROC OldWndProc;

	BOOL FIXAPI IsActive(VOID);
	HWND FIXAPI GetWindow(VOID);
	Classes::CUIMainWindow& FIXAPI GetWindowObj(VOID);
	Classes::CUIMenu& FIXAPI GetMainMenuObj(VOID);

	LRESULT CALLBACK WndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
}