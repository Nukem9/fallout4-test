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

#include "../common.h"
#include <libdeflate/libdeflate.h>
#include <intrin.h>
#include "../tools/INIHookInputToFile.h"
#include "TES/MemoryManager.h"
#include "TES/bhkThreadMemorySource.h"

#include "CKF4/ExperimentalNuukem.h"
#include "CKF4/TranslateUnicode_CK.h"
#include "CKF4/UIProgressDialog.h"

// include patches for editor
#include "CKF4/Editor.h"
#include "CKF4/EditorUI.h"
#include "CKF4/UIThemeMode.h"
#include "CKF4/EditorUIProgressDialog.h"

// include windows
#include "CKF4/LogWindow.h"
#include "CKF4/CellViewWindow.h"
#include "CKF4/ObjectWindow.h"
#include "CKF4/RenderWindow.h"
#include "CKF4/MainWindow.h"
#include "CKF4/ResponseWindow.h"
#include "CKF4/DataWindow.h"
#include "CKF4/PreferencesWindow.h"

// include json generator dialogs
#include "CKF4/UIDialogManager.h"

#include <xbyak/xbyak.h>

/*

This file is part of Fallout 4 Fixes source code.

*/

VOID FIXAPI Fix_PatchMemory(VOID);
VOID FIXAPI Fix_PatchThreading(VOID);
VOID FIXAPI Fix_GenerateCrashdumps(VOID);
size_t FIXAPI BNetConvertUnicodeString(char* Destination, size_t DestSize, const wchar_t* Source, size_t SourceSize);

namespace Classes = Core::Classes::UI;

static INT32 nCountArgCmdLine = 0;

