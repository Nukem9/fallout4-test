//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

#include "..\..\[Common]\StdAfx.h"

#include "Editor.h"
#include "EditorUI.h"
#include "MainWindow.h"
#include "ActorWindow.h"
#include "UIDialogManager.h"

#include "..\..\[EditorAPI]\TESRender.h"

BOOL bFogToggle = TRUE;
BOOL bAllowPoolMessage = FALSE;
BOOL bExtremelyMode = FALSE;

#pragma warning(disable : 26819; disable : 26451; disable : 6387; disable : 6001)

// clang-format off
struct
{
	LPCSTR Section;
	LPCSTR Key;
	std::string Value;
	std::string ExtremelyValue;
} CK_Settings[] = {
	// CreationKit.ini or CreationKitCustom.ini
	{ "General",		"bAllowMultipleEditors",				"1", "1" },
	{ "General",		"bAllowMultipleMasterLoads",			"1", "1" },
	{ "General",		"bSkipValidateInfos",					"1", "0" },
	{ "General",		"bSkipValidateForms",					"1", "0" },
	{ "General",		"bDisableDuplicateReferenceCheck",		"1", "0" },
	{ "General",		"bCheckForMultiFileForms",				"0", "0" },
	{ "General",		"bCheckForRefCellChanges",				"0", "0" },
	{ "General",		"iCheckCellRegionsOnInit",				"0", "0" },
	{ "General",		"bCheckOutWorldspacesOnInit",			"0", "0" },
	{ "General",		"bCheckDoorCollisionOnInit",			"0", "0" },
	{ "General",		"bFixBadLocRefsOnInit",					"0", "0" },
	{ "General",		"bFixBadBorderRegionDataOnInit",		"0", "0" },
	{ "General",		"bFixPersistenceOnRefInit",				"0", "0" },
	{ "General",		"bForceCheckOutOnRefFix",				"0", "0" },
	{ "General",		"bCheckLevActorsOnInit",				"0", "0" },
	{ "General",		"bUseToolTips",							"0", "0" },
	{ "General",		"bOnlyActiveFileForRefFix",				"0", "0" },
	{ "General",		"bFixAIPackagesOnLoad",					"0", "0" },
	{ "General",		"bCheckInventoryItemNameLength",		"0", "0" },
	{ "General",		"bWarnOnGameSettingLoad",				"0", "0" },
	{ "General",		"bCheckHairOnInit",						"0", "0" },
	{ "General",		"bCheckEyesOnInit",						"0", "0" },
	{ "General",		"bForbidNifErrorMarkers",				"1", "0" },
	{ "General",		"bEditorHotLoading",					"1", "1" },
	{ "General",		"bHideImportantWarnings",				"1", "0" },
	{ "General",		"bAllowResourceReloading",				"1", "1" },
	{ "General",		"bDoMultithreadedVisQuery",				"1", "1" },
	// set 1 about users asked.
	{ "General",		"bUpdateCellViewToMatchRenderWindow",	"1", "1" },
	{ "General",		"bUseMenuLoadCellPreload",				"0", "0" },
	{ "Archive",		"bUseArchives",							"1", "1" },
	// bAutoloadTESFileArchives option it works as it should, 
	// although I have already created my own handler and this option preclude me.
	{ "Archive",		"bAutoloadTESFileArchives",				"0", "0" },
	{ "Archive",		"bInvalidateOlderFiles",				"1", "1" },
	{ "Animation",		"bUseVariableCache",					"0", "0" },
	{ "Animation",		"bIgnoreNIFFlags",						"1", "1" },
	{ "Animation",		"bDisplayMarkWarning",					"0", "0" },
	{ "Animation",		"bSkipAnimationTextExport",				"0", "0" },
	{ "Bethesda.net",	"bEnablePlatformSelection",				"1", "1" },
	{ "Bethesda.net",	"bEnableXB1",							"1", "1" },
	{ "Bethesda.net",	"bEnablePS4",							"1", "1" },
	{ "Display",		"bDeferredCommands",					"1", "1" },
	{ "Display",		"bDynamicObjectQueryManager",			"0", "0" },
	{ "Display",		"bMultiThreadedAccumulation",			"1", "1" },
	{ "Display",		"bMultiThreadedRenderingUNP",			"1", "1" },
	{ "Display",		"iTexMipMapMinimum",					"0", "0" },
	{ "Display",		"bUsePreCulledObjects",					"0", "0" },
	{ "Weather",		"bFogEnabled",							"1", "1" },
	{ "Messages",		"bBlockMessageBoxes",					"1", "1" },
	{ "Messages",		"bShowErrorMessages",					"0", "0" },
	{ "Messages",		"bUseWindowsMessageBox",				"1", "1" },
	{ "Messages",		"bSkipInitializationFlows",				"1", "1" },
	{ "Messages",		"bSkipProgramFlows",					"1", "1" },
	{ "Messages",		"bAllowYesToAll",						"1", "1" },
	{ "Messages",		"bAllowFileWrite",						"0", "0" },
	{ "Messages",		"bDisableWarning",						"1", "1" },
	{ "Messages",		"bEnableAudio",							"0", "0" },
	{ "Messages",		"bFaceGenWarnings",						"0", "0" },
	{ "Messages",		"bDisableAssertQueuing",				"1", "1" },
	{ "Messages",		"bShowMissingLipWarnings",				"0", "0" },
	{ "Messages",		"bShowMissingAudioWarnings",			"0", "0" },
	{ "Messages",		"bTopMostWarnings",						"0", "0" },
};

struct DialogOverrideData {
	DLGPROC DialogFunc;	// Original function pointer
	BOOL IsDialog;		// True if it requires EndDialog()
};

std::recursive_mutex g_DialogMutex;
std::unordered_map<HWND, DialogOverrideData> g_DialogOverrides;
thread_local DialogOverrideData DlgData;

