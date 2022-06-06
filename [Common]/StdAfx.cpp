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

#include "StdAfx.h"

#pragma comment(lib, "delayimp.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "libdeflate.lib")			// Boost deflate
#pragma comment(lib, "tbb.lib")					// Thread Building Blocks
#pragma comment(lib, "libzydis.lib")			// Zydis
#pragma comment(lib, "libmeshoptimizer.lib")	// Meshoptimizer
#pragma comment(lib, "detours.lib")				// Detours
#pragma comment(lib, "tbbmalloc.lib")			// TBB Allocator

HINSTANCE g_hModule;

#if FALLOUT4_MINI_PROJECT
mINIReader* g_INI;
mINIReader* g_INI_CK;
mINIReader* g_INI_CK_Cfg;
mINIReader* g_INI_CK_CfgCustom;
#else
INIReader* g_INI;
INIReader* g_INI_CK_Cfg;
INIReader* g_INI_CK_CfgCustom;
#endif // FALLOUT4_MINI_PROJECT

uintptr_t g_ModuleBase;
uintptr_t g_ModuleSize;

uintptr_t g_ScrapSize = 0x2000000;
uintptr_t g_bhkMemSize = 0x2000000;

uintptr_t g_CodeBase;	// .text or .textbss
uintptr_t g_CodeEnd;
uintptr_t g_RdataBase;	// .rdata
uintptr_t g_RdataEnd;
uintptr_t g_DataBase;	// .data
uintptr_t g_DataEnd;

HMODULE g_DllDXGI;
HMODULE g_DllD3D11;

int8_t g_i8DialogMode = 0;
int8_t g_i8NeedLoadBA2 = 0;
BOOL g_AttachRenderWnd = 0;

GAME_EXECUTABLE_TYPE g_LoadType;
char g_GitVersion[64] = VER_CURRENT_COMMIT_ID;
char g_GitDataVersion[64] = VER_CURRENT_DATE;