/*
==================
F_RequiredPatches

Patches that are installed by default
==================
*/
VOID FIXAPI F_RequiredPatches(VOID) {
	//
	// MemoryManager
	//
	Fix_PatchMemory();

	XUtil::PatchMemory(OFFSET(0x030ECC0, 0), { 0xC3 });							// [XGB  ] MemoryManager - Default/Static/File heaps
	XUtil::PatchMemory(OFFSET(0x2004B70, 0), { 0xC3 });							// [1GB  ] BSSmallBlockAllocator
	XUtil::DetourJump(OFFSET(0x21115D0, 0), &bhkThreadMemorySource::__ctor__);	// [512MB][1GB  ][2GB  ] bhkThreadMemorySource
	XUtil::PatchMemory(OFFSET(0x200A920, 0), { 0xC3 });							// [32MB ][64MB ][128MB] ScrapHeap init
	XUtil::PatchMemory(OFFSET(0x200B440, 0), { 0xC3 });							// [32MB ][64MB ][128MB] ScrapHeap deinit
																				// [128MB] BSScaleformSysMemMapper is untouched due to complexity

	XUtil::PatchMemory(OFFSET(0x21179B8, 0), { (BYTE)(((UINT32)g_bhkMemSize & 0xFF000000) >> 24) });

	XUtil::DetourJump(OFFSET(0x2004E20, 0), &MemoryManager::Allocate);
	XUtil::DetourJump(OFFSET(0x20052D0, 0), &MemoryManager::Deallocate);
	XUtil::DetourJump(OFFSET(0x2004300, 0), &MemoryManager::Size);
	XUtil::DetourJump(OFFSET(0x200AB30, 0), &ScrapHeap::Allocate);
	XUtil::DetourJump(OFFSET(0x200B170, 0), &ScrapHeap::Deallocate);
	
	PatchIAT(hk_CreateDialogParamA, "USER32.DLL", "CreateDialogParamA");
	PatchIAT(hk_DialogBoxParamA, "USER32.DLL", "DialogBoxParamA");
	PatchIAT(hk_EndDialog, "USER32.DLL", "EndDialog");
	PatchIAT(hk_SendMessageA, "USER32.DLL", "SendMessageA");

	//
	// Threads
	//
	Fix_PatchThreading();

	//
	// Steam API
	//
	XUtil::PatchMemory(OFFSET(0x2881F84, 0), { 0x74, 0x1B, 0x90, 0x90, 0x90, 0x90 });
	XUtil::PatchMemory(OFFSET(0x2881FB4, 0), { 0x74, 0x26, 0x90, 0x90, 0x90, 0x90 });
	XUtil::PatchMemory(OFFSET(0x2881FF4, 0), { 0x74, 0x1D, 0x90, 0x90, 0x90, 0x90 });

	TESDataFileHandler_CK::Initialize();

	// Getting a pointer to TESDataFileHandler_CK. (no actual)
	// And when the ReplacingTipsWithProgressBar option is enabled, the dialog starts.
	XUtil::DetourCall(OFFSET(0x5BE5A6, 0), &EditorUI::hk_CallLoadFile);
	// Foreground CellView and Object Windows after loading.
	// And when the ReplacingTipsWithProgressBar option is enabled, closing the dialog box.
	XUtil::DetourJump(OFFSET(0x7DA80D, 0), &EditorUI::hk_EndLoadFile);

	// Ignore bAllowMultipleMasterLoads
	XUtil::PatchMemory(OFFSET(0x7D9F93, 0), { 0xEB, 0x40 });
	// Ignore bAllowMultipleEditors
	XUtil::PatchMemory(OFFSET(0x5B5A53, 0), { 0xEB });

	// Disable useless "Processing Topic X..." status bar updates
	XUtil::PatchMemoryNop(OFFSET(0xB89897, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0xB89BF3, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0xB8A472, 0), 5);

	// Fixes sky and fog
	PatchSky();

	// no support cmd line
	if (nCountArgCmdLine == 1) {
		//
		// Processing messages during file upload so that the window doesn't hang
		//

		// jump to function for useful work (messages pool)
		XUtil::DetourJump(OFFSET(0x2485C46, 0), &PatchMessage);
		XUtil::DetourJump(OFFSET(0x2485E46, 0), &PatchMessage);
		XUtil::DetourJump(OFFSET(0xDF2FBA, 0), &PatchMessage);
		XUtil::DetourJump(OFFSET(0x8531BD, 0), &PatchMessage);
		XUtil::DetourJump(OFFSET(0x262D1A7, 0), &PatchMessage);
		// Replacing Sleep(1) on (messages pool)
		XUtil::DetourCall(OFFSET(0x247EF69, 0), &PatchMessage);
		XUtil::DetourCall(OFFSET(0x5DD8C2, 0), &PatchMessage);
		XUtil::DetourCall(OFFSET(0x5DD7F2, 0), &PatchMessage);
		// Replacing a completely empty function with something useful (messages pool)
		XUtil::DetourCall(OFFSET(0x7E34D2, 0), &PatchMessage);
		XUtil::DetourCall(OFFSET(0x773DEE, 0), &PatchMessage);
	}

	//
	// Kill broken destructors causing crashes on exit
	//
	XUtil::DetourCall(OFFSET(0x5B6DF7, 0), &QuitHandler);
	XUtil::DetourCall(OFFSET(0x2D48FC0, 0), &QuitHandler);
	XUtil::DetourCall(OFFSET(0x2D48FCF, 0), &QuitHandler);
	
#if FALLOUT4_STUDY_CK64_INIFILE
	//
	// For study ini
	//
	Tools::IniHookInputInit();
#endif // !FALLOUT4_STUDY_CK64_INIFILE

	//
	// Fixed failed load d3dcompiler.dll
	//
	XUtil::PatchMemoryNop(OFFSET(0x2A46AC0, 0), 6);

	//
	// Fixed when the value is different from 0.0 to 1.0. Smoothness value to material (nif)
	//
	XUtil::DetourCall(OFFSET(0x2B7F5B7, 0), &Fixed_IncorrectSmoothnessValueToMaterialNif);
	XUtil::PatchMemory(OFFSET(0x2B7F5BC, 0), { 0x66, 0x0F, 0x7E, 0x85, 0x88, 0x00, 0x00, 0x00, 0xEB, 0x18 });

	//
	// Fixed a very harmful error that pops up very rarely
	// 
	XUtil::DetourCall(OFFSET(0x2511176, 0), &hk_call_2511176);

	//
	// Fixed infinite loop by Compile Papyrus Scripts...
	//
	XUtil::PatchMemory(OFFSET(0x12E5E50, 0), { 0xC3 });
	
	//
	// Fixed when you delete a group tinting to race window
	//
	XUtil::PatchMemory(OFFSET(0x963E2E, 0), { 0x4D, 0x8B, 0x47, 0x8, 0x4C, 0x89, 0xE2 });
	XUtil::DetourCall(OFFSET(0x963E35, 0), &Fixed_DeleteTintingRace);
	XUtil::PatchMemory(OFFSET(0x963E3A, 0), { 0xEB, 0x18 });

	//
	// Fixed crash when by load plugin in which there is no root parent to materials
	//
	XUtil::PatchMemory(OFFSET(0x2500E7C, 0), { 0x0F, 0xC6, 0x1D, 0x1C, 0xC0, 0x30, 0x01, 0xEE, 0x0F, 0xC6, 0x05, 0x14,
		0xC0, 0x30, 0x01, 0x44, 0x31, 0xC0, 0xEB, 0x4B });
	XUtil::PatchMemory(OFFSET(0x8ED2AF, 0), { 0xEB, 0x6F });
	XUtil::PatchMemory(OFFSET(0x2AC1D07, 0), { 0xEB });

	//
	// Enable the render window "Go to selection in game" hotkey even if version control is off
	//
	XUtil::PatchMemoryNop(OFFSET(0x472123, 0), 2);

	//
	// Fix for crash when using the -MapMaker command line option. Nullptr camera passed to BSGraphics::State::SetCameraData.
	//
	XUtil::DetourCall(OFFSET(0x0906407, 0), &hk_call_140906407);

	//
	// Fix for the -GeneratePreCombined command line option creating files for the PS4 (2) format. It should be WIN64 (0).
	//
	XUtil::PatchMemory(OFFSET(0x0DCB7DB, 0), { 0x00, 0x00, 0x00, 0x00 });

	//
	// Fix for crash when tab control buttons are deleted. Uninitialized TCITEMA structure variables.
	//
	XUtil::DetourJump(OFFSET(0x0564E30, 0), &EditorUI::TabControlDeleteItem);

	//
	// Fix for crash (recursive sorting function stack overflow) when saving certain ESP files (i.e SimSettlements.esp)
	//
	XUtil::DetourJump(OFFSET(0x07ED840, 0), &ArrayQuickSortRecursive<class TESForm_CK*>);
	XUtil::PatchMemory(OFFSET(0x07EDA50, 0), { 0xC3 });
	
	//
	// Raise the papyrus script editor text limit to 500k characters from 64k
	//
	XUtil::DetourCall(OFFSET(0x12E852C, 0), &hk_call_12E852C);

	//
	// Fix for crash (nullptr no test) when close CK with Sky enable 
	//
	XUtil::PatchMemory(OFFSET(0xF84521, 0), { 0xEB, 0x4D, 0x90 });
	XUtil::PatchMemory(OFFSET(0xF84570, 0), { 0x48, 0x85, 0xC9, 0x74, 0xB5, 0x48, 0x8B, 0x01, 0xEB, 0xAA });

	//
	// Adds support for quotation marks of some commands on the command line
	//
	PatchCmdLineWithQuote();

	//
	// Skipping the program update check
	//
	XUtil::PatchMemory(OFFSET(0x5C180B, 0), { 0xEB, 0x10 });
	
	//
	// Fix crash when Unicode string conversion fails with bethesda.net http responses
	//
	XUtil::DetourJump(OFFSET(0x27DAF60, 0), &BNetConvertUnicodeString);

	//
	// Change the default " 64-bit"
	//
	const char* newTitlePart = " Fallout 4 64-bit";
	XUtil::PatchMemory(OFFSET(0x5C16CB, 0), { 0x8B });
	XUtil::PatchMemory(OFFSET(0x3857828, 0), (uint8_t*)&newTitlePart, sizeof(newTitlePart));

#if !FALLOUT4_STUDY_CK64_INIFILE
	//
	// Skip option bSkipValidateForms and bDisableDuplicateReferenceCheck
	//
	PatchIAT(hk_modGetPrivateProfileIntA, "kernel32.dll", "GetPrivateProfileIntA");
#endif // !FALLOUT4_STUDY_CK64_INIFILE
	
	//
	// Plugin loading optimizations
	//

	INT32 cpuinfo[4];
	__cpuid(cpuinfo, 1);
	bool sse41 = (cpuinfo[2] & (1 << 19)) != 0;

	if (!g_INI->GetBoolean("CreationKit", "BSTArraySearchItemReplacement", FALSE)) {
		// Utilize SSE4.1 instructions if available
		if (sse41) {
			_MESSAGE("Utilize SSE4.1 instructions if available and loading optimizations enabled");
			XUtil::DetourJump(OFFSET(0x05B31C0, 0), &Experimental::BSTArraySIMD2SearchItem);
		}
		else
			XUtil::DetourJump(OFFSET(0x05B31C0, 0), &Experimental::BSTArraySearchItem);
	}
	else {
		// Search for a set of functions for accessing an array and getting an index.
		// Imho, there are a lot of functions and one and the same,
		// I don't even know what to say about this, there is another function with the third parameter, I assume this offset from.

		std::vector<uintptr_t> matches = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase,
			"48 89 5C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 30 44 8B 71 10 83 CB FF 33 FF 4C 8B FA 48 8B F1 45 85 F6");

		std::vector<uintptr_t> matches2 = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase,
			"48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 30 44 8B 71 10 83 CB FF 41 8B F8");
		
		//
		// Removal from the change, cause the loading of the quest diagram to hang
		//
		matches.erase(matches.begin() + 89); // 0xA14500
		matches.erase(matches.begin() + 53); // 0x525A50
		matches.erase(matches.begin() + 7);  // 0x193D20

		auto search_array_func = [](auto it) { XUtil::DetourJump(it, &Experimental::BSTArraySearchItem); };
		auto search_array_func_2 = [](auto it) { XUtil::DetourJump(it, &Experimental::BSTArraySearchItemWithOffset); };
		auto search_array_func_3 = [](auto it) { XUtil::DetourJump(it, &Experimental::BSTArraySIMD2SearchItem); };
		auto search_array_func_3_2 = [](auto it) { XUtil::DetourJump(it, &Experimental::BSTArraySIMD2SearchItemWithOffset); };
		std::vector<uintptr_t>::iterator match;

		// Utilize SSE4.1 instructions if available
		if (sse41) {
			_MESSAGE("Utilize SSE4.1 instructions if available and loading optimizations enabled");
			XUtil::Parallel::for_each(match = matches.begin(), matches.end(), search_array_func_3);
			XUtil::Parallel::for_each(match = matches2.begin(), matches2.end(), search_array_func_3_2);
		}
		else {
			XUtil::Parallel::for_each(match = matches.begin(), matches.end(), search_array_func);
			XUtil::Parallel::for_each(match = matches2.begin(), matches2.end(), search_array_func_2);
		}
	}

	XUtil::DetourCall(OFFSET(0x08056B7, 0), &hk_inflateInit);
	XUtil::DetourCall(OFFSET(0x08056F7, 0), &hk_inflate);
	PatchIAT(hk_FindFirstFileA, "kernel32.dll", "FindFirstFileA");
}


