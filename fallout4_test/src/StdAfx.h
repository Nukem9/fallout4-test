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

// 4018 - signed/unsigned mismatch
// 4244 - loss of data by assignment
// 4267 - possible loss of data (truncation)
// 4305 - truncation by assignment
// 4288 - disable warning for crap Microsoft extension screwing up the scope of variables defined in for loops
// 4311 - pointer truncation
// 4312 - pointer extension
#pragma warning(disable: 4018)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 4305)
#pragma warning(disable: 4288)
#pragma warning(disable: 4312)
#pragma warning(disable: 4311)
#pragma warning(disable: 4334)
#pragma warning(disable: 4800)
#pragma warning(disable: 6053)
#pragma warning(disable: 6067)
#pragma warning(disable: 6386)
#pragma warning(disable: 26812)
#pragma warning(disable: 28183)
#pragma warning(disable: 26451)

#define UI_CUSTOM_MESSAGE	52000
#define __NOMINMAX

#ifndef FIXAPI
#define FIXAPI __stdcall
#endif

#include "../resource.h"
#include "Config.h"
#include "EraseIf.h"

// WIN32
#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <atltypes.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <shlobj.h>
#include <Rpc.h>
#include <Dbghelp.h>
#include <uxtheme.h>
#include <Objbase.h>
#include <Psapi.h>
#include <shellapi.h>
#include <ShObjIdl_core.h>
#include <Uxtheme.h>
#include <vssym32.h>

// CRT
#include <time.h>
#include <intrin.h>
#include <smmintrin.h>
#include <errno.h>
#include <crtdefs.h>
#include <stdio.h>
#include <limits.h>

// STL
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <wchar.h>
#include <time.h>
#include <algorithm>
#include <chrono>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <future>
#include <set>
#include <queue>
#include <stack>
#include <string>
#include <fstream>
#include <stdexcept>
#include <exception>
#include <sstream>
#include <memory>
#include <iostream>
#include <complex>
#include <iomanip>
#include <numeric>
#include <filesystem>
#include <algorithm>
#include <functional>

// DIRECTX
#include <d3d11.h>
#include <d3d11_2.h>

// TBBMalloc
#include <tbb/scalable_allocator.h>
#include <tbb/concurrent_unordered_map.h>
#define TBB_PREVIEW_CONCURRENT_ORDERED_CONTAINERS 1
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_map.h>

// jDialogs
#define JDIALOG_NO_FUNCTIONS_CREATE_CONTROLS
#define JDIALOG_NO_MANIFEST_LINKER_COMMCTRL
#include "..\..\Dependencies\jDialogs\include\jdialogs.h"

// Zipper
#include "..\..\Dependencies\zipper\src\zip.h"

// Detours
#include <detours/Detours.h>

// ZLib
#include <libdeflate/libdeflate.h>

// Zydis
#include <zydis\Zydis.h>

// XByak
#include <xbyak/xbyak.h>

// Common
#include "common/MemoryManager.h"

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

// OTHER
#include "UtfStr.h"
#include "ProfilerInternal.h"
#include "Utilities.h"
#include "Offsets.h"

// LogWindow (ONLY CK)
#include "patches/CKF4/LogWindow.h"

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

extern int8_t g_i8DialogMode;

enum class GAME_EXECUTABLE_TYPE {
	UNKNOWN,
	LAUNCHER_FALLOUT4,
	GAME_FALLOUT4,
	CREATIONKIT_FALLOUT4,
};

extern GAME_EXECUTABLE_TYPE g_LoadType;
extern char g_GitVersion[64];