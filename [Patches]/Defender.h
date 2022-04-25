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

#pragma once

#include "..\[Common]\StdAfx.h"

class DlMemoryWorker {
public:
	__forceinline SIZE_T StrLen(LPCSTR szStr) const {
		LPCSTR szEnd = szStr;
		while (*szEnd++) {}
		return szEnd - szStr - 1;
	}

	__forceinline INT StrICmp(LPCVOID pv1, LPCVOID pv2, SIZE_T cb) const {
		if (!cb)
			return 0;

		while (--cb && (*(LPSTR)pv1 == *(LPSTR)pv2 || (*(PCHAR)pv1 - *(PCHAR)pv2) == -32) || *(PCHAR)pv1 - *(PCHAR)pv2 == 32) {
			pv1 = (LPSTR)pv1 + 1;
			pv2 = (LPSTR)pv2 + 1;
		}

		return *((PBYTE)pv1) - *((PBYTE)pv2);
	}

	__forceinline INT MemCmp(LPCVOID pv1, LPCVOID pv2, SIZE_T cb) const {
		if (!cb)
			return 0;

		while (--cb && *(LPSTR)pv1 == *(LPSTR)pv2) {
			pv1 = (LPSTR)pv1 + 1;
			pv2 = (LPSTR)pv2 + 1;
		}

		return *((PBYTE)pv1) - *((PBYTE)pv2);
	}

	__forceinline LPVOID MemCpy(LPVOID pvDst, LPCVOID pvSrc, SIZE_T cb) const {
		LPVOID pvRet = pvDst;

		// copy from lower addresses to higher addresses
		while (cb--) {
			*(LPSTR)pvDst = *(LPSTR)pvSrc;
			pvDst = (LPSTR)pvDst + 1;
			pvSrc = (LPSTR)pvSrc + 1;
		}

		return pvRet;
	}

	__forceinline LPCSTR StrCat(LPSTR Dest, LPCSTR Add) const {
		auto to = Dest;
		auto from = Add;

		while (*to) to++;
		for (; *from; to++, from++)
			*to = *from;

		return Dest;
	}
	
};

class DlImageProcessWorker {
private:
	UINT_PTR Base;
	DlMemoryWorker MemoryWorker;
private:
	__forceinline SIZE_T RFindDelimiter(LPCSTR szStr) const {
		auto len = MemoryWorker.StrLen(szStr) - 1;

		for (; len > 0; len--) {
			if (szStr[len] == '\\' || szStr[len] == '/')
				break;
		}

		len--;
		return len;
	}
public:
	typedef IMAGE_THUNK_DATA* PImgThunkData;
	typedef const IMAGE_THUNK_DATA* PCImgThunkData;
	typedef DWORD RVA;
public:
	enum EDllLoadedFlag : INT32 {
		dlfLoadedFailed = -2,
		dlfLoadedNoLoaded = -1,
		dlfLoadedFromSystemDirectory = 0,
		dlfLoadedFromAllowedDirectory,
		dlfLoadedFromApplicationDirectory,
		dlfLoadedFromOtherDirectory,
	};
public:
	template<class X>
	__forceinline X PFromRva(RVA rva) const { return X(PBYTE(Base) + rva); }
	__forceinline VOID SetBase(UINT_PTR base) { Base = base; }
	__forceinline DWORD CountOfImports(PCImgThunkData pitdBase) const {
		DWORD cRet = 0;
		PCImgThunkData pitd = pitdBase;
		while (pitd->u1.Function) {
			pitd++;
			cRet++;
		}
		return cRet;
	}
	__forceinline DWORD CountOfExports(PIMAGE_EXPORT_DIRECTORY pied) const {
		return pied->NumberOfNames;
	}
	__forceinline DWORD IndexFromPImgThunkData(PCImgThunkData pitdCur, PCImgThunkData pitdBase) const {
		return (DWORD)(pitdCur - pitdBase);
	}
	__forceinline PIMAGE_NT_HEADERS64 PinhFromImageBase(HMODULE hmod) const {
		return PIMAGE_NT_HEADERS64(PBYTE(hmod) + PIMAGE_DOS_HEADER(hmod)->e_lfanew);
	}
	__forceinline VOID OverlayIAT(PImgThunkData pitdDst, PCImgThunkData pitdSrc) const {
		MemoryWorker.MemCpy(pitdDst, pitdSrc, CountOfImports(pitdDst) * sizeof IMAGE_THUNK_DATA);
	}
	__forceinline DWORD TimeStampOfImage(PIMAGE_NT_HEADERS pinh) const {
		return pinh->FileHeader.TimeDateStamp;
	}
	__forceinline BOOL FLoadedAtPreferredAddress(PIMAGE_NT_HEADERS pinh, HMODULE hmod) const {
		return UINT_PTR(hmod) == pinh->OptionalHeader.ImageBase;
	}
	EDllLoadedFlag GetDllLoadedFlag(LPCSTR szDllName) const;
	PIMAGE_IMPORT_DESCRIPTOR PiidFromDllName(LPCSTR szDllName) const;
	PIMAGE_EXPORT_DIRECTORY PiedFromDllHandle(HMODULE handle) const;
public:
	DlImageProcessWorker(UINT_PTR base = (UINT_PTR)&__ImageBase);
};

extern "C" LPSTR* __DlAllowedPathForDllLoader;
extern "C" SIZE_T __DlAllowedPathForDllLoaderCount;

class DlCheckModuleProcess {
private:
	DlMemoryWorker MemoryWorker;
	DlImageProcessWorker ImageProcessWorker;
	DlImageProcessWorker ImageProcessWorker2;
public:
	enum EDllRecoveredFlag : INT32 {
		drfRecoveredFailed = -2,
		drfRecoveredNoModule = -1,
		drfRecoveredOk = 0,
	};
public:
	VOID AnalyzeAndRecoveryVTable(VOID);
private:
	EDllRecoveredFlag Recovery(LPCSTR szDllName, HMODULE hmod);
};

class DlLoadModule {
public:
	static BOOL FIXAPI VerifyFileA(LPCSTR fileName);
	static BOOL FIXAPI VerifyFileW(LPCWSTR fileName);
public:
	static HMODULE WINAPI LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
	static HMODULE WINAPI LoadLibraryW(LPCWSTR lpLibFileName);
public:
	DlLoadModule(VOID);
};