/*
==================
F_UIPatches

Patches for the UI
==================
*/
VOID FIXAPI F_UIPatches(VOID) {
	if (!g_UIEnabled)
		return;

	auto comDll = (uintptr_t)GetModuleHandleA("comctl32.dll");
	Assert(comDll);
	
	if (INT theme_id = (INT)g_INI->GetInteger("CreationKit", "UITheme", 0); theme_id > 0) {
		// Setting the colors
		UITheme::Initialize((UITheme::Theme::Theme)theme_id);

		Detours::IATHook(comDll, "USER32.dll", "GetSysColor", (uintptr_t)&UITheme::Comctl32GetSysColor);
		Detours::IATHook(comDll, "USER32.dll", "GetSysColorBrush", (uintptr_t)&UITheme::Comctl32GetSysColorBrush);
		Detours::IATDelayedHook(comDll, "UxTheme.dll", "DrawThemeBackground", (uintptr_t)&UITheme::Comctl32DrawThemeBackground);
		Detours::IATDelayedHook(comDll, "UxTheme.dll", "DrawThemeText", (uintptr_t)&UITheme::Comctl32DrawThemeText);

		// replace main toolbar
		XUtil::DetourCall(OFFSET(0x5FE166, 0), UITheme::Comctl32CreateToolbarEx_1);
		XUtil::DetourJump(OFFSET(0x5FE401, 0), UITheme::HideOldTimeOfDayComponents);
		// replace ImageList_LoadImage for item type
		XUtil::DetourCall(OFFSET(0x5B63E7, 0), UITheme::Comctl32ImageList_LoadImageA_1);
		// Sync TimeOfDay set value (from Preferences dialogs)
		XUtil::DetourCall(OFFSET(0x5ED96A, 0), PreferencesWindow::hk_SetInPreferencesToTimeOfDay);

		// Initializing the dialog manager. 
		// Loading all supported dialogs.
		g_DialogManager = new Classes::CDialogManager();
		if (g_DialogManager && g_DialogManager->Empty()) {
			_MESSAGE("DIALOG: Failed initialization DialogManager or no dialogs detected");
			delete g_DialogManager;
			g_DialogManager = NULL;
		}
		else {
			// Let's increase the "Filename" column in the Data dialog.
			XUtil::PatchMemory(OFFSET(0x5A520A, 0), { 0x2C, 0x01 });
		}
	}

	EditorUI::Initialize();

	*(uintptr_t*)&MainWindow::OldWndProc = Detours::X64::DetourFunctionClass(OFFSET(0x05B74D0, 0), &MainWindow::WndProc);
	*(uintptr_t*)&ObjectWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x03F9020, 0), &ObjectWindow::DlgProc);
	*(uintptr_t*)&CellViewWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x059D820, 0), &CellViewWindow::DlgProc);
	*(uintptr_t*)&ResponseWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x0B5EB50, 0), &ResponseWindow::DlgProc);
	*(uintptr_t*)&RenderWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x460570, 0), &RenderWindow::DlgProc);
	*(uintptr_t*)&DataWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x5A8250, 0), &DataWindow::DlgProc);

	if (UITheme::IsEnabledMode()) {
		*(uintptr_t*)&PreferencesWindow::OldDlgProc = OFFSET(0x1335AF0, 0);
		XUtil::DetourCall(OFFSET(0x1336521, 0), &PreferencesWindow::CreateDialogParamA);
	}

	// CheckMenuItem is called, however, it always gets zero, but eight is written on top, which is equal to MFS_CHECKED.
	XUtil::PatchMemoryNop(OFFSET(0x5B820D, 0), 6);
	// CheckMenuItem is called, however, it always gets MFS_CHECKED.
	XUtil::PatchMemoryNop(OFFSET(0x43E3A3, 0), 6);
	// Fix repeat CheckMenuItem is called
	XUtil::PatchMemory(OFFSET(0x5B84C1, 0), { 0x20, 0x63 });

	XUtil::PatchMemoryNop(OFFSET(0x2A4D45C, 0), 5);// Disable "Out of Pixel Shaders (running total: X)" log spam
	XUtil::PatchMemoryNop(OFFSET(0x2A476B6, 0), 5);// Disable "Out of UCode space" log spam
	XUtil::PatchMemoryNop(OFFSET(0x2B41723, 0), 5);// Disable "BSMeshCombiner" log spam
	XUtil::PatchMemoryNop(OFFSET(0x2B41787, 0), 5);// Disable "BSMeshCombiner" log spam

	XUtil::DetourJump(OFFSET(0x2001970, 0), &LogWindow::LogWarningVa);
	XUtil::DetourJump(OFFSET(0x2001EA0, 0), &LogWindow::LogWarningUnknown1);
	XUtil::DetourJump(OFFSET(0x2001CB0, 0), &LogWindow::LogWarningUnknown1);
	XUtil::DetourJump(OFFSET(0x2001D60, 0), &LogWindow::LogWarningUnknown1);
	XUtil::DetourJump(OFFSET(0x2001F60, 0), &LogWindow::LogWarningUnknown1);
	XUtil::DetourCall(OFFSET(0x07DA533, 0), &LogWindow::LogWarningUnknown2);
	XUtil::DetourJump(OFFSET(0x2001BC0, 0), &LogWindow::LogAssert);

	// Deferred dialog loading (batched UI updates)
	PatchTemplatedFormIterator();
	XUtil::DetourJump(OFFSET(0x05622B0, 0), &InsertComboBoxItem);
	XUtil::DetourJump(OFFSET(0x0562BC0, 0), &InsertListViewItem);
	XUtil::DetourCall(OFFSET(0x03FE701, 0), &UpdateObjectWindowTreeView);
	XUtil::DetourCall(OFFSET(0x05A05FB, 0), &UpdateCellViewCellList);
	XUtil::DetourCall(OFFSET(0x05A1B0A, 0), &UpdateCellViewObjectList);

	// Fix resize ObjectWindowProc
	XUtil::DetourCall(OFFSET(0x5669D2, 0), &ObjectWindow::hk_MoveWindow);
	XUtil::PatchMemoryNop(OFFSET(0x5669D7, 0), 0x46);

	// Allow forms to be filtered in ObjectWindowProc
	XUtil::DetourCall(OFFSET(0x3FE4CA, 0), &ObjectWindow::hk_7FF72F57F8F0);
	// Allow forms to be filtered in CellViewProc
	XUtil::DetourCall(OFFSET(0x6435BF, 0), &CellViewWindow::hk_7FF70C322BC0);
	XUtil::DetourCall(OFFSET(0xF3C311, 0), &CellViewWindow::hk_7FF70C322BC0);
	// Allow objects to be filtered in CellViewProc
	XUtil::DetourCall(OFFSET(0x5A43B5, 0), &CellViewWindow::hk_call_5A43B5);

	// Cancel resize World Space combo
	XUtil::PatchMemoryNop(OFFSET(0x59DA1D, 0), 5);

	//
	// Version Control fixes
	//

	XUtil::DetourCall(OFFSET(0x13339D6, 0), &LogWindow::LogInsteadOfMsgBox);
	// By disabling version control, allow the start
	XUtil::PatchMemory(OFFSET(0x5C3C4F, 0), { 0xEB, 0x81 });	// skip msgbox 
	XUtil::PatchMemory(OFFSET(0x5C3C74, 0), { 0xEB, 0xD9 });	// skip msgbox 
	
	// Cut dialog Save
	XUtil::DetourCall(OFFSET(0x611F3A, 0), &GetMasterFileNameByVersionControlEnabled);
	
	// Fixes lip generate
	PatchLip();

	//
	// Since I'm used to seeing SSE fixes
	//

	// Setting section sizes statusbar
	XUtil::DetourCall(OFFSET(0x5FDFC8, 0), &EditorUI::hk_SetSettingsPartStatusBar);
	// Spam in the status bar no more than 1 second
	XUtil::DetourCall(OFFSET(0x45E287, 0), &EditorUI::hk_SpamFPSToStatusBar);
	// Send text to 4 part StatusBar (Game cam: .....)
	XUtil::PatchMemory(OFFSET(0x45EB2A, 0), { 0x03 });

	// Close the progress dialog 
	// PS: Show markers (Light Markers render always after loading)
	XUtil::DetourJump(OFFSET(0x460239, 0), &EditorUI::hk_EndSendFromCellViewToRender);

	//
	// Replacing the Tips window "Do you know...". Which appears when the plugin is loaded. (no support cmd line)
	//

	if (BOOL enable = (BOOL)g_INI->GetBoolean("CreationKit", "ReplacingTipsWithProgressBar", FALSE); enable) {
		if (nCountArgCmdLine == 1) {
			XUtil::PatchMemory(OFFSET(0x392260, 0), { 0xC3 });
			XUtil::PatchMemory(OFFSET(0x3923C3, 0), { 0xC3 });
			XUtil::PatchMemoryNop(OFFSET(0x5BE590, 0), 5);

			XUtil::PatchMemory(OFFSET(0x7DEA53, 0), { 0x4C, 0x89, 0xD9 });
			XUtil::DetourCall(OFFSET(0x7DEA56, 0), &EditorUI::hk_StepItProgress);
			XUtil::PatchMemory(OFFSET(0x7DEA5B, 0), { 0xEB, 0x05 });

			// Load Files... Initializing...
			XUtil::DetourClassCall(OFFSET(0x7E2FF6, 0), &TESDataFileHandler_CK::InitUnknownDataSetTextStatusBar);
			// During the entire process, the update is only 95 times for each percentage.... very little, get in here for an update
			XUtil::DetourCall(OFFSET(0x7DEA67, 0), &EditorUI::hk_UpdateProgress);
			// Load Files... Done... etc.
			XUtil::DetourCall(OFFSET(0x7E34FD, 0), &EditorUI::hk_SetTextAndSendStatusBar);
			XUtil::DetourCall(OFFSET(0x7DC390, 0), &EditorUI::hk_SetTextAndSendStatusBar);

			// Run the progress dialog when loading the interior in the render.
			XUtil::DetourCall(OFFSET(0x59F6B9, 0), &EditorUI::hk_SendFromCellViewToRender);

			// The terrain patch freezes, so a queue is created taking into account the activity of the dialog
			XUtil::DetourClassCall(OFFSET(0x2629D96, 0), &Classes::CUIProgressDialog::ProcessMessages);
			XUtil::DetourClassCall(OFFSET(0x262A6B2, 0), &Classes::CUIProgressDialog::ProcessMessages);
			XUtil::DetourClassCall(OFFSET(0x262A6BF, 0), &Classes::CUIProgressDialog::ProcessMessages);

			// Experimental: Loading cell ... (need for progressbar drawing)
			XUtil::DetourClassJump(OFFSET(0xE1D2C7, 0), &Classes::CUIProgressDialog::ProcessMessagesOnlyLoadCellWorld);
		}
	}
}


