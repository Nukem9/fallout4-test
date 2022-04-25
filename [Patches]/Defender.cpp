//////////////////////////////////////////
/*
* Copyright (c) 2022 Perchik71 <email:perchik71@outlook.com>
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
#include "Defender.h"

#pragma comment(lib, "Wintrust.lib")

#include <tlhelp32.h>
#include <wintrust.h>
#include <SoftPub.h>

LPSTR* __DlAllowedPathForDllLoader = NULL;
SIZE_T __DlAllowedPathForDllLoaderCount = 0;

decltype(LoadLibraryA)* __DlLoadLibraryA;
decltype(LoadLibraryW)* __DlLoadLibraryW;
decltype(LoadLibraryExA)* __DlLoadLibraryExA;
decltype(LoadLibraryExW)* __DlLoadLibraryExW;

DlImageProcessWorker::DlImageProcessWorker(UINT_PTR base):
	Base(base)
{}

PIMAGE_IMPORT_DESCRIPTOR DlImageProcessWorker::PiidFromDllName(LPCSTR szDllName) const {
	PIMAGE_IMPORT_DESCRIPTOR piidRet = NULL;
	auto pinh = PinhFromImageBase(HMODULE(Base));

	auto len = MemoryWorker.StrLen(szDllName);
	if (!len)
		return piidRet;

	auto sectionImportRVA = pinh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	auto sectionImportSize = pinh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

	// Scan the import for the dll in question
	//
	if (sectionImportSize != 0) {
		PIMAGE_IMPORT_DESCRIPTOR piid = PFromRva<PIMAGE_IMPORT_DESCRIPTOR>(sectionImportRVA);

		// Check all of the dlls listed up to the NULL one.
		//
		for (; piid->Name != NULL; piid++) {
			// Check to see if it is the DLL we want
			// Intentionally case sensitive to avoid complication of using the CRT
			// for those that don't use the CRT...the user can replace this with
			// a variant of a case insensitive comparison routine.
			//
			LPCSTR libName = PFromRva<LPCSTR>(piid->Name);
		
			SIZE_T cchDllCur = MemoryWorker.StrLen(libName);
			if (cchDllCur == len && MemoryWorker.StrICmp(szDllName, libName, cchDllCur) == 0) {
				piidRet = piid;
				break;
			}
		}
	}

	return piidRet;
}

PIMAGE_EXPORT_DIRECTORY DlImageProcessWorker::PiedFromDllHandle(HMODULE handle) const {
	auto pinh = PinhFromImageBase(handle);

	auto sectionExportRVA = pinh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	auto sectionExportSize = pinh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	if (!sectionExportSize)
		return NULL;

	return PFromRva<PIMAGE_EXPORT_DIRECTORY>(sectionExportRVA);
}

DlImageProcessWorker::EDllLoadedFlag DlImageProcessWorker::GetDllLoadedFlag(LPCSTR szDllName) const {
	EDllLoadedFlag dlfRet = dlfLoadedFailed;
	
	CHAR szBuffer[MAX_PATH];
	CHAR szPath[MAX_PATH];

	auto hProcess = GetCurrentProcess();
	if(!hProcess)
		return dlfRet;

	if (!GetSystemDirectoryA(szPath, MAX_PATH))
		return dlfRet;

	auto moduleHandle = GetModuleHandleA(szDllName);
	if (!moduleHandle)
		return dlfLoadedNoLoaded;

	if (!GetModuleFileNameExA(hProcess, moduleHandle, szBuffer, MAX_PATH))
		return dlfRet;

	auto len1 = RFindDelimiter(szPath);
	auto len2 = RFindDelimiter(szBuffer);

	if (len2 == len1) {
		// no system dir
no_sysdir:
		if (!GetModuleFileNameA(GetModuleHandleA(NULL), szPath, MAX_PATH))
			return dlfRet;

		len1 = RFindDelimiter(szPath);

		if (len1 == len2 && !MemoryWorker.StrICmp(szPath, szBuffer, len1)) {
			dlfRet = dlfLoadedFromApplicationDirectory;
			goto Ret;
		}
		
		if (__DlAllowedPathForDllLoader && __DlAllowedPathForDllLoaderCount > 0) {
			for (SIZE_T i = 0; i < __DlAllowedPathForDllLoaderCount; i++) {
				auto path = __DlAllowedPathForDllLoader[i];
				len1 = RFindDelimiter(path);

				if (len1 == len2 && !MemoryWorker.StrICmp(szPath, szBuffer, len1)) {
					dlfRet = dlfLoadedFromAllowedDirectory;
					goto Ret;
				}
			}
		}
		
		dlfRet = dlfLoadedFromOtherDirectory;
	}
	else {
		if (!MemoryWorker.StrICmp(szPath, szBuffer, len1))
			dlfRet = dlfLoadedFromSystemDirectory;
		else
			goto no_sysdir;
	}

Ret:
	return dlfRet;
}

VOID DlCheckModuleProcess::AnalyzeAndRecoveryVTable(VOID) {
	ImageProcessWorker.SetBase(g_ModuleBase);

	static LPSTR szDllForCheck[] = {
		"advapi32.dll",
		"bcrypt.dll",
		"cfgmgr32.dll",
		"combase.dll",
		"comdlg32.dll",
		"d3d11.dll",
		"d3d9.dll",
		"dbgcore.dll",
		"dbghelp.dll",
		"dwmapi.dll",
		"dxgi.dll",
		"gdi32.dll",
		"gdi32full.dll",
		"hid.dll",
		"iphlpapi.dll",
		"msimg32.dll",
		"msvcp140.dll",
		"msvcp_win.dll",
		"msvcrt.dll",
		"ole32.dll",
		"oleaut32.dll",
		"rpcrt4.dll",
		"sechost.dll",
		"setupapi.dll",
		"shcore.dll",
		"shell32.dll",
		"shlwapi.dll",
		"ucrtbase.dll",
		"user32.dll",
		"uxtheme.dll",
		"vcruntime140.dll",
		"vcruntime140_1.dll",
		"version.dll",
		"win32u.dll",
		"windows.storage.dll",
		"wininet.dll",
		"winmm.dll",
		"wldap32.dll",
		"wldp.dll",
		"ws2_32.dll",
		"x3daudio1_7.dll",
		"xinput1_3.dll",
	};

	// This set of libraries must necessarily be from the system directory and nowhere else.
	// These libraries need to be downloaded from the system directory and try to switch vtable.

	for (auto moduleName : szDllForCheck) {
		if (ImageProcessWorker.GetDllLoadedFlag(moduleName) != DlImageProcessWorker::dlfLoadedFromSystemDirectory)
			DlCheckModuleProcess::Recovery(moduleName, GetModuleHandleA(moduleName));
	}
}

DlCheckModuleProcess::EDllRecoveredFlag DlCheckModuleProcess::Recovery(LPCSTR szDllName, HMODULE hmod) {
	CHAR szPath[MAX_PATH];
	if (!GetSystemDirectoryA(szPath, MAX_PATH))
		return drfRecoveredFailed;

	auto moduleNewHandle = LoadLibraryExA(szDllName, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (!moduleNewHandle)
		return drfRecoveredNoModule;

	MODULEENTRY32 modEntry;

	ImageProcessWorker.SetBase(g_ModuleBase);
	ImageProcessWorker2.SetBase((UINT_PTR)moduleNewHandle);

	auto pExportTable = ImageProcessWorker2.PiedFromDllHandle(moduleNewHandle);
	PULONG pAddressOfNames = ImageProcessWorker2.PFromRva<PULONG>(pExportTable->AddressOfNames);
	PULONG pAddressOfFunctions = ImageProcessWorker2.PFromRva<PULONG>(pExportTable->AddressOfFunctions);

	auto piid = ImageProcessWorker.PiidFromDllName(szDllName);
	if (piid)
		goto found;
other_module:
	auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if (!hSnap)
		return drfRecoveredFailed;

	modEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnap, &modEntry)) {
		do {
			if ((uintptr_t)modEntry.hModule == g_ModuleBase || modEntry.hModule == moduleNewHandle ||
				modEntry.hModule == g_hModule || modEntry.hModule == hmod)
				continue;

			ImageProcessWorker.SetBase((UINT_PTR)modEntry.hModule);

			auto pinhOther = ImageProcessWorker.PinhFromImageBase((HMODULE)g_ModuleBase);
			auto importOtherRVA = pinhOther->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
			auto importOtherSize = pinhOther->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

			if (!importOtherSize)
				continue;

			piid = ImageProcessWorker.PiidFromDllName(szDllName);
			if (!piid)
				continue;

			DWORD dwOtherProtection;
			VirtualProtect(ImageProcessWorker.PFromRva<LPVOID>(importOtherRVA), importOtherSize, PAGE_READWRITE, &dwOtherProtection);

			//Import Lookup Table address (functions names)
			auto nameOtherAddressPtr = ImageProcessWorker.PFromRva<PIMAGE_THUNK_DATA>(piid->OriginalFirstThunk);
			//Import Address Table (IAT) address (functions addresses)
			auto functionOtherAddressPtr = ImageProcessWorker.PFromRva<PIMAGE_THUNK_DATA>(piid->FirstThunk);

			for (; nameOtherAddressPtr->u1.Function; nameOtherAddressPtr++, functionOtherAddressPtr++) {
				auto nameFunction = ImageProcessWorker.PFromRva<PIMAGE_IMPORT_BY_NAME>(nameOtherAddressPtr->u1.AddressOfData);

				for (DWORD i = 0; i < ImageProcessWorker2.CountOfExports(pExportTable); i++) {
					auto nameFunc = ImageProcessWorker2.PFromRva<LPCSTR>(pAddressOfNames[i]);
					if (!MemoryWorker.StrICmp(nameFunction->Name, nameFunc, MemoryWorker.StrLen(nameFunc))) {
						//_MESSAGE_FMT("%s 0x%x (0x%x)", nameFunction->Name, (DWORD_PTR)((LPBYTE)moduleNewHandle + rva), rva);
						functionOtherAddressPtr->u1.Function = (DWORD_PTR)((LPBYTE)moduleNewHandle + pAddressOfFunctions[i]);
						break;
					}
				}
			}

			VirtualProtect(ImageProcessWorker.PFromRva<LPVOID>(importOtherRVA), importOtherSize, dwOtherProtection, &dwOtherProtection);
		} while (Module32Next(hSnap, &modEntry));
	}

	CloseHandle(hSnap);

	return DlCheckModuleProcess::drfRecoveredOk;
found:
	auto pinh = ImageProcessWorker.PinhFromImageBase((HMODULE)g_ModuleBase);
	auto importProcessRVA = pinh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	auto importProcessSize = pinh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

	DWORD dwProtection;
	VirtualProtect(ImageProcessWorker.PFromRva<LPVOID>(importProcessRVA), importProcessSize, PAGE_READWRITE, &dwProtection);

	//Import Lookup Table address (functions names)
	auto nameAddressPtr = ImageProcessWorker.PFromRva<PIMAGE_THUNK_DATA>(piid->OriginalFirstThunk);
	//Import Address Table (IAT) address (functions addresses)
	auto functionAddressPtr = ImageProcessWorker.PFromRva<PIMAGE_THUNK_DATA>(piid->FirstThunk);

	for (; nameAddressPtr->u1.Function; nameAddressPtr++, functionAddressPtr++) {
		auto nameFunction = ImageProcessWorker.PFromRva<PIMAGE_IMPORT_BY_NAME>(nameAddressPtr->u1.AddressOfData);

		for (DWORD i = 0; i < ImageProcessWorker2.CountOfExports(pExportTable); i++) {
			auto nameFunc = ImageProcessWorker2.PFromRva<LPCSTR>(pAddressOfNames[i]);
			if (!MemoryWorker.StrICmp(nameFunction->Name, nameFunc, MemoryWorker.StrLen(nameFunc))) {
				//_MESSAGE_FMT("%s 0x%x (0x%x)", nameFunction->Name, (DWORD_PTR)((LPBYTE)moduleNewHandle + rva), rva);
				functionAddressPtr->u1.Function = (DWORD_PTR)((LPBYTE)moduleNewHandle + pAddressOfFunctions[i]);
				break;
			}
		}
	}

	VirtualProtect(ImageProcessWorker.PFromRva<LPVOID>(importProcessRVA), importProcessSize, dwProtection, &dwProtection);

	goto other_module;
}

BOOL FIXAPI DlLoadModule::VerifyFileA(LPCSTR fileName) {
	if (!fileName)
		return FALSE;

	LPWSTR tempChar;
	INT tempCharLength;

	tempCharLength = MultiByteToWideChar(CP_ACP, 0, fileName, -1, NULL, 0);
	tempChar = new WCHAR[tempCharLength + 1];
	if (!tempChar)
		return FALSE;

	MultiByteToWideChar(CP_ACP, 0, fileName, -1, tempChar, tempCharLength);
	tempChar[tempCharLength] = L'\0';

	auto result = VerifyFileW(tempChar);
	delete[] tempChar;

	return result;
}

BOOL FIXAPI DlLoadModule::VerifyFileW(LPCWSTR fileName) {
	if (!fileName)
		return FALSE;

	WINTRUST_FILE_INFO FileData;
	memset(&FileData, 0, sizeof(FileData));
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.pcwszFilePath = fileName;
	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

	WINTRUST_DATA WinTrustData;
	memset(&WinTrustData, 0, sizeof(WinTrustData));
	WinTrustData.cbStruct = sizeof(WinTrustData);
	WinTrustData.dwUIChoice = WTD_UI_NONE;
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;

	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
	WinTrustData.dwProvFlags = WTD_SAFER_FLAG;
	WinTrustData.pFile = &FileData;

	LONG lStatus = WinVerifyTrust((HWND)INVALID_HANDLE_VALUE, &WVTPolicyGUID, &WinTrustData);
	if (lStatus == TRUST_E_NOSIGNATURE)
		// Microsoft say ** DON'T USE ** XD
		lStatus = WinVerifyTrustEx((HWND)INVALID_HANDLE_VALUE, &WVTPolicyGUID, &WinTrustData);

	return lStatus == ERROR_SUCCESS;
}

HMODULE WINAPI DlLoadModule::LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
	if (!lpLibFileName)
		return FALSE;

	LPWSTR tempChar;
	INT tempCharLength;

	tempCharLength = MultiByteToWideChar(CP_ACP, 0, lpLibFileName, -1, NULL, 0);
	tempChar = new WCHAR[tempCharLength + 1];
	if (!tempChar)
		return FALSE;

	MultiByteToWideChar(CP_ACP, 0, lpLibFileName, -1, tempChar, tempCharLength);
	tempChar[tempCharLength] = L'\0';

	auto result = LoadLibraryExW(tempChar, hFile, dwFlags);
	delete[] tempChar;

	return result;
}

HMODULE WINAPI DlLoadModule::LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
	if (!lpLibFileName)
		return NULL;

	WCHAR szSystemPath[MAX_PATH];
	AssertMsg(GetSystemDirectoryW(szSystemPath, MAX_PATH), "FAILED: No system dir ???");

	static LPWSTR arrayAllowed[] = {
		L"CreationKitUnicodePlugin.dll",
		L"bink2w64.dll",
		L"cudart64_75.dll",
		L"GFSDK_SSAO_D3D11.win64.dll",
		L"GFSDK_GodraysLib.x64.dll",
		L"flowchartx64.dll",
		L"flexRelease_x64.dll",
		L"flexExtRelease_x64.dll",
		L"ssce5564.dll",
		L"nvToolsExt64_1.dll",
	};

	// Set dir for dll
	SetDllDirectoryW(szSystemPath);
	AddDllDirectory((XUtil::GetDataPathUnicode() + L"F4CKFixes\\bin\\").c_str());
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_USER_DIRS);

	std::wstring fileName = lpLibFileName;
	auto del = fileName.find_last_of(L'\\');
	if (del == fileName.npos) {
		for (auto allow : arrayAllowed) {
			if (!_wcsicmp(allow, lpLibFileName))
				return __DlLoadLibraryExW((XUtil::GetAppPathUnicode() + lpLibFileName).c_str(),
					NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		}

		std::wstring filePath = szSystemPath;
		filePath += '\\';
		filePath += fileName;

		auto result = __DlLoadLibraryExW(filePath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!result)
			return __DlLoadLibraryExW(lpLibFileName, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_USER_DIRS);

		if (!result) {
			fileName = XUtil::GetAppPathUnicode() + lpLibFileName;

			if (VerifyFileW(fileName.c_str()))
				return __DlLoadLibraryExW(fileName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			_MESSAGEW_FMT(L"WARNING: Attempt to load module '%s'. Skip.", lpLibFileName);
			return NULL;
		}

		return result;
	}
	else {
		auto shortName = fileName.substr(del + 1);
		for (auto allow : arrayAllowed) {
			if (!_wcsicmp(allow, shortName.c_str()))
				return __DlLoadLibraryExW(fileName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		}

		std::wstring filePath = szSystemPath;
		filePath += '\\';
		filePath += shortName;

		auto result = __DlLoadLibraryExW(filePath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!result)
			result = __DlLoadLibraryExW(shortName.c_str(), NULL, LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_USER_DIRS);

		if (!result) {
			if (VerifyFileW(fileName.c_str()))
				return __DlLoadLibraryExW(fileName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

			_MESSAGEW_FMT(L"WARNING: Attempt to load module '%s'. Skip.", shortName.c_str());
			return NULL;
		}

		return result;
	}
}

HMODULE WINAPI DlLoadModule::LoadLibraryA(LPCSTR lpLibFileName) {
	return LoadLibraryExA(lpLibFileName, NULL, 0);
}

HMODULE WINAPI DlLoadModule::LoadLibraryW(LPCWSTR lpLibFileName) {
	return LoadLibraryExW(lpLibFileName, NULL, 0);
}

DlLoadModule::DlLoadModule(VOID) {
	// I changed my mind to complicate, the main thing is to intercept some calls, 
	// in the process itself, if it is injected, then the user does it intentionally.

	// The main thing is to protect the process from loading the dll at runtime, for ENBSeries or any other libraries.
	// CKFixes should have already done the restoration VTable.

	auto moduleKernel32 = GetModuleHandleA("kernel32.dll");

	__DlLoadLibraryA = (decltype(::LoadLibraryA)*)GetProcAddress(moduleKernel32, "LoadLibraryA");
	__DlLoadLibraryW = (decltype(::LoadLibraryW)*)GetProcAddress(moduleKernel32, "LoadLibraryW");
	__DlLoadLibraryExA = (decltype(::LoadLibraryExA)*)GetProcAddress(moduleKernel32, "LoadLibraryExA");
	__DlLoadLibraryExW = (decltype(::LoadLibraryExW)*)GetProcAddress(moduleKernel32, "LoadLibraryExW");

	PatchIAT(LoadLibraryA, "kernel32.dll", "LoadLibraryA");
	PatchIAT(LoadLibraryW, "kernel32.dll", "LoadLibraryW");
	PatchIAT(LoadLibraryExA, "kernel32.dll", "LoadLibraryExA");
	PatchIAT(LoadLibraryExW, "kernel32.dll", "LoadLibraryExW");
}