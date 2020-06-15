#include "../common.h"
#include <libdeflate/libdeflate.h>
#include <intrin.h>
#include "TES/MemoryManager.h"
#include "TES/bhkThreadMemorySource.h"
#include "CKF4/Editor.h"
#include "CKF4/EditorUI.h"
#include "CKF4/EditorUIDarkMode.h"
#include "CKF4/LogWindow.h"

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
	}

	if (g_INI.GetBoolean("CreationKit", "DisableWindowGhosting", false))
	{
		DisableProcessWindowsGhosting();
	}

	// Disable useless "Processing Topic X..." status bar updates
	XUtil::PatchMemoryNop(OFFSET(0xB89897, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0xB89BF3, 0), 5);
	XUtil::PatchMemoryNop(OFFSET(0xB8A472, 0), 5);

	//
	// Force the render window to draw at 60fps while idle (SetTimer(1ms)). This also disables vsync.
	//
	if (g_INI.GetBoolean("CreationKit", "RenderWindowUnlockedFPS", false))
	{
		XUtil::PatchMemory(OFFSET(0x0463383, 0), { 0x01 });
		XUtil::PatchMemory(OFFSET(0x2A39142, 0), { 0x33, 0xD2, 0x90 });
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
}