BOOL FIXAPI OpenPluginSaveDialog(HWND ParentWindow, LPCSTR BasePath, BOOL IsESM, LPSTR Buffer, uint32_t BufferSize, LPCSTR Directory) {
	if (!BasePath)
		BasePath = "\\Data";

	LPCSTR filter = "TES Plugin Files (*.esp)\0*.esp\0TES Master Files (*.esm)\0*.esm\0\0";
	LPCSTR title = "Select Target Plugin";
	LPCSTR extension = "esp";

	if (IsESM) {
		filter = "TES Master Files (*.esm)\0*.esm\0\0";
		title = "Select Target Master";
		extension = "esm";
	}

	auto result = ((BOOL(__fastcall*)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPVOID, BOOL, BOOL, LPSTR, uint32_t, LPCSTR, LPVOID))
		OFFSET(0x6461B0, 0))(ParentWindow, BasePath, filter, title, extension, NULL, FALSE, TRUE, Buffer, BufferSize, Directory, NULL);

	if (result && g_INI_CK_CfgCustom->GetBoolean("General", "bUseVersionControl", FALSE)) {
		std::string sbuf = Buffer;
		auto ibegin = sbuf.find_last_of('\\');
		if (ibegin == sbuf.npos) {
			ibegin = sbuf.find_last_of('/');
			if (ibegin == sbuf.npos)
				goto end_func;
			else
				sbuf = sbuf.substr(ibegin + 1);
		}
		else
			sbuf = sbuf.substr(ibegin + 1);

		strcpy_s(Buffer, BufferSize, sbuf.c_str());
	}

end_func:
	return result;
}

INT_PTR CALLBACK DialogFuncOverride(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	DLGPROC proc = NULL;

	g_DialogMutex.lock(); {
		if (auto itr = g_DialogOverrides.find(hwndDlg); itr != g_DialogOverrides.end())
			proc = itr->second.DialogFunc;

		// if (is new entry)
		if (!proc) {
			g_DialogOverrides[hwndDlg] = DlgData;
			proc = DlgData.DialogFunc;

			DlgData.DialogFunc = NULL;
			DlgData.IsDialog = FALSE;
		}

		// Purge old entries every now and then
		if (g_DialogOverrides.size() >= 50) {
			for (auto itr = g_DialogOverrides.begin(); itr != g_DialogOverrides.end();) {
				if (itr->first == hwndDlg || IsWindow(itr->first)) {
					itr++;
					continue;
				}

				itr = g_DialogOverrides.erase(itr);
			}
		}
	}
	g_DialogMutex.unlock();

	return proc(hwndDlg, uMsg, wParam, lParam);
}

INT_PTR CALLBACK DialogFuncAbout(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG: {
		Core::Classes::UI::CUIMonitor monitor = Core::Classes::UI::Screen.MonitorFromWindow(hwndDlg);
		Core::Classes::UI::CRECT wa = monitor.WorkAreaRect;

		MoveWindow(hwndDlg, wa.Left + ((wa.Width - 476) >> 1), wa.Top + ((wa.Height - 476) >> 1), 476, 476, TRUE);
		ShowWindow(hwndDlg, SW_SHOW);
		return (INT_PTR)TRUE;
	}
	case WM_ACTIVATE: {
		if (LOWORD(wParam) == WA_INACTIVE) {
			hk_EndDialog(hwndDlg, 1);
			return (INT_PTR)TRUE;
		}
	}
	}

	return (INT_PTR)FALSE;
}

HWND WINAPI hk_CreateDialogParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	// EndDialog MUST NOT be used
	DlgData.DialogFunc = lpDialogFunc;
	DlgData.IsDialog = FALSE;

	// FIX: 
	// Bug report "pra": Cannot use CK to preview NIFs
	// The compiler messed up the type, now I'm strictly without checking the type, I bring it to another type.
	DWORD m_lpTemplateName = reinterpret_cast<DWORD>(lpTemplateName);

	switch (m_lpTemplateName) {
	// Override certain default dialogs to use this DLL's resources
	case 100:
		m_lpTemplateName = 235;
		DlgData.DialogFunc = DialogFuncAbout;
	case 235:
	case 122:
	case 162:
	case 165:
	case 166:
	case 175:
	case 220:
	case 243:
	case 252:
	case 279:
	case 316:
	case 350:
	case 468:
		hInstance = (HINSTANCE)&__ImageBase;
		break;
	// Actor Dlg
	case 3202:
		ActorWindow::OldDlgProc = DlgData.DialogFunc;
		DlgData.DialogFunc = ActorWindow::DlgProc;
		break;
	}
	
	if ((g_i8DialogMode > 0) && g_DialogManager) {
		Core::Classes::UI::jDialog* dialog = NULL;

		switch (g_i8DialogMode) {
		case 1:
			dialog = g_DialogManager->GetDialog((ULONG)m_lpTemplateName, Core::Classes::UI::jdt8pt);
			break;
		case 2:
			dialog = g_DialogManager->GetDialog((ULONG)m_lpTemplateName, Core::Classes::UI::jdt10pt);
			break;
		case 3:
			dialog = g_DialogManager->GetDialog((ULONG)m_lpTemplateName, Core::Classes::UI::jdt12pt);
			break;
		}

		if (dialog)
			return dialog->Show(hWndParent, DialogFuncOverride, dwInitParam, hInstance);
	}

	return CreateDialogParamA(hInstance, (LPCSTR)m_lpTemplateName, hWndParent, DialogFuncOverride, dwInitParam);
}