/*
==================
F_UnicodePatches

Patches for multilingual support
==================
*/
VOID FIXAPI F_UnicodePatches(VOID) {
	//
	// Convert Utf-8 to WinCP when loading and back when saving
	//
	if (g_INI->GetBoolean("CreationKit", "Unicode", FALSE))
	{
#if FALLOUT4_LAZ_UNICODE_PLUGIN
		// Initialization CreationKitUnicodePlugin.dll
		BOOL bRes = XUtil::Conversion::LazUnicodePluginInit();
		if (!bRes)
		{
			_MESSAGE("Library 'CreationKitUnicodePlugin.dll' no found. Unicode support don't patched.");
			goto label_skip_msg_closeall_dialog;
		}
		else
		{
			// Also delete it message "You must close all Dialoge Boxes",
			// which has problems with programs that work with multiple monitors.
			XUtil::DetourCall(OFFSET(0x5C4470, 0), &Experimental::BeginPluginSave);
			// I don't quite understand the meaning of calling SetCursor in this function, which deals with saving
			// But we'll make the call in hook.
			XUtil::DetourCall(OFFSET(0x5C4511, 0), &Experimental::EndPluginSave);

			// Introduction of string processing.
			XUtil::PatchMemory(OFFSET(0x247D717, 0), { 0x22 });
			XUtil::PatchMemory(OFFSET(0x247D72F, 0), { 0x51, 0x48, 0x89, 0xC1 });
			XUtil::DetourCall(OFFSET(0x247D733, 0), &Experimental::Translate);
			XUtil::PatchMemory(OFFSET(0x247D738, 0), { 0x59, 0xC3, 0x31, 0xC0, 0xC3 });
			XUtil::PatchMemory(OFFSET(0x2481DF7, 0), { 0xE9, 0x33, 0xB9, 0xFF, 0xFF });

			// In the "Data" dialog box, the "author" and "description" controls are independent, and I'm forced to make a trap for WinAPI calls
			XUtil::DetourClassCall(OFFSET(0x5A8C81, 0), &Experimental::hk_SetDlgItemTextA);
			XUtil::DetourClassCall(OFFSET(0x5A8CA0, 0), &Experimental::hk_SetDlgItemTextA);
			XUtil::DetourClassCall(OFFSET(0x5A8A95, 0), &Experimental::hk_SendDlgItemMessageA);
			XUtil::DetourClassCall(OFFSET(0x5A8ACD, 0), &Experimental::hk_SendDlgItemMessageA);
			XUtil::DetourClassCall(OFFSET(0x5A8B07, 0), &Experimental::hk_SendDlgItemMessageA);
			XUtil::DetourClassCall(OFFSET(0x5A8B3F, 0), &Experimental::hk_SendDlgItemMessageA);

			//
			// Cut check spelling window
			//

			XUtil::PatchMemoryNop(OFFSET(0x84D7EE, 0), 5);
			XUtil::PatchMemoryNop(OFFSET(0xA55B7E, 0), 5);
			XUtil::PatchMemoryNop(OFFSET(0xB08405, 0), 5);
			XUtil::PatchMemoryNop(OFFSET(0xB5F59F, 0), 5);
			XUtil::PatchMemoryNop(OFFSET(0xB5FECF, 0), 5);
			XUtil::PatchMemoryNop(OFFSET(0xB600B9, 0), 5);
			XUtil::PatchMemoryNop(OFFSET(0xB6D13E, 0), 5);
			XUtil::PatchMemoryNop(OFFSET(0xBA0505, 0), 5);
			XUtil::PatchMemoryNop(OFFSET(0xBED343, 0), 5);
		}
#else
		_MESSAGE("Unfortunately, your compiled version does not support the 'Experimental::Unicode' option.");
#endif // !FALLOUT4_LAZ_UNICODE_PLUGIN
	}
	else
	{
	label_skip_msg_closeall_dialog:
		XUtil::PatchMemoryNop(OFFSET(0x5C4470, 0), 7);
		XUtil::PatchMemory(OFFSET(0x5C4477, 0), { 0xEB });
	}
}


