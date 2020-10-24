#include "../common.h"
#include <libdeflate/libdeflate.h>
#include <intrin.h>
#include "TES/MemoryManager.h"
#include "TES/bhkThreadMemorySource.h"
#include "CKF4/Editor.h"
#include "CKF4/EditorUI.h"
#include "CKF4/EditorUIDarkMode.h"
#include "CKF4/LogWindow.h"
#include "CKF4/ExperimentalNuukem.h"
#include "CKF4/TranslateUnicode_CK.h"
#include "CKF4/UIProgressDialog.h"

void PatchMemory();
void PatchFileIO();

void Patch_Fallout4CreationKit()
{
	if (!_stricmp((const char *)(g_ModuleBase + 0x3896168), "1.10.162.0"))
	{
		// ???
	}
	else
	{
		char modulePath[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(nullptr), modulePath, ARRAYSIZE(modulePath));

		char message[1024];
		sprintf_s(message,
			"Unknown Creation Kit version detected. Patches are disabled.\n\n"
			"Required versions:\n"
			"CreationKit.exe 1.10.162.0 released on 2019-11-20\n"
			"\nExecutable path: %s", modulePath);

		MessageBoxA(nullptr, message, "Version Check", MB_ICONERROR);
		return;
	}

	//
	// Replace broken crash dump functionality
	//
	if (g_INI.GetBoolean("CreationKit", "GenerateCrashdumps", true))
	{
		SetUnhandledExceptionFilter(DumpExceptionHandler);

		XUtil::PatchMemory(OFFSET(0x2D49F12, 0), { 0xC3 });// crtSetUnhandledExceptionFilter
		XUtil::PatchMemory(OFFSET(0x204AE80, 0), { 0xC3 });// StackTrace::MemoryTraceWrite
		XUtil::PatchMemory(OFFSET(0x204D1A0, 0), { 0xC3 });// SetUnhandledExceptionFilter, BSWin32ExceptionHandler
		XUtil::PatchMemory(OFFSET(0x204D1E0, 0), { 0xC3 });// SetUnhandledExceptionFilter, BSWin32ExceptionHandler

		_set_invalid_parameter_handler([](const wchar_t *, const wchar_t *, const wchar_t *, uint32_t, uintptr_t)
		{
			RaiseException('PARM', EXCEPTION_NONCONTINUABLE, 0, nullptr);
		});

		auto purecallHandler = []()
		{
			RaiseException('PURE', EXCEPTION_NONCONTINUABLE, 0, nullptr);
		};

		auto terminateHandler = []()
		{
			RaiseException('TERM', EXCEPTION_NONCONTINUABLE, 0, nullptr);
		};

		PatchIAT((void(*)())terminateHandler, "MSVCR110.dll", "_cexit");
		PatchIAT((void(*)())terminateHandler, "MSVCR110.dll", "_exit");
		PatchIAT((void(*)())terminateHandler, "MSVCR110.dll", "exit");
		PatchIAT((void(*)())terminateHandler, "MSVCR110.dll", "abort");
		PatchIAT((void(*)())terminateHandler, "MSVCR110.dll", "terminate");
		PatchIAT((void(*)())purecallHandler, "MSVCR110.dll", "_purecall");
	}

	//
	// MemoryManager
	//
	if (g_INI.GetBoolean("CreationKit", "MemoryPatch", false))
	{
		PatchMemory();

		XUtil::PatchMemory(OFFSET(0x030ECC0, 0), { 0xC3 });							// [3GB  ] MemoryManager - Default/Static/File heaps
		XUtil::PatchMemory(OFFSET(0x2004B70, 0), { 0xC3 });							// [1GB  ] BSSmallBlockAllocator
		XUtil::DetourJump(OFFSET(0x21115D0, 0), &bhkThreadMemorySource::__ctor__);	// [512MB] bhkThreadMemorySource
		XUtil::PatchMemory(OFFSET(0x200A920, 0), { 0xC3 });							// [64MB ] ScrapHeap init
		XUtil::PatchMemory(OFFSET(0x200B440, 0), { 0xC3 });							// [64MB ] ScrapHeap deinit
																					// [128MB] BSScaleformSysMemMapper is untouched due to complexity

		XUtil::DetourJump(OFFSET(0x2004E20, 0), &MemoryManager::Allocate);
		XUtil::DetourJump(OFFSET(0x20052D0, 0), &MemoryManager::Deallocate);
		XUtil::DetourJump(OFFSET(0x2004300, 0), &MemoryManager::Size);
		XUtil::DetourJump(OFFSET(0x200AB30, 0), &ScrapHeap::Allocate);
		XUtil::DetourJump(OFFSET(0x200B170, 0), &ScrapHeap::Deallocate);
	}

	//
	// UI
	//
	PatchIAT(hk_CreateDialogParamA, "USER32.DLL", "CreateDialogParamA");
	PatchIAT(hk_DialogBoxParamA, "USER32.DLL", "DialogBoxParamA");
	PatchIAT(hk_EndDialog, "USER32.DLL", "EndDialog");
	PatchIAT(hk_SendMessageA, "USER32.DLL", "SendMessageA");

	if (g_INI.GetBoolean("CreationKit", "UIDarkTheme", false))
	{
		auto comDll = (uintptr_t)GetModuleHandle("comctl32.dll");
		Assert(comDll);

		EditorUIDarkMode::Initialize();
		Detours::IATHook(comDll, "USER32.dll", "GetSysColor", (uintptr_t)&EditorUIDarkMode::Comctl32GetSysColor);
		Detours::IATHook(comDll, "USER32.dll", "GetSysColorBrush", (uintptr_t)&EditorUIDarkMode::Comctl32GetSysColorBrush);
		Detours::IATDelayedHook(comDll, "UxTheme.dll", "DrawThemeBackground", (uintptr_t)&EditorUIDarkMode::Comctl32DrawThemeBackground);
		Detours::IATDelayedHook(comDll, "UxTheme.dll", "DrawThemeText", (uintptr_t)&EditorUIDarkMode::Comctl32DrawThemeText);
	}

	if (g_INI.GetBoolean("CreationKit", "UI", false))
	{
		EditorUI::Initialize();

		*(uintptr_t *)&EditorUI::OldWndProc = Detours::X64::DetourFunctionClass(OFFSET(0x05B74D0, 0), &EditorUI::WndProc);
		*(uintptr_t *)&EditorUI::OldObjectWindowProc = Detours::X64::DetourFunctionClass(OFFSET(0x03F9020, 0), &EditorUI::ObjectWindowProc);
		*(uintptr_t *)&EditorUI::OldCellViewProc = Detours::X64::DetourFunctionClass(OFFSET(0x059D820, 0), &EditorUI::CellViewProc);
		*(uintptr_t *)&EditorUI::OldResponseWindowProc = Detours::X64::DetourFunctionClass(OFFSET(0x0B5EB50, 0), &EditorUI::ResponseWindowProc);

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
		XUtil::DetourCall(OFFSET(0x5669D8, 0), &EditorUI::hk_0x5669D8);
		XUtil::PatchMemoryNop(OFFSET(0x5669DD, 0), 0x40);

		// Allow forms to be filtered in ObjectWindowProc
		XUtil::DetourCall(OFFSET(0x3FE4CA, 0), &EditorUI::hk_7FF72F57F8F0);
		// Allow forms to be filtered in CellViewProc
		XUtil::DetourCall(OFFSET(0x6435BF, 0), &EditorUI::hk_7FF70C322BC0);

		//
		// Since I'm used to seeing SSE fixes
		//

		// Setting section sizes statusbar
		XUtil::DetourCall(OFFSET(0x5FDFC8, 0), &EditorUI::hk_SetSettingsPartStatusBar);
		// Ban on sending text to section 2 statusbar and send in 3 section
		XUtil::DetourJump(OFFSET(0x5FDFF2, 0), &EditorUI::hk_SetTextPartStatusBar);

		//
		// Replacing the Tips window "Do you know...". Which appears when the plugin is loaded.
		//

		if (g_INI.GetBoolean("CreationKit", "ReplacingTipsWithProgressBar", false))
		{
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
		}
	}

	if (g_INI.GetBoolean("CreationKit", "DisableWindowGhosting", false))
	{
		DisableProcessWindowsGhosting();
	}

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

	//
	// Processing messages during file upload so that the window doesn't hang
	//

	// jump to function for useful work (messages pool)
	XUtil::DetourClassJump(OFFSET(0x2485C46, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	XUtil::DetourClassJump(OFFSET(0x2485E46, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	XUtil::DetourClassJump(OFFSET(0xDF2FBA, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	XUtil::DetourClassJump(OFFSET(0x2001B1B, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	XUtil::DetourClassJump(OFFSET(0x8531BD, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	// Replacing Sleep(1) on (messages pool)
	XUtil::DetourClassCall(OFFSET(0x247EF69, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	XUtil::DetourClassCall(OFFSET(0x5DD8C2, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	XUtil::DetourClassCall(OFFSET(0x5DD7F2, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	// Replacing a completely empty function with something useful (messages pool)
	XUtil::DetourClassCall(OFFSET(0x7E34D2, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);
	XUtil::DetourClassCall(OFFSET(0x773DEE, 0), &Core::Classes::UI::CUIMainWindow::ProcessMessages);

	//
	// Force the render window to draw at 60fps while idle (SetTimer(1ms)). 
	//
	if (g_INI.GetBoolean("CreationKit", "RenderWindowUnlockedFPS", false))
	{
		// SetTimer minimum value 10ms
		// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-settimer
		XUtil::PatchMemory(OFFSET(0x0463383, 0), { (uint8_t)USER_TIMER_MINIMUM });

		// In a separate option VSync
		if (!g_INI.GetBoolean("CreationKit", "VSyncRender", false))
			XUtil::PatchMemory(OFFSET(0x2A39142, 0), { 0x33, 0xD2, 0x90 });
	}

	//
	// Convert Utf-8 to WinCP when loading and back when saving
	//
	if (g_INI.GetBoolean("Experimental", "Unicode", false))
	{
#ifdef __INC_LAZ_UNICODE_PLUGIN
		// Initialization CreationKitUnicodePlugin.dll
		BOOL bRes = XUtil::Conversion::LazUnicodePluginInit();
		if (!bRes)
			LogWindow::Log("Library 'CreationKitUnicodePlugin.dll' no found. Unicode support don't patched.");
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
			PatchIAT(Experimental::hk_SetDlgItemTextA, "User32.dll", "SetDlgItemTextA");
			PatchIAT(Experimental::hk_SendDlgItemMessageA, "User32.dll", "SendDlgItemMessageA");
		}
#else
		LogWindow::Log("Unfortunately, your compiled version does not support the 'Experimental::Unicode' option.");
#endif // __INC_LAZ_UNICODE_PLUGIN
	}

	//
	// Kill broken destructors causing crashes on exit
	//
	XUtil::DetourCall(OFFSET(0x5B6DF7, 0), &QuitHandler);
	XUtil::DetourCall(OFFSET(0x2D48FC0, 0), &QuitHandler);
	XUtil::DetourCall(OFFSET(0x2D48FCF, 0), &QuitHandler);

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
	XUtil::DetourJump(OFFSET(0x07ED840, 0), &ArrayQuickSortRecursive<class TESForm_CK *>);
	XUtil::PatchMemory(OFFSET(0x07EDA50, 0), { 0xC3 });

	//
	// Plugin loading optimizations
	//
	int cpuinfo[4];
	__cpuid(cpuinfo, 1);

	// Utilize SSE4.1 instructions if available
	if ((cpuinfo[2] & (1 << 19)) != 0)
		XUtil::DetourJump(OFFSET(0x05B31C0, 0), &sub_1405B31C0_SSE41);
	else
		XUtil::DetourJump(OFFSET(0x05B31C0, 0), &sub_1405B31C0);

	XUtil::DetourCall(OFFSET(0x08056B7, 0), &hk_inflateInit);
	XUtil::DetourCall(OFFSET(0x08056F7, 0), &hk_inflate);

	// Experimental. Must be run last to avoid interfering with other hooks and patches.
	// Optimization that nuukem did in CK SSE
	// https://github.com/Nukem9/SkyrimSETest/blob/master/skyrim64_test/src/patches/CKSSE/Experimental.cpp
	Experimental::RunOptimizations();
}