INT_PTR WINAPI hk_DialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	// EndDialog MUST be used
	DlgData.DialogFunc = lpDialogFunc;
	DlgData.IsDialog = TRUE;

	// FIX: 
	// Bug report "pra": Cannot use CK to preview NIFs
	// The compiler messed up the type, now I'm strictly without checking the type, I bring it to another type.
	DWORD m_lpTemplateName = reinterpret_cast<DWORD>(lpTemplateName);

	switch (m_lpTemplateName) {
		// Override certain default dialogs to use this DLL's resources
	case 100:
		m_lpTemplateName = 235;
		DlgData.DialogFunc = DialogFuncAbout;
	case 235:
	case 122:
	case 162:
	case 165:
	case 166:
	case 175:
	case 220:
	case 243:
	case 252:
	case 279:
	case 316:
	case 350:
	case 468:
		hInstance = (HINSTANCE)&__ImageBase;
		break;
		// Actor Dlg
	case 3202:
		ActorWindow::OldDlgProc = DlgData.DialogFunc;
		DlgData.DialogFunc = ActorWindow::DlgProc;
		break;
	}

	if ((g_i8DialogMode > 0) && g_DialogManager) {
		Core::Classes::UI::jDialog* dialog = NULL;

		switch (g_i8DialogMode) {
		case 1:
			dialog = g_DialogManager->GetDialog((ULONG)m_lpTemplateName, Core::Classes::UI::jdt8pt);
			break;
		case 2:
			dialog = g_DialogManager->GetDialog((ULONG)m_lpTemplateName, Core::Classes::UI::jdt10pt);
			break;
		case 3:
			dialog = g_DialogManager->GetDialog((ULONG)m_lpTemplateName, Core::Classes::UI::jdt12pt);
			break;
		}

		if (dialog)
			return dialog->ShowModal(hWndParent, DialogFuncOverride, dwInitParam, hInstance);
	}

	return DialogBoxParamA(hInstance, (LPCSTR)m_lpTemplateName, hWndParent, DialogFuncOverride, dwInitParam);
}

BOOL WINAPI hk_EndDialog(HWND hDlg, INT_PTR nResult) {
	std::lock_guard lock(g_DialogMutex);

	// Fix for the CK calling EndDialog on a CreateDialogParamA window
	if (auto itr = g_DialogOverrides.find(hDlg); itr != g_DialogOverrides.end() && !itr->second.IsDialog) {
		DestroyWindow(hDlg);
		return TRUE;
	}

	return EndDialog(hDlg, nResult);
}

LRESULT WINAPI hk_SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	if (hWnd && Msg == WM_DESTROY) {
		std::lock_guard lock(g_DialogMutex);

		// If this is a dialog, we can't call DestroyWindow on it
		if (auto itr = g_DialogOverrides.find(hWnd); itr != g_DialogOverrides.end()) {
			if (!itr->second.IsDialog)
				DestroyWindow(hWnd);
		}

		return 0;
	}

	return SendMessageA(hWnd, Msg, wParam, lParam);
}

