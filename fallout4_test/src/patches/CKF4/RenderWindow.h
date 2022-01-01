//////////////////////////////////////////
/*
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

namespace RenderWindow
{
	using namespace api;

	class IPicker {
	private:
		BSTArray<TESObjectREFR*> forms;
	public:
		BOOL QAdd(TESObjectREFR* form);
		BOOL QRemove(TESObjectREFR* form);
		VOID QDump(VOID);
	public:
		__forceinline const BSTArray<TESObjectREFR*>* GetForms() const { return &forms; }
	public:
		static VOID hk_Add(HWND hCtlWnd, INT index, BOOL no_unselect);
		static VOID hk_Remove(HWND hCtlWnd, TESObjectREFR* form);
	public:
		IPicker(VOID) = default;
	};

	extern IPicker Picker;
	extern DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID);
	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID);

	BOOL FIXAPI IsCollisionView(VOID);
	VOID FIXAPI SetCollisionView(const BOOL Value);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);
}