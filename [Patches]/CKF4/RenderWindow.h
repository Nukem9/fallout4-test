//////////////////////////////////////////
/*
* Copyright (c) 2020-2022 Perchik71 <email:perchik71@outlook.com>
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
#include "..\..\[EditorAPI]\Core.h"
#include "..\..\[EditorAPI]\TESRender.h"

namespace RenderWindow
{
	class IPicker {
	private:
		BSTArray<TESObjectREFR*> forms;
	public:
		BOOL QAdd(TESObjectREFR* form);
		BOOL QRemove(TESObjectREFR* form);
		VOID QDump(VOID);
	public:
		//__forceinline const BSTArray<TESObjectREFR*>* GetForms(VOID) const { return &forms; }
		const BSTArray<TESObjectREFR*>* GetForms(VOID) const;
		__forceinline NiPoint4 GetCentralSelectionPosition(VOID) const { return TESPICKER->GetCentralSelectionPosition(); }
		__forceinline DWORD GetSelectCount(VOID) const { return TESPICKER->SelectCount; }
	public:
		READ_PROPERTY(GetSelectCount) DWORD SelectCount;
		READ_PROPERTY(GetCentralSelectionPosition) NiPoint4 CentralSelectionPosition;
		READ_PROPERTY(GetForms) const BSTArray<TESObjectREFR*>* Forms;
	public:
		//static VOID hk_Add(HWND hCtlWnd, INT index, BOOL no_unselect);
		//static VOID hk_Remove(HWND hCtlWnd, TESObjectREFR* form);
	public:
		IPicker(VOID) = default;
	};

	BGSRenderWindow* FIXAPI GetRender(VOID);

	extern IPicker Picker;
	extern DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID);
	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID);

	BOOL FIXAPI IsCollisionView(VOID);
	VOID FIXAPI SetCollisionView(const BOOL Value);

	VOID FIXAPI UpdatePositionAndSize(VOID);

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam);
}

// Own storage class for selected objects on the scene, more reliable and thread-safe, for any needs.
// Unfortunately, it uses a list from the Cell View Window.
#define TESPICKER2 (&RenderWindow::Picker)