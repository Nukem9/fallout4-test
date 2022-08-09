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

#include "..\[Common]\StdAfx.h"
#include "Defender.h"
#include "bhkThreadMemorySource.h"

#include "CKF4/ExperimentalNuukem.h"
#include "CKF4/TranslateUnicode_CK.h"
#include "CKF4/UIProgressDialog.h"

// include patches for editor
#include "CKF4/Editor.h"
#include "CKF4/EditorUI.h"
#include "CKF4/UIThemeMode.h"
#include "CKF4/EditorUIProgressDialog.h"

// include windows
#include "CKF4/LayersWindow.h"
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

// API
#include "..\[EditorAPI]\BSFile.h"
#include "..\[EditorAPI]\BSArchiveManager.h"
#include "..\[EditorAPI]\TESObjectREFR.h"
#include "..\[EditorAPI]\BSResourceLooseFiles.h"
#include "..\[EditorAPI]\BSPointerHandleManager.h"

/*

This file is part of Fallout 4 Fixes source code.

*/

VOID FIXAPI ENBSeriesFixableRunHandler(VOID);
VOID FIXAPI Fix_PatchMemory(VOID);
VOID FIXAPI Fix_PatchThreading(VOID);
VOID FIXAPI Fix_HandleRefPatch(VOID);
VOID FIXAPI Fix_GenerateCrashdumps(VOID);
BOOL FIXAPI Fix_CheckPatchPreCombined(VOID);
VOID FIXAPI Fix_PatchD3D11_CreationKit(VOID);
size_t FIXAPI BNetConvertUnicodeString(char* Destination, size_t DestSize, const wchar_t* Source, size_t SourceSize);

namespace Classes = Core::Classes::UI;

static BSString sCommandRun;
static INT32 nCountArgCmdLine = 0;

BOOL bAllow64BitBA2Files = FALSE;

VOID FIXAPI F_RequiredPatches(VOID)
{
	// MemoryManager
	_MESSAGE_BEGIN_PATCH("Memory");

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

	_MESSAGE_END_PATCH;
}