VOID FIXAPI UpdateObjectWindowTreeView(LPVOID Thisptr, HWND ControlHandle, INT64 Unknown) {
	SendMessageA(ControlHandle, WM_SETREDRAW, FALSE, 0);
	fastCall<VOID, LPVOID, HWND, INT64>(0x0413EB0, Thisptr, ControlHandle, Unknown);
	SendMessageA(ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(ControlHandle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

VOID FIXAPI UpdateCellViewCellList(LPVOID Thisptr, HWND ControlHandle, INT64 Unknown) {
	SendMessageA(ControlHandle, WM_SETREDRAW, FALSE, 0);
	fastCall<VOID, LPVOID, HWND, INT64>(0x06434C0, Thisptr, ControlHandle, Unknown);
	SendMessageA(ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(ControlHandle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

VOID FIXAPI UpdateCellViewObjectList(LPVOID Thisptr, HWND *ControlHandle) {
	SendMessageA(*ControlHandle, WM_SETREDRAW, FALSE, 0);
	fastCall<VOID, LPVOID, HWND*>(0x05A4320, Thisptr, ControlHandle);
	SendMessageA(*ControlHandle, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(*ControlHandle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
}

BOOL g_UseDeferredDialogInsert;
HWND g_DeferredListView;
HWND g_DeferredComboBox;
uintptr_t g_DeferredStringLength;
BOOL g_AllowResize;
tbb::concurrent_vector<std::pair<LPCSTR, LPVOID>> g_DeferredMenuItems;

VOID FIXAPI ResetUIDefer(VOID) {
	g_UseDeferredDialogInsert = FALSE;
	g_DeferredListView = NULL;
	g_DeferredComboBox = NULL;
	g_DeferredStringLength = 0;
	g_AllowResize = FALSE;
	g_DeferredMenuItems.clear();
}

VOID FIXAPI BeginUIDefer(VOID) {
	ResetUIDefer();
	g_UseDeferredDialogInsert = TRUE;
}

//VOID FIXAPI hk_vsprintf_autosave(LPSTR lpBuffer, UINT uBufferSize, LPCSTR lpFormat, LPCSTR lpDefaultName) {
//	static LPCSTR format = "autosave_%s_%s.bak";
//	auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//	// Mon Oct 2 00:59 : 08 2017
//	std::string stime = XUtil::Str::LowerCase(XUtil::Str::trim(std::ctime(&time)));
//	XUtil::Str::replaceAll(stime, " ", "_");
//	XUtil::Str::replaceAll(stime, ":", "_");
//
//	if (FileHandler && FileHandler->ActiveMod) {
//		auto name = FileHandler->ActiveMod->FileName;
//
//		if (!name.Length())
//			goto l_default;
//
//		sprintf_s(lpBuffer, uBufferSize, format, *name, stime.c_str());
//		return;
//	}
//
//	l_default:
//	sprintf_s(lpBuffer, uBufferSize, format, lpDefaultName, stime.c_str());
//}

VOID FIXAPI EndUIDefer(VOID) {
	if (!g_UseDeferredDialogInsert)
		return;

	if (g_DeferredListView) {
		SendMessage(g_DeferredListView, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(g_DeferredListView, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN);
	}

	if (!g_DeferredMenuItems.empty()) {
		const HWND control = g_DeferredComboBox;

		// Sort alphabetically if requested to try and speed up inserts
		LONG style = GetWindowLongPtrA(control, GWL_STYLE);
		BOOL sortedEnabled = (style & CBS_SORT) == CBS_SORT;
		if (sortedEnabled) {
			std::sort(g_DeferredMenuItems.begin(), g_DeferredMenuItems.end(),
				[](const std::pair<LPCSTR, LPVOID>& a, const std::pair<LPCSTR, LPVOID>& b) -> BOOL
			{
				return _stricmp(a.first, b.first) > 0;
			});

			// Disable sort
			SetWindowLongPtrA(control, GWL_STYLE, style & ~CBS_SORT);
		}

		INT32 finalWidth = 0;

		SendMessageA(control, CB_INITSTORAGE, g_DeferredMenuItems.size(), g_DeferredStringLength * sizeof(CHAR) + (g_DeferredMenuItems.size() * 8));

		if (HDC hdc = GetDC(control); hdc) {
			EditorUI::ComboBox_Redraw(control, FALSE);

			// Pre-calculate font widths for resizing, starting with TrueType
			INT32 fontWidths[UCHAR_MAX + 1];
			ABC trueTypeFontWidths[UCHAR_MAX + 1];

			if (!GetCharABCWidthsA(hdc, 0, ARRAYSIZE(trueTypeFontWidths) - 1, trueTypeFontWidths)) {
				BOOL result = GetCharWidthA(hdc, 0, ARRAYSIZE(fontWidths) - 1, fontWidths);
				AssertMsg(result, "Failed to determine any font widths");
			}
			else {
				for (INT32 i = 0; i < ARRAYSIZE(fontWidths); i++)
					fontWidths[i] = trueTypeFontWidths[i].abcB;
			}
			
			//_TIMING_START;

			// Insert everything all at once
			for (auto [display, value] : g_DeferredMenuItems) {
				LRESULT index = ComboBox_AddString(control, display);
				INT32 lineSize = 0;

				if (index != CB_ERR && index != CB_ERRSPACE)
					ComboBox_SetItemData(control, index, value);

				for (LPCSTR c = display; *c != '\0'; c++)
					lineSize += fontWidths[*c];

				finalWidth = std::max<INT32>(finalWidth, lineSize);

				QMemFree((LPVOID)display);
			}

			//_TIMING_END_FMT("The count of elements analyzed: %d for control: %d", g_DeferredMenuItems.size(), (INT)control);

			EditorUI::ComboBox_Redraw(control, TRUE);
			ReleaseDC(control, hdc);
		}
		
		if (sortedEnabled)
			// Enable sort (if it was exists)
			SetWindowLongPtrA(control, GWL_STYLE, style);

		// Resize to fit
		if (g_AllowResize) {
			LRESULT currentWidth = SendMessageA(control, CB_GETDROPPEDWIDTH, 0, 0);

			if (finalWidth > currentWidth)
				SendMessageA(control, CB_SETDROPPEDWIDTH, finalWidth, 0);
		}
	}

	ResetUIDefer();
}

VOID FIXAPI InsertComboBoxItem(HWND ComboBoxHandle, LPCSTR DisplayText, LPVOID Value, BOOL AllowResize) {
	if (!ComboBoxHandle)
		return;

	if (!DisplayText)
		DisplayText = "NONE";

	if (g_UseDeferredDialogInsert) {
		AssertMsg(!g_DeferredComboBox || (g_DeferredComboBox == ComboBoxHandle), "Got handles to different combo boxes? Reset probably wasn't called.");

		g_DeferredComboBox = ComboBoxHandle;
		g_DeferredStringLength += strlen(DisplayText) + 1;
		g_AllowResize |= AllowResize;

		// A copy must be created since lifetime isn't guaranteed after this function returns
		g_DeferredMenuItems.emplace_back(QStrDup(DisplayText), Value);
	}
	else {
		if (AllowResize) {
			if (HDC hdc = GetDC(ComboBoxHandle); hdc) {
				if (SIZE size; GetTextExtentPoint32A(hdc, DisplayText, (INT32)strlen(DisplayText), &size)) {
					LRESULT currentWidth = SendMessageA(ComboBoxHandle, CB_GETDROPPEDWIDTH, 0, 0);

					if (size.cx > currentWidth)
						SendMessageA(ComboBoxHandle, CB_SETDROPPEDWIDTH, size.cx, 0);
				}

				ReleaseDC(ComboBoxHandle, hdc);
			}
		}

		LRESULT index = SendMessageA(ComboBoxHandle, CB_ADDSTRING, 0, (LPARAM)DisplayText);

		if (index != CB_ERR && index != CB_ERRSPACE)
			SendMessageA(ComboBoxHandle, CB_SETITEMDATA, index, (LPARAM)Value);
	}
}

VOID FIXAPI InsertListViewItem(HWND ListViewHandle, LPVOID Parameter, BOOL UseImage, INT32 ItemIndex) {
	if (ItemIndex == -1)
		ItemIndex = INT_MAX;

	LVITEMA item;
	memset(&item, 0, sizeof(item));

	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.iItem = ItemIndex;
	item.lParam = (LPARAM)Parameter;
	item.pszText = LPSTR_TEXTCALLBACK;
	
	if (UseImage) {
		item.mask |= LVIF_IMAGE;
		item.iImage = I_IMAGECALLBACK;
	}

	if (g_UseDeferredDialogInsert) {
		AssertMsg(!g_DeferredListView || (g_DeferredListView == ListViewHandle), "Got handles to different list views? Reset probably wasn't called.");

		if (!g_DeferredListView) {
			g_DeferredListView = ListViewHandle;
			SendMessageA(ListViewHandle, WM_SETREDRAW, FALSE, 0);
		}
	}

	ListView_InsertItem(ListViewHandle, &item);
}

VOID FIXAPI QuitHandler(VOID) {
	TerminateProcess(GetCurrentProcess(), 0);
}

VOID FIXAPI hk_call_140906407(INT64 a1, INT64 a2, INT64 a3) {
	if (a2)
		fastCall<VOID>(0x2A6B230, a1, a2, a3);
}

BOOL FIXAPI hk_call_12E852C(HWND RichEditControl, LPCSTR Text) {
	SendMessageA(RichEditControl, EM_LIMITTEXT, 500000, 0);
	return SetWindowTextA(RichEditControl, Text);
}

/*
==================
hk_call_2511176

For some reason, an object whose address to the function is garbage gets here.

CONTEXT:  (.ecxr)
rax=000002db7f90f850 rbx=000000f780cfea40 rcx=000002db7f90ed10
rdx=000000f780cfea40 rsi=000002db299aa800 rdi=000002db7f90ed10
rip=00007ff77f381176 rsp=000000f780cfe9f0 rbp=000000f780cfeb80
 r8=0000000000000000  r9=0000000000000016 r10=000000000000000b
r11=000000f780cfea18 r12=0000000000000001 r13=00000000001506ec
r14=0000000000000000 r15=0000000000000000
iopl=0         nv up ei pl zr na po nc
cs=0033  ss=002b  ds=002b  es=002b  fs=0053  gs=002b             efl=00010246
CreationKit+0x2511176:
00007ff7`7f381176 ff9090010000    call    qword ptr [rax+190h] ds:000002db`7f90f9e0=bd8a591342b6b52e

This happens when you close the application
==================
*/
VOID FIXAPI hk_call_2511176(LPVOID a1, LPVOID a2, LPVOID a3) {
	if (a1) {
		uintptr_t f = (uintptr_t)(*(PINT64)a1);
		if ((g_ModuleBase & 0xffffffff00000000) == (*((uintptr_t*)(f + 0x190)) & 0xffffffff00000000))
			thisVirtualCall<VOID>(0x190, a1, a2, 0);
	}
}

/*
==================
hk_call_F8CAF3
==================
*/
BOOL FIXAPI hk_call_F8CAF3(VOID) {
	TESUnknown_6D54CF8* scene = TESUnknown_6D54CF8::GetInstance();

	/*
	Don't draw fog without a scene...
	*/

	if (!scene || !scene->RenderInfo)
		return FALSE;

	/*
	The interior has its own fog.
	It has its own parameters, etc., in contrast to the location.
	The world draws the fog itself, depending on TimeOfDay.
	*/

	if (scene->IsInteriorsCell())
		return bFogToggle;

	/*
	You only need to draw fog when you turn on the sky.
	It does not have its own fog and its parameters, and the pointer to the parameters itself is NULL.
	*/
	
	return scene->RenderInfo->IsSky();
}

/*
==================
hk_call_F8AF16
==================
*/
VOID FIXAPI hk_call_F8AF16(const TESUnknown_6D54CF8::TESRenderInfo* RenderInfo) {
	if (TESUnknown_6D54CF8* scene = TESUnknown_6D54CF8::GetInstance(); scene->IsInteriorsCell()) {
		if (bFogToggle)
			return;
	}

	//This function resets the fog parameters and resets them again...
	fastCall<VOID, const TESUnknown_6D54CF8::TESRenderInfo*>(0xF8B6A0, RenderInfo);
}

/*
==================
PatchSky

Fixes the display of fog in the render.
The bFogEnabled option must be TRUE otherwise there will be a crash.
This option is set to TRUE by default, but you can override.
The patch sets its variable to bypass.
==================
*/
VOID FIXAPI PatchSky(VOID) {
	// Call our function and check the rax
	XUtil::DetourCall(OFFSET(0xF8CAF3, 0), &hk_call_F8CAF3);
	XUtil::PatchMemory(OFFSET(0xF8CAF8, 0), { 0x85, 0xC0 });
	XUtil::PatchMemoryNop(OFFSET(0xF8CAFA, 0), 4);
	XUtil::PatchMemory(OFFSET(0xF8CAFE, 0), { 0x75 });
	// Replace IsFogEnabled
	XUtil::DetourJump(OFFSET(0xF90CE0, 0), &hk_call_F8CAF3);
	// Update fog params
	XUtil::DetourCall(OFFSET(0xF8AF16, 0), &hk_call_F8AF16);
}

/*
==================
Fixed_IncorrectSmoothnessValueToMaterialNif

The program simply writes about an erroneous value and passes it into the calculation,
the function will now correct the value from 0.0 to 1.0
==================
*/
FLOAT FIXAPI Fixed_IncorrectSmoothnessValueToMaterialNif(FLOAT a1, FLOAT a2) {
	return (a2 > 1.0) ? 1.0 : ((a2 < 0.0) ? 0.0 : a2);
}

/*
==================
Fixed_DeleteTintingRace

The function removes from lists without looping
==================
*/
VOID FIXAPI Fixed_DeleteTintingRace(INT64 count, INT64 item_id, HWND listview) {
	if (!count)
		return;
	
	for (auto i = 0; i < count; i++)
		ListView_DeleteItem(listview, item_id);
}

/*
==================
hk_jmp_B62A9B

Enable/disable the button for generating .lip files. 
All .wav files must exist with set flag loaded Y(L).
==================
*/
VOID FIXAPI hk_jmp_B62A9B(HWND hWndButtonGenerate) {
	HWND hDlg = GetParent(hWndButtonGenerate);
	HWND hList = GetDlgItem(hDlg, 0x878);
	HWND hLTFCheck = GetDlgItem(hDlg, 0x94C);

	// no support .ltf
	EnableWindow(hLTFCheck, FALSE);
	// wav check default
	CheckDlgButton(hDlg, 0x94B, BST_CHECKED);

	BOOL bEnableGenerate = FALSE;
	
	if (INT iCount = ListView_GetItemCount(hList); iCount > 0) {
		if (auto item = (INT64)EditorUI::ListViewGetSelectedItem(hList); item) {
			bEnableGenerate =
				IsDlgButtonChecked(hDlg, 2379) && *(PUINT32)(item + 0x110) ||
				IsDlgButtonChecked(hDlg, 2380) && *(PUINT32)(item + 0x118);
		}
	}

	EnableWindow(hWndButtonGenerate, bEnableGenerate);
}

/*
==================
ExportFaceGenForSelectedNPCs
==================
*/
//VOID FIXAPI ExportFaceGenForSelectedNPCs(int64_t a1, int64_t a2) {
//	// Display confirmation message box first
//	if (!fastCall<BOOL>(0xAC2590))
//		return;
//
//	// In this memory area, the list handle is
//	HWND listHandle = *(HWND*)(a1 + 16);
//
//	auto sub_00007FF61C972F50 = (int64_t(*)(HWND, int64_t))OFFSET(0x562F50, 0);
//	auto sub_00007FF61CED25E0 = (BOOL(*)(int64_t, int64_t))OFFSET(0xAC25E0, 0);
//
//	// Go through all the selected elements and generate facegen
//	int32_t itemIndex = ListView_GetNextItem(listHandle, -1, LVNI_SELECTED);
//	int32_t itemCount = 0;
//	for (BOOL flag = TRUE; itemIndex >= 0 && flag; itemCount++) {
//		if (flag = sub_00007FF61CED25E0(a2, sub_00007FF61C972F50(listHandle, itemIndex)); flag) {
//			auto oldIndex = itemIndex;
//			if (itemIndex = ListView_GetNextItem(listHandle, itemIndex, LVNI_SELECTED); itemIndex == oldIndex)
//				itemIndex = -1;
//		}
//	}
//
//	// Reload loose file paths manually since it's patched out
//	_MESSAGE_FMT("Exported FaceGen for %d NPCs. Reloading loose file paths...", itemCount);
//	fastCall<VOID>(0x7B9E70, *(int64_t*)OFFSET(0x6D54CF8, 0));
//
//	// Done
//	fastCall<VOID>(0xAC2670);
//}
//
//sub_24EDC40_t oldsub_24EDC40;

//BOOL FIXAPI sub_24EDC40(LPVOID a1, INT64 a2) {
//	if (a1)
//		return oldsub_24EDC40(a1, a2);
//
//	return FALSE;
//}

/*
==================
PatchLip

The developers have cut out all the creation functionality .lip files. 
But this patch will make the button available.
==================
*/
VOID FIXAPI PatchLip(VOID) {
	// Hooking the jump. I don't allow the deny button.
	XUtil::DetourJump(OFFSET(0xB62A9B, 0), &hk_jmp_B62A9B);
	// Then continue in the same spirit, remove the button.... skip it
	XUtil::PatchMemoryNop(OFFSET(0xB5EEFB, 0), 25);
	// In case of cancellation .wav triggers and closes the button, we will remove everything
	XUtil::DetourCall(OFFSET(0xB5F182, 0), &hk_jmp_B62A9B);
}


/*
==================
PatchMessage

Most often, CK freezes when loading something large.
But otherwise it works quite well.
I limit the impact of the message processing patch by setting the permission flag.

I will give permission when loading with ProgressDialog enabled.
==================
*/
VOID FIXAPI PatchMessage(VOID) {
	if (bAllowPoolMessage)
		IProcessMessage::ProcessMessage(NULL, 0, 0, IProcessMessage::RemoveMessage | IProcessMessage::NoYield);
}

VOID FIXAPI PatchTemplatedFormIterator(VOID) {
	class FormIteratorHook : public Xbyak::CodeGenerator {
	public:
		FormIteratorHook(uintptr_t Callback) : Xbyak::CodeGenerator()
		{
			// Allocate enough space for 8 copied parameters (4 regs)
			mov(ptr[rsp + 0x8], rbx);
			mov(ptr[rsp + 0x10], rbp);
			mov(ptr[rsp + 0x18], rsi);
			push(rdi);
			sub(rsp, 0x50);

			mov(rbp, rcx);
			mov(rsi, rdx);
			mov(rdi, r8);
			mov(rbx, r9);
			mov(rax, (uintptr_t)&BeginUIDefer);// BeginUIDefer (0 params)
			call(rax);
			mov(rcx, rbp);
			mov(rdx, rsi);
			mov(r8, rdi);
			mov(r9, rbx);

			for (uint32_t i = 0; i < 6; i++)
			{
				mov(rax, ptr[rsp + (0xA8 - i * 0x8)]);
				mov(ptr[rsp + (0x48 - i * 0x8)], rax);
			}

			mov(rax, Callback);// Callback (X params)
			call(rax);
			mov(rbx, rax);

			mov(rax, (uintptr_t)&EndUIDefer);// EndUIDefer (0 params)
			call(rax);
			mov(rax, rbx);
			add(rsp, 0x50);
			pop(rdi);
			mov(rsi, ptr[rsp + 0x18]);
			mov(rbp, ptr[rsp + 0x10]);
			mov(rbx, ptr[rsp + 0x8]);
			ret();
		}

		static VOID Generate(uintptr_t Target) {
			// Manually resolve the called function address. NOTE: This is leaking memory on purpose. It's a mess.
			Assert(*(uint8_t *)Target == 0xE9);

			auto destination = Target + *(int32_t *)(Target + 1) + 5;
			auto hook = new FormIteratorHook(destination);

			XUtil::DetourJump(Target, (uintptr_t)hook->getCode());
		}
	};

	// List view
	FormIteratorHook::Generate(OFFSET(0x2450A, 0));
	FormIteratorHook::Generate(OFFSET(0x7C16A, 0));
	FormIteratorHook::Generate(OFFSET(0x37DF3, 0));
	FormIteratorHook::Generate(OFFSET(0x75324, 0));
	FormIteratorHook::Generate(OFFSET(0x65F55, 0));
	FormIteratorHook::Generate(OFFSET(0x2B21, 0));

	// Combo box
	FormIteratorHook::Generate(OFFSET(0x77E49, 0));
	FormIteratorHook::Generate(OFFSET(0x52F4, 0));
	FormIteratorHook::Generate(OFFSET(0x191D2, 0));
	FormIteratorHook::Generate(OFFSET(0x28CBD, 0));
	FormIteratorHook::Generate(OFFSET(0x118FB, 0));
}


/*
==================
PatchGeneratePreCombined_CmdLineWithQuote

Adds support for quotation marks of some commands on the command line
==================
*/

LPSTR FIXAPI Fixed_StrTok(LPSTR str, LPSTR delim, LPSTR* next_token) {
	if (str) {
		while (*str == ' ') {
			if (*str == '\0')
				return NULL;
			str++;
		}
		if (*str == '\"')
			return strtok_s(++str, "\"", next_token);
		else
			return strtok_s(str, " ", next_token);
	}
	else if (next_token && *next_token) {
		if (strchr(*next_token, '\"')) {
			LPSTR lpRes = strtok_s(NULL, "\"", next_token);
			
			if (lpRes && !XUtil::Str::trim(lpRes).length())
				lpRes = strtok_s(NULL, "\"", next_token);

			return lpRes;
		} 
		else
			return strtok_s(NULL, " ", next_token);
	}
	else 
		return strtok_s(str, delim, next_token);
};


VOID FIXAPI RestoreGenerateSingleLip(LPSTR lpCmdLine, LPSTR arg2) {
	namespace fs = std::filesystem;

	LPSTR next_token = NULL;

	LPSTR token = Fixed_StrTok(lpCmdLine, " ", &next_token);
	if (token) {
		LPSTR filename = token;
		token = Fixed_StrTok(NULL, " ", &next_token);
		if (token) {
			auto pathAudioFile = fs::path(filename);
			// Replacement by .wav
			pathAudioFile.replace_extension(L".wav");

			if (fs::exists(pathAudioFile)) {
				auto AudioFilePath = XUtil::Conversion::WideToAnsi(pathAudioFile);
				((BOOL(__fastcall*)(HWND, LPCSTR, LPCSTR))OFFSET(0x0B66BF0, 0))(MainWindow::GetWindow(), AudioFilePath.c_str(), token);

				return;
			}
		}
	}

	((VOID(__fastcall*)(LPCSTR))OFFSET(0x2001A90, 0))(((LPSTR)OFFSET(0x3837940, 0)));
}


HANDLE WINAPI hk_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
	return FindFirstFileExA(lpFileName, FindExInfoStandard, lpFindFileData, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
}


DWORD FIXAPI GetCountItemInLayer(LPVOID unknown, BGSLayer* layer) {
	if (!unknown || !layer)
		return 0;

	auto scene = TESUnknown_6D54CF8::GetInstance();

	if (scene->IsInteriorsCell())
		return layer->GetItemsCountInCell(scene->Interios);
	else
		return layer->GetItemsCountInCell(NULL);
}

DWORD WINAPI hk_modGetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString,
	DWORD nSize, LPCTSTR lpFileName) {
	if (!_stricmp(lpAppName, "General") && !_stricmp(lpKeyName, "sFile Path")) {
		auto str = BSString::Utils::GetApplicationPath();
		_MESSAGE_FMT("sFile Path: %s", *str);

		if (lpReturnedString)
			Assert(!strcpy_s(lpReturnedString, nSize, *str));

		return str.Length();
	}


	return GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

DWORD WINAPI hk_modGetPrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName) {
	// Check for overrides first
	for (auto& setting : CK_Settings) {
		if (!_stricmp(lpAppName, setting.Section) && !_stricmp(lpKeyName, setting.Key)) {
			LPCSTR start = bExtremelyMode ? setting.ExtremelyValue.c_str() : setting.Value.c_str();
			return (DWORD)strtoul(start, NULL, 10);
		}
	}
	
	return GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);
}

VOID FIXAPI EnabledExtremelyMode(VOID) {
	bExtremelyMode = TRUE;
}

VOID FIXAPI SkipAnimationTextExport(VOID) {
	Assert(!_stricmp(CK_Settings[35].Key, "bSkipAnimationTextExport"));
	CK_Settings[35].Value = "1";
	CK_Settings[35].ExtremelyValue = "1";
}

VOID FIXAPI OriginalLoadBA2(VOID) {
	Assert(!_stricmp(CK_Settings[30].Key, "bAutoloadTESFileArchives"));
	CK_Settings[30].Value = "1";
	CK_Settings[30].ExtremelyValue = "1";
}

VOID FIXAPI hk_call_142D12196(VOID) {
	AssertMsg(FALSE, "Creation Kit renderer initialization failed because your graphics card doesn't support D3D11 Feature Level 11 (FL11_0). Updating your drivers may fix this.");
}

//VOID FIXAPI hk_FastAnimationBuild(LPVOID rcx, LPVOID rdx, LPVOID r8) {
//	PULONGLONG* beginAddr = (PULONGLONG*)rdx;
//	PULONGLONG* endAddr = (PULONGLONG*)((DWORD_PTR)rdx + ((DWORD_PTR)rcx * 8));
//
//	//std::async(std::launch::async, exec, ping.c_str());
//
//	for (; beginAddr != endAddr; ++beginAddr) {
//		if (!(*beginAddr))
//			break;
//
//		BOOL bResult = FALSE;
//		std::thread th([&beginAddr, &r8, &bResult]() {
//			bResult = (BOOL)fastCall<LPVOID>(0xFBE20, r8, *beginAddr);
//		});
//
//		th.join();
//
//		if (!bResult)
//			break;
//	}
//}

//VOID FIXAPI hk_sub_6450B0(HWND ControlHandle) {
	//auto sub_644CC0 = (void(*)(HWND, uint8_t, bool, __int64, __int64, __int64, __int64, __int64))OFFSET(0x644CC0, 0);

	//sub_644CC0(ControlHandle, 18, true, 0, 0, 0, 0, 0);	// MGEF Magic Effect
	//sub_644CC0(ControlHandle, 21, false, 0, 0, 0, 0, 0);	// ENCH Enchantment
//}

VOID FIXAPI PatchCmdLineWithQuote(VOID) {
	//	Add support quote to command line with -GeneratePreCombined
	//	Should be: -GeneratePreCombined:"<ESMFilename>" [clean, filtered] [all, other, main, ints]

	XUtil::DetourCall(OFFSET(0x33B9D3, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B9F3, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33BA0C, 0), &Fixed_StrTok);

	//	-GeneratePreVisData
	//	This command into the code section -GeneratePreCombined.
	//	Should be: -GeneratePreVisData:"<ESMFilename>" [clean, filtered] [all, other, main, ints]

	XUtil::DetourCall(OFFSET(0x33BB0E, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33BB2C, 0), &Fixed_StrTok);

	//	Add support quote to command line with -CheckInPlugin
	//	Should be: -CheckInPlugin:"<PluginFilename>" "<ESMFilename>"

	XUtil::DetourCall(OFFSET(0x33B30B, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B32D, 0), &Fixed_StrTok);

	//	Add support quote to command line with -ConvertToESL
	//	Should be: -ConvertToESL:"<PluginFilename>"

	XUtil::DetourCall(OFFSET(0x33C4F4, 0), &Fixed_StrTok);

	//	Add support quote to command line with -DumpNeededFiles
	//	Should be: -DumpNeededFiles:"<ESMFilename>" "<OutputFilepath>"

	XUtil::DetourCall(OFFSET(0x33B43E, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B45C, 0), &Fixed_StrTok);

	//	Add support quote to command line with -SaveDefaultPlugin
	//	Should be: -SaveDefaultPlugin:"<PluginFilename>" "<ESMFilename>"
	
	XUtil::DetourCall(OFFSET(0x33B278, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B296, 0), &Fixed_StrTok);
	
	//	Add support quote to command line with -SaveDefaultPlugin
	//	Should be: -ExportDismemberData:"<ESMFilename>" <XB1|X64|PS4|W32>

	XUtil::DetourCall(OFFSET(0x33B1A4, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B1C2, 0), &Fixed_StrTok);
	
	//	Add support quote to command line with -UpdateModelData
	//	Should be: -UpdateModelData:"<ESMFilename>"

	XUtil::DetourCall(OFFSET(0x33B139, 0), &Fixed_StrTok);

	//	Add support quote to command line with -OutputAreaArt
	//	Should be: -OutputAreaArt:"<ESMFilename>" "<AreasFilename>" "<OutputFilename>"

	XUtil::DetourCall(OFFSET(0x33B07F, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B09D, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B0BB, 0), &Fixed_StrTok);

	//	Add support quote to command line with -CompileTextExport
	//	Should be: -CompileTextExport:"<ESMFilename>" "<language>" "<PathToTextExport>" ["<PathBackupToTextExport>"]

	XUtil::DetourCall(OFFSET(0x33AF37, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33AF55, 0), &Fixed_StrTok);

	//	Add support quote to command line with -ExportFaceGenData
	//	Should be: -ExportFaceGenData:"<ESMFilename>" <XB1|X64|PS4|W32>

	XUtil::DetourCall(OFFSET(0x33ADD1, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33ADEF, 0), &Fixed_StrTok);

	//	Add support quote to command line with -GenerateAnimInfo
	//	Should be: -GenerateAnimInfo:"<ESMFilename>" "<DataFilepath>" "<OutputFilepath>" [%s] [%s] [%s]
	
	XUtil::DetourCall(OFFSET(0x33B4DA, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B4F8, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B516, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B534, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B694, 0), &Fixed_StrTok);
	
	//	Add support quote to command line with -GenerateSingleLip
	//	Should be: -GenerateSingleLip:"<WavFilename>" "<Text>"
	//	Warning: /Data/Sound/ this is the directory relative to which it will be stored .lip file.

	XUtil::DetourCall(OFFSET(0x33B843, 0), &RestoreGenerateSingleLip);

	//	Add support quote to command line with -GenerateStaticCollections
	//	Should be: -GenerateStaticCollections:"<ESMFilename>" <XB1|X64|PS4|W32>

	XUtil::DetourCall(OFFSET(0x33B90E, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33B92C, 0), &Fixed_StrTok);

	//	Add support quote to command line with -DepersistRefs
	//	Should be: -DepersistRefs:"<ESMFilename>"

	XUtil::DetourCall(OFFSET(0x33BE82, 0), &Fixed_StrTok);

	//	Add support quote to command line with -MapMarker
	//	Should be: -MapMarker:"<ESMFilename>" "<Worldspace|Interior>"

	XUtil::DetourCall(OFFSET(0x33BEFE, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33BF1C, 0), &Fixed_StrTok);

	//	Add support quote to command line with -MapInfo
	//	Should be: -MapInfo:"<ESMFilename>" "<Worldspace|Interior>" ["<%s>"]

	XUtil::DetourCall(OFFSET(0x33BF87, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33BFA5, 0), &Fixed_StrTok);
	XUtil::DetourCall(OFFSET(0x33BFC3, 0), &Fixed_StrTok);
	
	//	Add support quote to command line with -ImportScalingData
	//	Should be: -ImportScalingData:"<ESMFilename>"

	XUtil::DetourCall(OFFSET(0x33C10D, 0), &Fixed_StrTok);

	//	Add support quote to command line with -CompressPSG
	//	Should be: -CompressPSG:"<ESMFilename>"

	XUtil::DetourCall(OFFSET(0x33C178, 0), &Fixed_StrTok);

	//	Add support quote to command line with -BuildCDX
	//	Should be: -BuildCDX:"<ESMFilename>"

	XUtil::DetourCall(OFFSET(0x33C304, 0), &Fixed_StrTok);
}

#pragma warning(default : 26819; default : 26451; default : 6387; default : 6001)