/*
==================
F_UnicodePatches

Patches for facegen
==================
*/
VOID FIXAPI F_FaceGenPatches(VOID) {
	// Don't produce DDS files
	if (g_INI->GetBoolean("CreationKit_FaceGen", "DisableExportDDS", FALSE)) {
		XUtil::PatchMemoryNop(OFFSET(0xACBF63, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACBF83, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACBFA8, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACBFC8, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACBFED, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACC00D, 0), 5);
	}
		
	// Don't produce TGA files
	if (g_INI->GetBoolean("CreationKit_FaceGen", "DisableExportTGA", FALSE)) {
		XUtil::PatchMemoryNop(OFFSET(0xACC031, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACC050, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACC074, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACC093, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACC0B7, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xACC0D6, 0), 5);
	}

	// Don't produce NIF files
	if (g_INI->GetBoolean("CreationKit_FaceGen", "DisableExportNIF", FALSE))
		XUtil::PatchMemory(OFFSET(0xACC160, 0), { 0xC3 });

	// Allow variable tint mask resolution
	uint32_t tintResolution = g_INI->GetInteger("CreationKit_FaceGen", "TintMaskResolution", 2048);
	XUtil::PatchMemory(OFFSET(0x2B77ACC, 0), (uint8_t*)&tintResolution, sizeof(uint32_t));
	XUtil::PatchMemory(OFFSET(0x2B77AD3, 0), (uint8_t*)&tintResolution, sizeof(uint32_t));

	// Prevent internal filesystem reloads when exporting FaceGen for many NPCs
	XUtil::PatchMemory(OFFSET(0x3FD451, 0), { 0x95 });
	XUtil::PatchMemory(OFFSET(0x3FD45A, 0), { 0x48, 0x89, 0xF9, 0xE8, 0x9E, 0xE4, 0x00, 0x00 });
	XUtil::PatchMemoryNop(OFFSET(0x3FD462, 0), 0x1C);
	XUtil::DetourJump(OFFSET(0x40B900, 0), &ExportFaceGenForSelectedNPCs);
	XUtil::PatchMemoryNop(OFFSET(0xAC20BD, 0), 5);
}