VOID FIXAPI F_VerifiedPatches(VOID)
{
	_MESSAGE_BEGIN_PATCH("Dialoges");

	PatchIAT(hk_CreateDialogParamA, "USER32.DLL", "CreateDialogParamA");
	PatchIAT(hk_DialogBoxParamA, "USER32.DLL", "DialogBoxParamA");
	PatchIAT(hk_EndDialog, "USER32.DLL", "EndDialog");
	PatchIAT(hk_SendMessageA, "USER32.DLL", "SendMessageA");

	_MESSAGE_END_PATCH;

	_MESSAGE_BEGIN_PATCH("TESDataHandler");

	TESDataHandler::Initialize();
	// Getting a pointer to TESDataFileHandler_CK. (no actual)
	// And when the ReplacingTipsWithProgressBar option is enabled, the dialog starts.
	XUtil::DetourCall(OFFSET(0x5BE5A6, 0), &EditorUI::hk_CallLoadFile);
	// Foreground CellView and Object Windows after loading.
	// And when the ReplacingTipsWithProgressBar option is enabled, closing the dialog box.
	XUtil::DetourJump(OFFSET(0x7DA80D, 0), &EditorUI::hk_EndLoadFile);
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

	_MESSAGE_END_PATCH;

	_MESSAGE_BEGIN_PATCH("Console");

	// Adds support for quotation marks of some commands on the command line
	PatchCmdLineWithQuote();

	_MESSAGE_END_PATCH;

	_MESSAGE_BEGIN_PATCH("Thread");

	Fix_PatchThreading();

	_MESSAGE_END_PATCH;

	_MESSAGE_BEGIN_PATCH("Steam");

	XUtil::PatchMemory(OFFSET(0x2881F84, 0), { 0x74, 0x1B, 0x90, 0x90, 0x90, 0x90 });
	XUtil::PatchMemory(OFFSET(0x2881FB4, 0), { 0x74, 0x26, 0x90, 0x90, 0x90, 0x90 });
	XUtil::PatchMemory(OFFSET(0x2881FF4, 0), { 0x74, 0x1D, 0x90, 0x90, 0x90, 0x90 });

	_MESSAGE_END_PATCH;

	_MESSAGE_BEGIN_PATCH("Sky/Fog");

	PatchSky();
	// Fix for crash (nullptr no test) when close CK with Sky enable 
	XUtil::PatchMemory(OFFSET(0xF84521, 0), { 0xEB, 0x4D, 0x90 });
	XUtil::PatchMemory(OFFSET(0xF84570, 0), { 0x48, 0x85, 0xC9, 0x74, 0xB5, 0x48, 0x8B, 0x01, 0xEB, 0xAA });

	_MESSAGE_END_PATCH;

	_MESSAGE_BEGIN_PATCH("Misc");

	// Kill broken destructors causing crashes on exit
	XUtil::DetourCall(OFFSET(0x5B6DF7, 0), &QuitHandler);
	XUtil::DetourCall(OFFSET(0x2D48FC0, 0), &QuitHandler);
	XUtil::DetourCall(OFFSET(0x2D48FCF, 0), &QuitHandler);
	// Disable useless "Processing Topic X..." status bar updates
	XUtil::PatchMemoryNop(OFFSET(0xB89897, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0xB89BF3, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0xB8A472, 0), 5);
	// Fixed failed load d3dcompiler.dll
	XUtil::PatchMemoryNop(OFFSET(0x2A46AC0, 0), 6);
	// Fixed when the value is different from 0.0 to 1.0. Smoothness value to material (nif)
	XUtil::DetourCall(OFFSET(0x2B7F5B7, 0), &Fixed_IncorrectSmoothnessValueToMaterialNif);
	XUtil::PatchMemory(OFFSET(0x2B7F5BC, 0), { 0x66, 0x0F, 0x7E, 0x85, 0x88, 0x00, 0x00, 0x00, 0xEB, 0x18 });
	// Fixed crash when by load plugin in which there is no root parent to materials
	XUtil::PatchMemory(OFFSET(0x2500E7C, 0), { 0x0F, 0xC6, 0x1D, 0x1C, 0xC0, 0x30, 0x01, 0xEE, 0x0F, 0xC6, 0x05, 0x14,
		0xC0, 0x30, 0x01, 0x44, 0x31, 0xC0, 0xEB, 0x4B });
	XUtil::PatchMemory(OFFSET(0x8ED2AF, 0), { 0xEB, 0x6F });
	XUtil::PatchMemory(OFFSET(0x2AC1D07, 0), { 0xEB });
	// Fixed when you delete a group tinting to race window
	XUtil::PatchMemory(OFFSET(0x963E2E, 0), { 0x4D, 0x8B, 0x47, 0x8, 0x4C, 0x89, 0xE2 });
	XUtil::DetourCall(OFFSET(0x963E35, 0), &Fixed_DeleteTintingRace);
	XUtil::PatchMemory(OFFSET(0x963E3A, 0), { 0xEB, 0x18 });
	// Raise the papyrus script editor text limit to 500k characters from 64k
	XUtil::DetourCall(OFFSET(0x12E852C, 0), &hk_call_12E852C);
	// Assert if D3D11 FL11 features are not supported
	XUtil::DetourCall(OFFSET(0x2A48B1E, 0), &hk_call_142D12196);
	// Skipping the string check is more than 33 characters, this is irrelevant for new games
	XUtil::PatchMemory(OFFSET(0xDAF421, 0), { 0xEB });
	// Fixed infinite loop by Compile Papyrus Scripts...
	XUtil::PatchMemory(OFFSET(0x12E5E50, 0), { 0xC3 });
	// Enable the render window "Go to selection in game" hotkey even if version control is off
	XUtil::PatchMemoryNop(OFFSET(0x472123, 0), 2);
	// Fix for crash when using the -MapMaker command line option. Nullptr camera passed to BSGraphics::State::SetCameraData.
	XUtil::DetourCall(OFFSET(0x0906407, 0), &hk_call_140906407);
	// Fix for crash when tab control buttons are deleted. Uninitialized TCITEMA structure variables.
	XUtil::DetourJump(OFFSET(0x0564E30, 0), &EditorUI::TabControlDeleteItem);
	// Fix for crash (recursive sorting function stack overflow) when saving certain ESP files (i.e SimSettlements.esp)
	XUtil::DetourJump(OFFSET(0x07ED840, 0), &ArrayQuickSortRecursive<class TESForm_CK*>);
	XUtil::PatchMemory(OFFSET(0x07EDA50, 0), { 0xC3 });
	// Skipping the program update check
	XUtil::PatchMemory(OFFSET(0x5C180B, 0), { 0xEB, 0x10 });
	// Fix crash when Unicode string conversion fails with bethesda.net http responses
	XUtil::DetourJump(OFFSET(0x27DAF60, 0), &BNetConvertUnicodeString);
	// Change the default " 64-bit"
	const char* newTitlePart = " Fallout 4 64-bit";
	XUtil::PatchMemory(OFFSET(0x5C16CB, 0), { 0x8B });
	XUtil::PatchMemory(OFFSET(0x3857828, 0), (uint8_t*)&newTitlePart, sizeof(newTitlePart));

	// Fixed sFile Path (The option should specify the current directory, but when changing the folder, it points to the old one)
	PatchIAT(hk_modGetPrivateProfileStringA, "kernel32.dll", "GetPrivateProfileStringA");

#if !FALLOUT4_STUDY_CK64_INIFILE
	//
	// Skip option bSkipValidateForms and bDisableDuplicateReferenceCheck
	//
	PatchIAT(hk_modGetPrivateProfileIntA, "kernel32.dll", "GetPrivateProfileIntA");
#else
	//
	// For study ini
	//
	Tools::IniHookInputInit();
#endif // !FALLOUT4_STUDY_CK64_INIFILE

	// Fixed a very harmful error that pops up very rarely
	XUtil::DetourCall(OFFSET(0x2511176, 0), &hk_call_2511176);

	//////////////////////////////////////
	//
	// Skip some warning
	//

	// EXTRA SPACE
	XUtil::PatchMemoryNop(OFFSET(0x86F151, 0), 0x35);

	// NavMesh ID %08X in cell %s has invalid cover data.  It will be removed.
	XUtil::PatchMemoryNop(OFFSET(0x100B39C, 0), 0x5);
	
	// Animation messages
	XUtil::PatchMemoryNop(OFFSET(0x257CF88, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0x257D4B3, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0x257D518, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0x257D44A, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0x1C62B27, 0), 5);
	// TEXTURE Unable to load file
	XUtil::PatchMemory(OFFSET(0x2AC6E17, 0), { 0xE9, 0xB8, 0x00, 0x00, 0x00, 0x90 });
	// MODEL Could not find model
	XUtil::PatchMemoryNop(OFFSET(0x260CFC6, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0x260D2A3, 0), 5);
	// SHADER Could not find material
	XUtil::PatchMemoryNop(OFFSET(0x183D42, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0x2AB67AC, 0), 5);
	// MODEL Found geometry
	XUtil::PatchMemoryNop(OFFSET(0x8DF0A6, 0), 5);
	//////////////////////////////////////

	_MESSAGE_END_PATCH;
}

