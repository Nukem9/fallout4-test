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

#include "../../api/ComClasses.h"
#include "../../common/MemoryManager.h"
#include "../../profiler_internal.h"

#include <shellapi.h>
#include <filesystem>

#pragma warning(disable : 26451; disable : 6053)

#define F4CK_LOADER_FILENAME L"f4ck_loader.exe"

namespace ENBFix {
	std::map<std::wstring, BOOL /*no used*/ > dllENBs = {
	{ L"d3d11.dll", FALSE },
	{ L"d3d10.dll", FALSE },
	{ L"d3d9.dll", FALSE },
	{ L"d3dcompiler_46e.dll", FALSE },
	{ L"dxgi.dll", FALSE },
	{ L"enbimgui.dll", FALSE }
	};

	static BOOL is_enabled = FALSE;
	static std::wstring app_path; 
	static LPBYTE image_base = NULL;
	static DWORD image_size, image_crc32 = 0;

	inline BOOL FIXAPI FileExists(const std::wstring fname) {
		return std::filesystem::exists(app_path + fname);
	}

	DWORD FIXAPI GetCRC32File(const std::wstring fname) {
		if (FileExists(fname)) {
			HANDLE file = CreateFileW((app_path + fname).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (!file)
				return 0;

			DWORD file_size = GetFileSize(file, NULL);
			LPBYTE file_data = (LPBYTE)scalable_aligned_malloc(file_size, 16);
			if (!file_data) {
				CloseHandle(file);
				return 0;
			}

			DWORD m;
			if (!ReadFile(file, file_data, file_size, &m, NULL)) {
				scalable_aligned_free(file_data);
				CloseHandle(file);
				return 0;
			}

			CloseHandle(file);

			DWORD crc32 = (DWORD)CRC32Buffer((void*)file_data, file_size);

			scalable_aligned_free(file_data);
			return crc32;
		}

		return 0;
	}

	BOOL FIXAPI InstallLoader(const std::wstring fname) {
		if (!image_base)
			return FALSE;

		HANDLE file = CreateFileW((app_path + fname).c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (!file)
			return FALSE;

		DWORD m;
		BOOL bResult = WriteFile(file, image_base, image_size, &m, NULL) != 0;

		CloseHandle(file);
		return bResult;
	}

	DWORD FIXAPI GetResource(HMODULE module, LPCSTR name, LPCSTR type, LPBYTE& dest, DWORD& size)
	{
		LPBYTE p = NULL;
		HRSRC resource;
		HGLOBAL resource_handle;
		
		if (resource = FindResourceA(module, name, type); !resource)
			return 1;

		if (resource_handle = LoadResource(module, resource); !resource_handle)
			return 2;

		if (size = SizeofResource(module, resource); !dest)
			return 0;

		if (memcpy_s(dest, size, LockResource(resource_handle), size))
			return 3;

		return 0;
	}

	VOID FIXAPI ReleaseFix(VOID) {
		if (image_base) {
			scalable_aligned_free(image_base);

			image_base = NULL;
			image_size = 0;
			image_crc32 = 0;
		}
	}

	BOOL FIXAPI InitFix(VOID) {
		WCHAR basePath[MAX_PATH] = L"";
		GetModuleFileNameW(g_hModule, basePath, MAX_PATH);
		std::filesystem::path p(basePath);
		app_path = p.parent_path();
		app_path += L"\\";

		for each (auto it in dllENBs) {
			is_enabled = FileExists(it.first);
			if (is_enabled)
				break;
		}

		DWORD dwResult = GetResource(g_hModule, MAKEINTRESOURCEA(IDR_BINARY_F4CKLOADER), "BINARY", image_base, image_size);
		if (dwResult || !image_size)
			return FALSE;

		image_base = (LPBYTE)scalable_aligned_malloc(image_size, 16);
		if (!image_base)
			return FALSE;

		if (GetResource(g_hModule, MAKEINTRESOURCEA(IDR_BINARY_F4CKLOADER), "BINARY", image_base, image_size)) {
			ReleaseFix();
			return FALSE;
		}

		image_crc32 = (DWORD)CRC32Buffer((void*)image_base, image_size);

		return TRUE;
	}

	inline BOOL FIXAPI IsENBSeriesEnabled(VOID) {
		return is_enabled;
	}
}

VOID FIXAPI ENBSeriesFixableRunHandler(VOID) {
	BOOL bResult = ENBFix::InitFix();

	if (!ENBFix::IsENBSeriesEnabled()) {
		ENBFix::ReleaseFix();
		return;
	}

	if (!ENBFix::FileExists(F4CK_LOADER_FILENAME)) {
		if (MessageBoxW(
			0,
			L"The ENBSeries mod is installed, Creation Kit cannot work with this mod, and it will install the loader in the root directory of the game.\n"
			L"Install the loader or close the Creation Kit?.",
			L"Question",
			MB_YESNO | MB_ICONQUESTION) == IDNO) {
			ENBFix::ReleaseFix();
			ExitProcess(0);
		}

	l_no_installed_loader:
		if (!bResult) {
			MessageBoxW(
				0,
				L"Failed to initialize ENB fixes, resource not found or failed to load.\n"
				L"Delete the ENB files yourself.\n"
				L"Contact the developer of \"F4 Creation Kit Fixes\".",
				L"Error",
				MB_OK | MB_ICONERROR);

			ENBFix::ReleaseFix();
			ExitProcess(0);
		}

		if (!ENBFix::InstallLoader(F4CK_LOADER_FILENAME)) {
			MessageBoxW(
				0,
				L"Couldn't save to the root game directory \""
				F4CK_LOADER_FILENAME
				L"\".\n"
				L"Check if there are permissions for changes files in the root game directory.\n"
				L"Or delete the ENB files yourself.",
				L"Error",
				MB_OK | MB_ICONERROR);

			ENBFix::ReleaseFix();
			ExitProcess(0);
		}

		goto l_installed_loader;
	}
	else if (ENBFix::GetCRC32File(F4CK_LOADER_FILENAME) != ENBFix::image_crc32) {

		// I'm already sick of this message, I'm afraid to imagine how it will get users, 
		// of course, who are frequently updated, I'll cut out
		/*
		MessageBoxW(
			0,
			L"Are you using an outdated \""
			F4CK_LOADER_FILENAME
			"\" and the Creation Kit will make an update.",
			L"Information",
			MB_OK | MB_ICONINFORMATION);
		*/

		goto l_no_installed_loader;
	}
	else {
	l_installed_loader:
		ENBFix::ReleaseFix();

		WCHAR szCommand[MAX_PATH + 1] = { 0 };
		_snwprintf(szCommand, MAX_PATH, L"pid:%d", GetCurrentProcessId());
		if ((INT_PTR)ShellExecuteW(NULL, L"open", F4CK_LOADER_FILENAME, szCommand, ENBFix::app_path.c_str(), SW_HIDE) <= 0x20) {
			MessageBoxW(
				0,
				L"Failed to start the loader.\n"
				L"Delete the ENB files yourself.\n"
				L"Contact the developer of \"F4 Creation Kit Fixes\".",
				L"Error",
				MB_OK | MB_ICONERROR);
			ExitProcess(0);
		}
	}
}

#pragma warning(default : 26451; default : 6053)