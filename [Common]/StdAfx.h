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

#include "..\[Common]\CommInc.h"

#define SETTING_CREATIONKIT			"CreationKit.ini"
#define SETTING_CREATIONKIT_CUSTOM	"CreationKitCustom.ini"
#define SETTING_CREATIONKIT_PREFS	"CreationKitPrefs.ini"
#define SETTING_CREATIONKIT_FIXES	"fallout4_test.ini"

// jDialogs
#define JDIALOG_NO_FUNCTIONS_CREATE_CONTROLS
#define JDIALOG_NO_MANIFEST_LINKER_COMMCTRL
#include "..\[Depends]\jDialogs\include\jdialogs.h"

// Zipper
#include "..\[Depends]\zipper\src\zip.h"

// Memory
#include "IMemory.h"
#include "IMemoryManager.h"

// String
#include "..\[EditorAPI]\BSString.h"

// INIReader
#if FALLOUT4_MINI_PROJECT
#include "INIReader.h"

extern mINIReader* g_INI;
extern mINIReader* g_INI_CK;
extern mINIReader* g_INI_CK_Cfg;
extern mINIReader* g_INI_CK_CfgCustom;
#else
#define INI_ALLOW_MULTILINE 0
#define INI_USE_STACK 0
#define INI_MAX_LINE 4096
#include "INIReader.h"

extern INIReader* g_INI;
extern INIReader* g_INI_CK_Cfg;
extern INIReader* g_INI_CK_CfgCustom;
#endif // FALLOUT4_MINI_PROJECT

#define F4CK_PATH_BIN		"bin\\"
#define F4CK_PATH_DIALOGS	"dia\\"
#define F4CK_PATH_FONTS		"fonts\\"

// OTHER
#include "UtfStr.h"
#include "ProfilerInternal.h"
#include "Utilities.h"
#include "Offsets.h"
#include "IProcessMessage.h"
#include "IProgressTaskBar.h"

// DirectX
#include "D3D11Proxy.h"

// RTTI
#include "MSRTTI.h"

// LogWindow (ONLY CK)
#include "..\[Patches]\CKF4\LogWindow.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;

extern HINSTANCE g_hModule;

extern uintptr_t g_ModuleBase;
extern uintptr_t g_ModuleSize;

extern uintptr_t g_ScrapSize;
extern uintptr_t g_bhkMemSize;

extern uintptr_t g_CodeBase;	// .text or .textbss
extern uintptr_t g_CodeEnd;
extern uintptr_t g_RdataBase;	// .rdata
extern uintptr_t g_RdataEnd;
extern uintptr_t g_DataBase;	// .data
extern uintptr_t g_DataEnd;

extern HMODULE g_DllDXGI;
extern HMODULE g_DllD3D11;

extern int8_t g_i8DialogMode;
extern int8_t g_i8NeedLoadBA2;
extern BOOL g_AttachRenderWnd;
extern BOOL g_HotkeyReplace;

enum class GAME_EXECUTABLE_TYPE {
	UNKNOWN,
	LAUNCHER_FALLOUT4,
	GAME_FALLOUT4,
	CREATIONKIT_FALLOUT4,
	CREATIONKIT_FALLOUT4_PATCHED_PREVIS,
};

struct __addr_t
{
	uintptr_t Based;
	uintptr_t End;
};

struct __hotkey_t {
	CHAR VirtualKey;
	BOOL Alt, Ctrl, Shift;
};

struct range_t
{
	__addr_t addr;
	DWORD protection;
};

extern GAME_EXECUTABLE_TYPE g_LoadType;
extern char g_GitVersion[64];
extern char g_GitDataVersion[64];