VOID FIXAPI F_OptimizationWithRiskPatches(VOID)
{
	if (g_INI->GetBoolean("CreationKit", "AlteredFormListPatch", FALSE))
	{
		_MESSAGE_BEGIN_PATCH("TESForm storage");

		XUtil::DetourCall(OFFSET(0x853E47, 0), &TESForm::AlteredFormList_ElementExists);

		_MESSAGE_END_PATCH;
	}

	_MESSAGE_BEGIN_PATCH("PreVis");

	XUtil::PatchMemoryNop(OFFSET(0x133BB9A, 0), 0x62);

	_MESSAGE_END_PATCH;

	if (g_INI->GetBoolean("CreationKit", "BSHandleRefObjectPatch", FALSE))
	{
		_MESSAGE_BEGIN_PATCH("BSHandleRefObject");

		Fix_HandleRefPatch();

		_MESSAGE_END_PATCH;
	}

	_MESSAGE_BEGIN_PATCH("FastLoad");
	
	// Plugin loading optimizations
	INT32 cpuinfo[4];
	__cpuid(cpuinfo, 1);
	bool sse41 = (cpuinfo[2] & (1 << 19)) != 0;

	if (sse41 && g_INI->GetBoolean("CreationKit", "BSTArraySearchItemReplacement", FALSE)) {
		// Without SIMD support, there is no pointand the code has been cut out
		_MESSAGE("Utilize SSE4.1 instructions if available and loading optimizations enabled.");

		// In the previous version, there was a different signature.
		// I have redone and now it does not lead to errors in previs, because they were slightly different, namely the size of the data type.

		std::size_t count = 0;

		std::vector<uintptr_t>::iterator match;
		std::vector<uintptr_t> matches = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase,
			"48 83 C5 08 41 3B FE 72 9F 48 8B 6C 24 50 8B C3 48 8B 5C 24 58 48 8B 74 24 60 48 83 C4 30 41 5F 41 5E 5F C3 8B C3 EB E8");

		XUtil::Parallel::for_each(match = matches.begin(), matches.end(), [&count](auto it) { XUtil::DetourJump(it - 0x8A, &Experimental::QSIMDFastSearchArrayItemQWORD); count++; });

		matches = XUtil::FindPatterns(g_CodeBase, g_CodeEnd - g_CodeBase,
			"48 83 C5 08 41 3B FE 72 9F 48 8B 6C 24 58 48 8B 74 24 60 8B C3 48 8B 5C 24 50 48 83 C4 30 41 5F 41 5E 5F C3");

		XUtil::Parallel::for_each(match = matches.begin(), matches.end(), [&count](auto it) { XUtil::DetourJump(it - 0x8D, &Experimental::QSIMDFastSearchArrayItemOffsetQWORD); count++; });

		XUtil::DetourJump(OFFSET(0xB0930, 0), &Experimental::QSIMDFastSearchArrayItemDWORD);
		XUtil::DetourJump(OFFSET(0x193D20, 0), &Experimental::QSIMDFastSearchArrayItemDWORD);
		XUtil::DetourJump(OFFSET(0x409AB0, 0), &Experimental::QSIMDFastSearchArrayItemDWORD);

		class Search_IA128 : public Xbyak::CodeGenerator {
		public:
			Search_IA128(VOID) : Xbyak::CodeGenerator() {
				mov(ptr[rsp + 0x10], rbx);
				mov(ptr[rsp + 0x18], rsi);
				push(rdi);
				push(r14);
				push(r15);
				sub(rsp, 0x30);
				mov(r14d, dword[rcx + 0x10]);
				or (ebx, 0xFFFFFFFF);
				xor (edi, edi);
				mov(r15, rdx);
				mov(rsi, rcx);
				test(r14d, r14d);
				je(".quit_no");
				push(rbp);
				mov(ebp, edi);
				mov(rcx, qword[r15]);
				mov(rdx, qword[rsi]);
				L(".c1");
				cmp(ebx, -1);
				jne(".quit_yes");
				cmp(qword[rdx + rbp], rcx);
				cmove(ebx, edi);
				inc(edi);
				lea(rbp, qword[rbp + 0x10]);
				cmp(edi, r14d);
				jb(".c1");
				L(".quit_yes");
				pop(rbp);
				L(".quit_no");
				mov(eax, ebx);
				mov(rbx, qword[rsp + 0x58]);
				mov(rsi, qword[rsp + 0x60]);
				add(rsp, 0x30);
				pop(r15);
				pop(r14);
				pop(rdi);
				ret();
			}

			static VOID Generate(uintptr_t Target) {
				auto hook = new Search_IA128();
				XUtil::DetourJump(Target, (uintptr_t)hook->getCode());
			}

		};

		Search_IA128::Generate(OFFSET(0xB21130, 0));
		Search_IA128::Generate(OFFSET(0x131790, 0));
		Search_IA128::Generate(OFFSET(0x576540, 0));
		Search_IA128::Generate(OFFSET(0xAAEDD0, 0));

		count += 3;

		_MESSAGE_FMT("Replaced function with SIMD function: %d.", count);
	}

	// Skip remove failed forms
	XUtil::PatchMemory(OFFSET(0x7E4064, 0), { 0xEB });
	// Skip preload interier or exterier
	XUtil::PatchMemory(OFFSET(0x5BE646, 0), { 0xEB });

	XUtil::DetourCall(OFFSET(0x08056B7, 0), &hk_inflateInit);
	XUtil::DetourCall(OFFSET(0x08056F7, 0), &hk_inflate);
	PatchIAT(hk_FindFirstFileA, "kernel32.dll", "FindFirstFileA");

	_MESSAGE_END_PATCH;

	_MESSAGE_BEGIN_PATCH("Archive");

	//
	// Archive load interceptor
	//
	XUtil::DetourCall(OFFSET(0x24BB1D7, 0), BSArchive::hk_LoadArchive);
	XUtil::DetourJump(OFFSET(0x24E2E40, 0), BSArchive::hk_Check64bitSize);

	// Fix for crash when plugins.txt is present in the game root folder. Buffer overflow in ArchiveManager::OpenMasterArchives when appending
	// to a string. Skip the parsing code completely.
	XUtil::PatchMemoryNop(OFFSET(0x249EFD7, 0), 6);

	_MESSAGE_END_PATCH;
}

