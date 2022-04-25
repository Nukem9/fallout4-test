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
#pragma warning(disable: 4229)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 4305)
#pragma warning(disable: 4288)
#pragma warning(disable: 4312)
#pragma warning(disable: 4311)
#pragma warning(disable: 4334)
#pragma warning(disable: 4800)
#pragma warning(disable: 6053)
#pragma warning(disable: 6269)
#pragma warning(disable: 6066)
#pragma warning(disable: 6067)
#pragma warning(disable: 6387)
#pragma warning(disable: 6386)
#pragma warning(disable: 26812)
#pragma warning(disable: 28183)
#pragma warning(disable: 26451)

#ifndef UI_CUSTOM_MESSAGE
#define UI_CUSTOM_MESSAGE	52000
#endif // UI_CUSTOM_MESSAGE

#ifndef __NOMINMAX
#define __NOMINMAX
#endif // __NOMINMAX

#ifndef FIXAPI
#define FIXAPI __stdcall
#endif

#ifndef RAD2DEG
#define RAD2DEG (57.295779513082320876798154814105)
#endif // RAD2DEG

#ifndef DEG2RAD
#define DEG2RAD (0.01745329251994329576923690768489)
#endif // DEG2RAD

#include "../version_info.h"

// WIN64
#if defined(_WINDOWS) && defined(_M_X64) && (_MSC_VER > 1900)

// For use WinSDK 11
// (Unexpected work is possible)
#ifndef WINDOWS_IGNORE_PACKING_MISMATCH
#define WINDOWS_IGNORE_PACKING_MISMATCH
#endif // !WINDOWS_IGNORE_PACKING_MISMATCH

#include "../resource.h"
#include "../[Win64]/Config.h"
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
#include <processthreadsapi.h>
#else
#error Only WINDOWS
#endif

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
#if (_MSC_VER > 1910)
#include <filesystem>
#else
#error Only Visual Studio 2017 or higher
#endif
#include <algorithm>
#include <functional>
#include "EraseIf.h"

// DIRECTX
#include <d3d11.h>
#include <d3d11_2.h>

// TBBMalloc
#define TBB_PREVIEW_CONCURRENT_ORDERED_CONTAINERS 1
#include <tbb/scalable_allocator.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_map.h>
#include <tbb/concurrent_set.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_set.h>

// Detours
#include <detours/Detours.h>

// ZLib
#include <libdeflate/libdeflate.h>

// Zydis
#include <zydis\Zydis.h>

// XByak
#include <xbyak/xbyak.h>
