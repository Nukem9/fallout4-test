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

#include "..\..\[Common]\StdAfx.h"
#include "DMDebugInfoFormWindow.h"
#include "MainWindow.h"
#include "Editor.h"

namespace DMDebugInfoFormWindow {
	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		switch (Message) {
			case WM_COMMAND: {
				auto result = LOWORD(wParam);
				if (result == IDOK) {
					HWND hEdit = GetDlgItem(DialogHwnd, IDC_EDIT_SPECIFY_FORMID_DM);
					if (hEdit) {
						auto len = GetWindowTextLengthA(hEdit);
						if ((len > 8) || !len)
							_MESSAGE("DMDebug: Incorrectly specified FormID");
						else {
							CHAR szBuf[16] = { 0 };
							GetWindowTextA(hEdit, szBuf, 16);
							
							LPSTR unused = NULL;
							DWORD formid = strtoul(szBuf, &unused, 16);

							auto form = GetFormByNumericID(formid);
							if (!form)
								_MESSAGE_FMT("DMDebug: %08X FormID no found", formid);
							else {
								CHAR szBuf2[1024] = { 0 };
								form->DebugInfo(szBuf2, 1024);

								_MESSAGE_FMT("DMDebug: %s", szBuf2);
								_MESSAGE_FMT("\t\t Address 0x%p", form);
								_MESSAGE_FMT("\t\t Flags %08X", form->FormFlag);

								if (form->TypeID == TESForm::FormType::ftReference) {
									_MESSAGE_FMT("\t\t REFR parent cell: %s", ((TESObjectREFR*)form)->ParentCell->GetFullName().c_str());
									_MESSAGE_FMT("\t\t REFR count ref: %d", ((TESObjectREFR*)form)->GetRef());
								}

								hk_EndDialog(DialogHwnd, IDOK);
							}
						}

						hk_EndDialog(DialogHwnd, IDCANCEL);
					}
				}
				else if (result == IDCANCEL || result == IDCLOSE)
					hk_EndDialog(DialogHwnd, IDCANCEL);
			}	  
		}

		return FALSE;
	}

	INT_PTR FIXAPI ShowModal(VOID) {
		return hk_DialogBoxParamA(g_hModule, (LPCSTR)42001, MainWindow::GetWindow(), DlgProc, NULL);
	}
}