VOID FIXAPI F_UIPatches(VOID) {
	auto comDll = (uintptr_t)GetModuleHandleA("comctl32.dll");
	Assert(comDll);
	
	_MESSAGE_BEGIN_PATCH("UI");

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
	}

	EditorUI::Initialize();

	*(uintptr_t*)&MainWindow::OldWndProc = Detours::X64::DetourFunctionClass(OFFSET(0x05B74D0, 0), &MainWindow::WndProc);
	*(uintptr_t*)&ObjectWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x03F9020, 0), &ObjectWindow::DlgProc);
	*(uintptr_t*)&CellViewWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x059D820, 0), &CellViewWindow::DlgProc);
	*(uintptr_t*)&ResponseWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x0B5EB50, 0), &ResponseWindow::DlgProc);
	*(uintptr_t*)&RenderWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x460570, 0), &RenderWindow::DlgProc);
	*(uintptr_t*)&DataWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x5A8250, 0), &DataWindow::DlgProc);
	*(uintptr_t*)&LayersWindow::OldDlgProc = Detours::X64::DetourFunctionClass(OFFSET(0x3C7A80, 0), &LayersWindow::DlgProc);

	// RENDER: Clinging to have a list of selected forms
	//XUtil::DetourJump(OFFSET(0x563BF0, 0), &RenderWindow::IPicker::hk_Add);
	//XUtil::DetourJump(OFFSET(0x563C90, 0), &RenderWindow::IPicker::hk_Remove);
	//
	// Fix movable Ligth refr
	//
	/**(uintptr_t*)&BGSRenderWindowReferenceEditModule::MoveObjects = 
		Detours::X64::DetourFunctionClass(OFFSET(0x4D3890, 0), &BGSRenderWindowReferenceEditModule::hk_MoveObjects);
	*(uintptr_t*)&TESObjectREFR::SetNewPosition = 
		Detours::X64::DetourFunctionClass(OFFSET(0xE24B10, 0), &TESObjectREFR::hk_SetNewPosition);*/

	//XUtil::DetourJump(OFFSET(0x4D3890, 0), &BGSRenderWindowReferenceEditModule::hk_MoveObjects);

	if (UITheme::IsEnabledMode()) {
		*(uintptr_t*)&PreferencesWindow::OldDlgProc = OFFSET(0x1335AF0, 0);
		XUtil::DetourCall(OFFSET(0x1336521, 0), &PreferencesWindow::CreateDialogParamA);
	}

	// Layers enable doublebuffered
	XUtil::DetourCall(OFFSET(0x67A325, 0), &LayersWindow::SendMessageAfterCreateTreeView);
	// Layers dialog fix resize
	XUtil::DetourCall(OFFSET(0x67C135, 0), &LayersWindow::MoveWindowBody);
	XUtil::DetourCall(OFFSET(0x67C165, 0), &LayersWindow::MoveWindowHeader);

	// ret skip Warnings window
	XUtil::PatchMemory(OFFSET(0x551C30, 0), { 0xC3, 0xCC, 0xCC, 0xCC, 0xCC });
	// ACTOR: Wrong InvalidateRect
	XUtil::PatchMemoryNop(OFFSET(0x717FF1, 0), 6);
	XUtil::PatchMemoryNop(OFFSET(0x565FFA, 0), 6);
	XUtil::PatchMemoryNop(OFFSET(0xAC7875, 0), 0xF);
	XUtil::PatchMemoryNop(OFFSET(0x645048, 0), 0x1D);

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
	// Fix for crash when editing a spell effect with a large (>= 1'000'000'000) duration. WARNING: Stack padding allows the buffer
	// to be up to 12 bytes, 10 are originally reserved.
	//
	XUtil::PatchMemory(OFFSET(0xF4D344, 0), { 0xBA, 0x0C, 0x00, 0x00, 0x00 });
	XUtil::PatchMemory(OFFSET(0xF4D45C, 0), { 0xBA, 0x0C, 0x00, 0x00, 0x00 });

	//
	// Version Control fixes
	//

	XUtil::DetourCall(OFFSET(0x13339D6, 0), &LogWindow::LogInsteadOfMsgBox);
	// By disabling version control, allow the start
	XUtil::PatchMemory(OFFSET(0x5C3C4F, 0), { 0xEB, 0x81 });	// skip msgbox 
	XUtil::PatchMemory(OFFSET(0x5C3C74, 0), { 0xEB, 0xD9 });	// skip msgbox
	
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

	// Run the progress dialog when loading the interior in the render.
	XUtil::DetourCall(OFFSET(0x59F6B9, 0), &BGSRenderWindow::SetCell);

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
			XUtil::DetourClassCall(OFFSET(0x7E2FF6, 0), &TESDataHandler::InitUnknownDataSetTextStatusBar);
			// During the entire process, the update is only 95 times for each percentage.... very little, get in here for an update
			//XUtil::DetourCall(OFFSET(0x7DEA67, 0), &EditorUI::hk_UpdateProgress);
			// Load Files... Done... etc.
			XUtil::DetourCall(OFFSET(0x7E34FD, 0), &EditorUI::hk_SetTextAndSendStatusBar);
			XUtil::DetourCall(OFFSET(0x7DC390, 0), &EditorUI::hk_SetTextAndSendStatusBar);

			// The terrain patch freezes, so a queue is created taking into account the activity of the dialog
			//XUtil::DetourClassCall(OFFSET(0x2629D96, 0), &Classes::CUIProgressDialog::ProcessMessages);
			//XUtil::DetourClassCall(OFFSET(0x262A6B2, 0), &Classes::CUIProgressDialog::ProcessMessages);
			//XUtil::DetourClassCall(OFFSET(0x262A6BF, 0), &Classes::CUIProgressDialog::ProcessMessages);

			//// Experimental: Loading cell ... (need for progressbar drawing)
			//XUtil::DetourClassJump(OFFSET(0xE1D2C7, 0), &Classes::CUIProgressDialog::ProcessMessagesOnlyLoadCellWorld);
		}
	}

	_MESSAGE_END_PATCH;
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
	if (g_INI->GetBoolean("CreationKit", "Unicode", FALSE)) {
		_MESSAGE_BEGIN_PATCH("Unicode");
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
		_MESSAGE_END_PATCH;
	}
	else {
	label_skip_msg_closeall_dialog:
		XUtil::PatchMemoryNop(OFFSET(0x5C4470, 0), 7);
		XUtil::PatchMemory(OFFSET(0x5C4477, 0), { 0xEB });
	}
}