/*
==================
MainFix_PatchFallout4CreationKit

Implements the code in the process CK64
==================
*/
VOID FIXAPI MainFix_PatchFallout4CreationKit(VOID)
{
	if (!_stricmp((LPCSTR)(g_ModuleBase + 0x3896168), "1.10.162.0"))
	{
		std::string sCmdLineStr = GetCommandLineA();

		LPSTR lpCmdLineStr = const_cast<LPSTR>(sCmdLineStr.c_str());
		LPSTR lpStartArgs = NULL;
		
		// if the path has spaces, then you should meet quotes at the beginning
		if (*lpCmdLineStr == '\"')
		{
			lpStartArgs = strchr(lpCmdLineStr + 1, '\"') + 1;
			AssertMsg(lpStartArgs, "Incorrect command line");
	
		ParserCommandLine:
			lpStartArgs = strchr(lpStartArgs, ':');
			if (!lpStartArgs)
				// need for normally run fixes
				nCountArgCmdLine = 1;
			else
			{
				nCountArgCmdLine++;
				lpStartArgs++;

				LPSTR lpArg = strtok(lpStartArgs, " ");
				while (lpArg != NULL)
				{
					// bimbo args? what...
					if (strcmp(lpArg, " ") && (lpArg != ""))
						nCountArgCmdLine++;

					lpArg = strtok(NULL, " ");
				}
			}
		}
		else
		{
			lpStartArgs = strchr(lpCmdLineStr, ' ');
			if (!lpStartArgs)
				// need for normally run fixes
				nCountArgCmdLine = 1;
			else
				goto ParserCommandLine;
		}

		_MESSAGE_FMT("CommandLine: %d (Args) %s", nCountArgCmdLine, GetCommandLineA());
	}
	else
	{
		CHAR modulePath[MAX_PATH];
		GetModuleFileNameA(GetModuleHandleA(NULL), modulePath, ARRAYSIZE(modulePath));

		CHAR message[1024];
		sprintf_s(message,
			"Unknown Creation Kit version detected. Patches are disabled.\n\n"
			"Required versions:\n"
			"CreationKit.exe 1.10.162.0 released on 2019-11-20\n"
			"\nExecutable path: %s", modulePath);

		MessageBoxA(NULL, message, "Version Check", MB_ICONERROR);
		return;
	}

	///////////////////////////////////////////////////////////////////////

	g_UIEnabled = (bool)g_INI->GetBoolean("CreationKit", "UI", false);
	g_i8DialogMode = (int8_t)g_INI->GetInteger("CreationKit", "DialogMode", 0);

	//
	// Replace broken crash dump functionality
	//
	if (g_INI->GetBoolean("CreationKit", "GenerateCrashdumps", TRUE))
	{
		Fix_GenerateCrashdumps();

		XUtil::PatchMemory(OFFSET(0x2D49F12, 0), { 0xC3 });			// crtSetUnhandledExceptionFilter
		XUtil::PatchMemory(OFFSET(0x204AE80, 0), { 0xC3 });			// StackTrace::MemoryTraceWrite
		XUtil::PatchMemory(OFFSET(0x204D1A0, 0), { 0xC3 });			// SetUnhandledExceptionFilter, BSWin32ExceptionHandler
		XUtil::PatchMemory(OFFSET(0x204D1E0, 0), { 0xC3 });			// SetUnhandledExceptionFilter, BSWin32ExceptionHandler
	}

	F_RequiredPatches();

	if (g_INI->GetBoolean("CreationKit", "SkipChangeWorldSpace", FALSE))
		XUtil::PatchMemoryNop(OFFSET(0x5FBE14, 0), 0x13);

	if (g_INI->GetBoolean("CreationKit", "SkipAnimationBuildProcessData", FALSE)) {
		//
		// Loading Files... Done! and continue
		//

		// Skip all
		XUtil::PatchMemory(OFFSET(0xB22C0, 0), { 0xC3 });
		// Skipping the build initialization AnimationBuildProcessDataExportUtils::DoDataExport
		// For speed up load files
		XUtil::PatchMemory(OFFSET(0xB2E48, 0), { 0xEB });
		// Remove temporary files
		DeleteFileA("TemporaryBehaviorEventInfoOutput.txt");
		DeleteFileA("TemporaryClipDataOutput.txt");
		DeleteFileA("TemporarySyncAnimDataOutput.txt");
		// Skipping create temporary files
		XUtil::PatchMemory(OFFSET(0xD36C0, 0), { 0xC3 });
	}

	F_FaceGenPatches();
	F_UIPatches();
	F_UnicodePatches();

	if (g_INI->GetBoolean("CreationKit", "DisableWindowGhosting", FALSE))
		DisableProcessWindowsGhosting();

	//
	// AllowSaveESM   - Allow saving ESMs directly without version control
	// AllowMasterESP - Allow ESP files to act as master files while saving
	//
	TESFile_CK::AllowSaveESM = g_INI->GetBoolean("CreationKit", "AllowSaveESM", FALSE);
	TESFile_CK::AllowMasterESP = g_INI->GetBoolean("CreationKit", "AllowMasterESP", FALSE);

	if (TESFile_CK::AllowSaveESM || TESFile_CK::AllowMasterESP)
	{
		*(uintptr_t*)&TESFile_CK::LoadTESInfo = Detours::X64::DetourFunctionClass(OFFSET(0x7FFF10, 0), &TESFile_CK::hk_LoadTESInfo);
		*(uintptr_t*)&TESFile_CK::WriteTESInfo = Detours::X64::DetourFunctionClass(OFFSET(0x800850, 0), &TESFile_CK::hk_WriteTESInfo);

		if (TESFile_CK::AllowSaveESM)
		{
			// Also allow non-game ESMs to be set as "Active File"
			XUtil::DetourCall(OFFSET(0x5A569F, 0), &TESFile_CK::IsActiveFileBlacklist);
			XUtil::PatchMemoryNop(OFFSET(0x7D9CD8, 0), 2);

			// Disable: "File '%s' is a master file or is in use.\n\nPlease select another file to save to."
			LPCSTR newFormat = "File '%s' is in use.\n\nPlease select another file to save to.";

			XUtil::PatchMemoryNop(OFFSET(0x7DDBC9, 0), 13);
			XUtil::PatchMemory(OFFSET(0x38840B0, 0), (uint8_t*)newFormat, strlen(newFormat) + 1);

			XUtil::DetourJump(OFFSET(0x646BD0, 0), &OpenPluginSaveDialog);	
		}
		
		if (TESFile_CK::AllowMasterESP)
		{
			// Remove the check for IsMaster()
			XUtil::PatchMemoryNop(OFFSET(0x7EF13C, 0), 9);
		}
	}
	
	//
	// Force the render window to draw at 60fps while idle (SetTimer(1ms)). 
	//
	if (g_INI->GetBoolean("CreationKit", "RenderWindowUnlockedFPS", FALSE))
	{
		// SetTimer minimum value 10ms
		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-settimer
		XUtil::PatchMemory(OFFSET(0x0463383, 0), { (uint8_t)USER_TIMER_MINIMUM });

		// In a separate option VSync
		if (!g_INI->GetBoolean("CreationKit", "VSyncRender", FALSE))
			XUtil::PatchMemory(OFFSET(0x2A39142, 0), { 0x33, 0xD2, 0x90 });
	}

	// Experimental. Must be run last to avoid interfering with other hooks and patches.
	// Optimization that nuukem did in CK SSE
	// https://github.com/Nukem9/SkyrimSETest/blob/master/skyrim64_test/src/patches/CKSSE/Experimental.cpp
	Experimental::RunOptimizations();
}