/*
==================
F_UnicodePatches

For read large files
==================
*/
VOID FIXAPI F_LooseFilePatches(VOID) {
	if (!g_INI->GetBoolean("CreationKit", "LooseFilePatch", FALSE))
		return;

	_MESSAGE_FMT("It is allowed to load large files with risk.");
	bAllow64BitBA2Files = TRUE;

	//
	// Loose files
	//
	_MESSAGE_BEGIN_PATCH("Loose files");

	// Set new size class 0x160 to 0x180
	XUtil::PatchMemory(OFFSET(0x24D3341, 0), { 0x80 });

	// I will add initialization to fill in the new fields with data
	class LooseFileStreamHook : public Xbyak::CodeGenerator {
	public:
		LooseFileStreamHook(VOID) : Xbyak::CodeGenerator()
		{
			push(r11);
			push(rax);
			mov(rcx, ptr[r11 + 0x18]);
			mov(rdx, rbp);
			sub(rsp, 0x40);
			mov(rax, (uintptr_t)&BSResource::LooseFileStream::CreateInstance);
			call(rax);
			add(rsp, 0x40);
			pop(rax);
			pop(r11);
			mov(rbx, ptr[r11 + 0x10]);
			mov(rbp, ptr[r11 + 0x18]);
			mov(rsi, ptr[r11 + 0x20]);
			mov(rsp, r11);
			pop(rdi);
			ret();
		}

		static VOID Generate(uintptr_t Target) {
			auto hook = new LooseFileStreamHook();
			XUtil::DetourJump(Target, (uintptr_t)hook->getCode());
		}
	};
	LooseFileStreamHook::Generate(OFFSET(0x24D3BB7, 0));

	// As I understand it, CK evaluates the amount of useful data with the file size, 
	// since I will now adjust the 64-bit size, sacrificing checking for NullPtr.
	//
	// mov rax, qword ptr ds:[rcx+0x160]
	// jmp -> cmp rsi, rax
	XUtil::PatchMemory(OFFSET(0x24BB104, 0), { 0x48, 0x8B, 0x81, 0x60, 0x01, 0x00, 0x00, 0xEB, 0x1D });

	// Ignoring the correctness check is not useful
	XUtil::PatchMemory(OFFSET(0x24D6E50, 0), { 0xEB });
	XUtil::PatchMemory(OFFSET(0x24D6FDD, 0), { 0xEB });
	XUtil::PatchMemory(OFFSET(0x24D7196, 0), { 0xEB });
	XUtil::PatchMemory(OFFSET(0x24D87A0, 0), { 0xC3 });
	XUtil::PatchMemory(OFFSET(0x24D87F0, 0), { 0xC3 });

	_MESSAGE_END_PATCH;
}


/*
==================
F_UnicodePatches

Patches for facegen
==================
*/
VOID FIXAPI F_FaceGenPatches(VOID) {
	if (!g_INI->GetBoolean("CreationKit", "FaceGenPatch", FALSE))
		return;

	_MESSAGE_BEGIN_PATCH("FaceGen");

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

	_MESSAGE_END_PATCH;
}

/*
==================
MainFix_PatchFallout4CreationKit

Implements the code in the process CK64
==================
*/
VOID FIXAPI MainFix_PatchFallout4CreationKit(VOID)
{
	BOOL versionSupportedCK = !_stricmp((LPCSTR)(g_ModuleBase + 0x3896168), "1.10.162.0");
	if (versionSupportedCK) {
		std::string sCmdLineStr = GetCommandLineA();

		LPSTR lpCmdLineStr = const_cast<LPSTR>(sCmdLineStr.c_str());
		LPSTR lpStartArgs = NULL;
		
		// if the path has spaces, then you should meet quotes at the beginning
		if (*lpCmdLineStr == '\"') {
			lpStartArgs = strchr(lpCmdLineStr + 1, '\"') + 1;
			AssertMsg(lpStartArgs, "Incorrect command line");
			
		ParserCommandLine:
			std::string sDataCmdLineStr = lpStartArgs;
			lpStartArgs = strchr(lpStartArgs, ':');
			if (!lpStartArgs)
				// need for normally run fixes
				nCountArgCmdLine = 1;
			else {
				sCommandRun = XUtil::Str::trim(sDataCmdLineStr.substr(0, sDataCmdLineStr.find_first_of(':')).c_str()).c_str();
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
		else {
			lpStartArgs = strchr(lpCmdLineStr, ' ');
			if (!lpStartArgs)
				// need for normally run fixes
				nCountArgCmdLine = 1;
			else
				goto ParserCommandLine;
		}
	
		if (*sCommandRun)
			_MESSAGE_FMT("Command: %s", *sCommandRun);

		_MESSAGE_FMT("CommandLine: %d (Args) %s", nCountArgCmdLine, GetCommandLineA());

		if (g_LoadType == GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4_PATCHED_PREVIS) {

			if (Fix_CheckPatchPreCombined())
			{
				if (g_INI->GetBoolean("CreationKit", "BSHandleRefObjectPatch", FALSE))
				{
					MessageBoxA(NULL,
						"Patched SeargeDP Creation Kit version detected.\n"
						"It is already incompatible.\n\n"
						"Close Creation Kit.\n",
						"Incorrect user actions",
						MB_ICONERROR);

					QuitHandler();
					return;
				}
				else
				{
					_MESSAGE("Detected patch SeargeDP");

					if ((nCountArgCmdLine == 1) || (sCommandRun.Compare("-GeneratePreCombined") && sCommandRun.Compare("-GeneratePreVisData") &&
						sCommandRun.Compare("-BuildCDX") && sCommandRun.Compare("-CompressPSG"))) {
						MessageBoxA(NULL,
							"Patched SeargeDP Creation Kit version detected.\nCalling an unsupported command.\n\n"
							"Close Creation Kit.\n"
							"Support command:\n"
							"-GeneratePreCombined, -GeneratePreVisData, -BuildCDX and -CompressPSG",
							"Incorrect user actions",
							MB_ICONERROR);

						QuitHandler();
						return;
					}
				}
			}
			else
				goto UnknownVersionCK;
		}
	}
	else {
		UnknownVersionCK:
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

	srand(GetTickCount64());

//	BOOL checkVTable = g_INI->GetBoolean("CreationKit", "CheckVTable", FALSE);
	g_AttachRenderWnd = g_INI->GetBoolean("CreationKit", "AttachRenderWindow", FALSE);

#if FALLOUT4_CK64_ENB_FIXABLE
	//
	// ENBSeries detected and create double launch CreationKit
	//
	//if (!checkVTable)
		ENBSeriesFixableRunHandler();
#endif // FALLOUT4_CK64_ENB_FIXABLE

	// Let's check our CK and will restore it if necessary
	//DlCheckModuleProcess CheckModuleProcess;

	//if (checkVTable)
	//	CheckModuleProcess.AnalyzeAndRecoveryVTable();

#if FALLOUT4_DEVELOPER_MODE
	MSRTTI::Initialize();
#endif // FALLOUT4_DEVELOPER_MODE

	// Call constructor DlLoadModule
	DlLoadModule LoadModule;

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

	// Option for automatic load of archives
	BOOL loaderBA2 = g_INI->GetBoolean("CreationKit", "OriginalLoadBA2", TRUE);

	if (!loaderBA2) {
		_MESSAGE_BEGIN_PATCH("LoaderBA2");

		if (g_i8NeedLoadBA2 = g_INI->GetInteger("CreationKit", "AutoloadBA2Files", 3); g_i8NeedLoadBA2 > 0)
			ArchiveManager.Initialize();

		_MESSAGE_END_PATCH;
	}
	else
		OriginalLoadBA2();

	F_RequiredPatches();
	F_VerifiedPatches();
	F_OptimizationWithRiskPatches();
	F_LooseFilePatches();
	
	// Will force CK to read large pages
	if (g_INI->GetBoolean("CreationKit", "IOPatch", FALSE)) {
		_MESSAGE_BEGIN_PATCH("IOPatch");

		// 2 kb -> x kb >= 256 kb
		*(uintptr_t*)&BSFile::ICreateInstance = Detours::X64::DetourFunctionClass(OFFSET(0x2482D70, 0), &BSFile::hk_ICreateInstance);
		// 2 kb -> 256 kb
		XUtil::PatchMemory(OFFSET(0x7FF972, 0), { 0x41, 0xB9, 0x00, 0x00, 0x04, 0x00, 0x90 });
		XUtil::PatchMemory(OFFSET(0x7FEE30, 0), { 0x04 });
		XUtil::PatchMemoryNop(OFFSET(0x7FEE2F, 0), 4);

		_MESSAGE_END_PATCH;
	}

	if (g_INI->GetBoolean("CreationKit", "LoadingExtremelyUnreliableFiles", FALSE))
		EnabledExtremelyMode();
	
	if (g_INI->GetBoolean("CreationKit", "SkipChangeWorldSpace", FALSE))
		XUtil::PatchMemoryNop(OFFSET(0x5FBE14, 0), 0x13);

	//
	// Fix for the -GeneratePreCombined command line option creating files for the PS4 (2) format. It should be WIN64 (0).
	//
	XUtil::PatchMemory(OFFSET(0xDCB7DB, 0), { 0x00, 0x00, 0x00, 0x00 });

	if (nCountArgCmdLine != 1 && g_LoadType == GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4 && !sCommandRun.Compare("-GeneratePreCombined")) {
		_MESSAGE_BEGIN_PATCH("PreCombined");
		
		//
		// Additional options taken from the patch, if you do not use the patched F4CK.
		//
		XUtil::PatchMemory(OFFSET(0x347E6E, 0), { 0x00, 0x00, 0x00, 0x00 });
		XUtil::PatchMemory(OFFSET(0xDCB677, 0), { 0x00, 0x00, 0x00, 0x00 });

		_MESSAGE_END_PATCH;
	}

	if (nCountArgCmdLine == 1 && g_INI->GetBoolean("CreationKit", "SkipAnimationBuildProcessData", FALSE)) {
		//
		// Loading Files... Done! and continue
		//
		SkipAnimationTextExport();

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
	Fix_PatchD3D11_CreationKit();

	if (g_INI->GetBoolean("CreationKit", "DisableWindowGhosting", FALSE))
		DisableProcessWindowsGhosting();

	//
	// AllowSaveESM   - Allow saving ESMs directly without version control
	// AllowMasterESP - Allow ESP files to act as master files while saving
	//
	TESFile::AllowSaveESM = g_INI->GetBoolean("CreationKit", "AllowSaveESM", FALSE);
	TESFile::AllowMasterESP = g_INI->GetBoolean("CreationKit", "AllowMasterESP", FALSE);

	if (TESFile::AllowSaveESM || TESFile::AllowMasterESP) {
		_MESSAGE_BEGIN_PATCH("AllowSaveESM/AllowMasterESP");

		*(uintptr_t*)&TESFile::LoadTESInfo = Detours::X64::DetourFunctionClass(OFFSET(0x7FFF10, 0), &TESFile::hk_LoadTESInfo);
		*(uintptr_t*)&TESFile::WriteTESInfo = Detours::X64::DetourFunctionClass(OFFSET(0x800850, 0), &TESFile::hk_WriteTESInfo);

		if (TESFile::AllowSaveESM) {
			// Also allow non-game ESMs to be set as "Active File"
			XUtil::DetourCall(OFFSET(0x5A569F, 0), &TESFile::IsMasterFileToBlacklist);
			XUtil::PatchMemoryNop(OFFSET(0x7D9CD8, 0), 2);

			// Disable: "File '%s' is a master file or is in use.\n\nPlease select another file to save to."
			LPCSTR newFormat = "File '%s' is in use.\n\nPlease select another file to save to.";

			XUtil::PatchMemoryNop(OFFSET(0x7DDBC9, 0), 13);
			XUtil::PatchMemory(OFFSET(0x38840B0, 0), (uint8_t*)newFormat, strlen(newFormat) + 1);

			XUtil::DetourJump(OFFSET(0x646BD0, 0), &OpenPluginSaveDialog);	
		}
		
		if (TESFile::AllowMasterESP)
			// Remove the check for IsMaster()
			XUtil::PatchMemoryNop(OFFSET(0x7EF13C, 0), 9);

		_MESSAGE_END_PATCH;
	}

	//
	// Remove assertion message boxes
	//
	if (g_INI->GetBoolean("CreationKit", "DisableAssertions", FALSE)) {
		_MESSAGE_BEGIN_PATCH("DisableAssertions");

		XUtil::PatchMemoryNop(OFFSET(0x2001B80, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0x2001C06, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0x2001C77, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0x2001CF4, 0), 5);

		_MESSAGE_END_PATCH;
	}

	//
	// Workaround for ref links and enable state parent links (2D lines) causing the CK to hang indefinitely when too many objects
	// are selected. This hack prevents said lines from being created or rendered.
	//
	if (g_INI->GetBoolean("CreationKit", "RefLinkGeometryHangWorkaround", FALSE)) {
		_MESSAGE_BEGIN_PATCH("RefLinkGeometryHangWorkaround");

		XUtil::PatchMemoryNop(OFFSET(0xE7062E, 0), 5);
		XUtil::PatchMemoryNop(OFFSET(0xE970DC, 0), 5);

		_MESSAGE_END_PATCH;
	}

	//
	// Workaround for version control not allowing merges when a plugin index is above 02. Bethesda's VC bitmap files determine
	// check-in status along with user IDs for each specific form in the game. They're also hardcoded for 2 masters only. Using
	// this hack for anything EXCEPT merging will break the bitmaps.
	//
	if (g_INI->GetBoolean("CreationKit", "VersionControlMergeWorkaround", FALSE)) {
		_MESSAGE_BEGIN_PATCH("VersionControlMergeWorkaround");

		XUtil::PatchMemory(OFFSET(0x61C86F, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x61E667, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x61EB57, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x61EFB8, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x6270DE, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x61C892, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x61E68A, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x61EB7A, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x61EFDB, 0), { 0xEB });
		XUtil::PatchMemory(OFFSET(0x6270FF, 0), { 0xEB });

		_MESSAGE_END_PATCH;
	}

	if (g_INI->GetBoolean("CreationKit", "UIHotkeys", FALSE)) {
		_MESSAGE_BEGIN_PATCH("UIHotkeys");

		g_HotkeyReplace = TRUE;

		XUtil::PatchMemoryNop(OFFSET(0x457763, 0), 0x19);
		
		Detours::X64::DetourFunctionClass(OFFSET(0xAADD, 0), &EditorUI::RegisterHotkeyFunction);

		_MESSAGE_END_PATCH;
	}

	//
	// Force the render window to draw at 60fps while idle (SetTimer(1ms)). 
	//
	if (g_INI->GetBoolean("CreationKit", "RenderWindowUnlockedFPS", FALSE)) {
		_MESSAGE_BEGIN_PATCH("RenderWindowUnlockedFPS");

		// SetTimer minimum value 10ms
		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-settimer
		XUtil::PatchMemory(OFFSET(0x0463383, 0), { (uint8_t)USER_TIMER_MINIMUM });

		// In a separate option VSync
		if (!g_INI->GetBoolean("CreationKit", "VSyncRender", FALSE))
			XUtil::PatchMemory(OFFSET(0x2A39142, 0), { 0x33, 0xD2, 0x90 });

		_MESSAGE_END_PATCH;
	}

	//
	// Layers: I can't fix it, everything that contains a primitive is added with a leak. 
	// I'll just forbid adding them to the layer.
	//
	if (g_INI->GetBoolean("CreationKit", "BanAddPrimitiveToLayer", FALSE))
		XUtil::DetourCall(OFFSET(0x4C8F9D, 0), &EditorUI::hkAddRefrToLayer);

	_MESSAGE_BEGIN_PATCH("Optimizations");
	
	// Experimental. Must be run last to avoid interfering with other hooks and patches.
	// Optimization that nuukem did in CK SSE
	// https://github.com/Nukem9/SkyrimSETest/blob/master/skyrim64_test/src/patches/CKSSE/Experimental.cpp
	Experimental::RunOptimizations();

	_MESSAGE_END_PATCH;
}