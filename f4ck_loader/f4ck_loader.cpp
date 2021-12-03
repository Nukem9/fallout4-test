#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define F4CKLAPI __stdcall

#pragma warning(disable : 26451)

#include "resource1.h"

#include <Windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <iterator>
#include <memory>
#include <map>

#define CREATIONKIT L"CreationKit.exe"
#define CREATIONKIT_PATCHED L"CreationKit.patched.exe"

struct membuf : public std::streambuf {
	membuf(DWORD len)
		: std::streambuf()
		, len(len)
		, src((len > 0) ? new CHAR[len] : NULL)
	{
		setg(src.get(), src.get(), src.get() + len);
	}

	// direct buffer access for file load.
	LPSTR get(VOID) { return src.get(); };
	DWORD size(VOID) const { return len; };

private:
	std::unique_ptr<CHAR> src;
	DWORD len;
};

std::map<std::wstring, membuf*> dllENBs = { 
	{ L"d3d11.dll", NULL },
	{ L"d3d10.dll", NULL },
	{ L"d3d9.dll", NULL },
	{ L"d3dcompiler_46e.dll", NULL },
	{ L"dxgi.dll", NULL },
	{ L"enbimgui.dll", NULL },
};

std::wstring app_path;

#ifdef HIDEWINDOW

VOID F4CKLAPI HideConsole(VOID) {
	ShowWindow(GetConsoleWindow(), SW_HIDE);
}

#endif

BOOL F4CKLAPI FileExists(const std::wstring fname) {
	return std::filesystem::exists(app_path + fname);
}

BOOL F4CKLAPI KillProcess(DWORD dwPid) {
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProc)
		return FALSE;

	BOOL bResult = TerminateProcess(hProc, 0);
	WaitForSingleObject(hProc, INFINITE);

	return bResult;
}

VOID F4CKLAPI ReadFileToMemory(VOID) {
	for (auto dll = dllENBs.begin(); dllENBs.end() != dll; dll++) {
		if (FileExists((*dll).first)) {
#ifndef HIDEWINDOW
			std::wcout << L"File """ << (*dll).first.c_str() << L""" found and will be remove.\n";
#endif
			HANDLE fileDll = CreateFileW((app_path + (*dll).first).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (!fileDll) {
#ifndef HIDEWINDOW
				std::wcout << L"File """ << (*dll).first.c_str() << L""" failed open.\n";
#else
				std::wstring msg = std::wstring(L"File """) + (*dll).first.c_str() + L""" failed open.";
				MessageBoxW(0, msg.c_str(), L"Error", MB_OK | MB_ICONERROR);
#endif
				break;
			}

			DWORD len = GetFileSize(fileDll, NULL);

			(*dll).second = new membuf(len);

			DWORD m;
			if (!ReadFile(fileDll, (*dll).second->get(), len, &m, NULL)) {
#ifndef HIDEWINDOW
				std::wcout << L"File """ << (*dll).first.c_str() << L""" failed read file.\n";
#endif
			}
			CloseHandle(fileDll);
#ifndef HIDEWINDOW
			std::wcout << L"File """ << (*dll).first.c_str() << L""" read " << m << L".\n";
#endif
			DeleteFileW((app_path + (*dll).first).c_str());
		}
	}
}

VOID F4CKLAPI WriteFileToFolder(VOID)
{
	for (auto dll = dllENBs.begin(); dllENBs.end() != dll; dll++) {
		if ((*dll).second) {
#ifndef HIDEWINDOW
			std::wcout << L"File """ << (*dll).first.c_str() << L""" was removed and will be restored.\n";
#endif
			HANDLE fileDll = CreateFileW((app_path + (*dll).first).c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
			if (!fileDll) {
#ifndef HIDEWINDOW
				std::wcout << L"File """ << (*dll).first.c_str() << L""" failed create.\n";
#else
				std::wstring msg = std::wstring(L"File """) + (*dll).first.c_str() + L""" failed create.";
				MessageBoxW(0, msg.c_str(), L"Error", MB_OK | MB_ICONERROR); 
#endif
				break;
			}

			DWORD m;
			if (!WriteFile(fileDll, (*dll).second->get(), (*dll).second->size(), &m, NULL)) {
#ifndef HIDEWINDOW
				std::wcout << L"File """ << (*dll).first.c_str() << L""" failed write file.\n";
#endif
			}
			CloseHandle(fileDll);
#ifndef HIDEWINDOW
			std::wcout << L"File """ << (*dll).first.c_str() << L""" write " << m << L".\n";
#endif
			delete (*dll).second;
			(*dll).second = NULL;
		}
	}
}

INT_PTR CALLBACK DlgPleaseWaitProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	return FALSE;
}

VOID WaitCloseF4CKLoader(VOID) {
	DWORD dwCount = 0;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcess = INVALID_HANDLE_VALUE;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	DWORD dwCurrentProcessID = GetCurrentProcessId();

	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (entry.th32ProcessID != dwCurrentProcessID && !_wcsicmp(entry.szExeFile, L"f4ck_loader.exe")) {
				dwCount++;
				if (dwCount >= 2)
					// user spam run
					TerminateProcess(GetCurrentProcess(), 0);

				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
			}
		}
	}

	CloseHandle(snapshot);

	if (hProcess != INVALID_HANDLE_VALUE) {
	
		HWND hDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PLEASEWAIT), NULL, &DlgPleaseWaitProc);
			
		WaitForSingleObject(hProcess, INFINITE);

		EndDialog(hDlg, 0);
		DestroyWindow(hDlg);
	}
}

INT32 WINAPI main(INT32 argc, LPSTR argv[])
{
#ifdef HIDEWINDOW
	HideConsole();
#endif

	WaitCloseF4CKLoader();

	std::wstring StrCmdLine;
	BOOL bSafeRun = TRUE;
	BOOL bPatchedPreVis = FALSE;
	CHAR basePath[2048] = "";
	GetFullPathNameA(argv[0], 2048, basePath, NULL);
	std::filesystem::path p(basePath);
	app_path = p.parent_path();
	app_path += L"\\";

	if (argc > 1) {
		bSafeRun = FALSE;
		DWORD pid = MAXDWORD;
		LPSTR cmd, token;
		DWORD scmd = (DWORD)strlen(argv[1]);
		if (!scmd)
			goto l_skip;

		cmd = new CHAR[scmd + 1];
		strcpy(cmd, argv[1]);
		token = strtok(cmd, ":");
		if (token) {
			if (_stricmp(token, "pid"))
				goto l_skip_f;

			if (token = strtok(NULL, ","); token)
				pid = (DWORD)strtoul(token, NULL, 10);
		}

	l_skip_f:
		delete[] cmd;

		if (pid != MAXDWORD) {
			bSafeRun = KillProcess(pid);
			if (!bSafeRun) {
#ifndef HIDEWINDOW
				std::wcout << L"required process """ << pid << L""" couldn't be closed.\n";
#else
				MessageBoxW(0,
					L"Required process couldn't be closed.\n"
					L"The CreationKit will not be started.",
					L"Error",
					MB_OK | MB_ICONERROR);

#endif
				return 1;
			}
		}
		else {
			bSafeRun = TRUE;

			size_t istart = std::wstring::npos;
			StrCmdLine = GetCommandLineW();
			if (StrCmdLine.c_str()[0] == '"') {
				istart = StrCmdLine.find_first_of('"', 1);
				if (std::wstring::npos != istart)
					istart++;
			}
			else
				istart = StrCmdLine.find_first_of(' ');

			if (std::wstring::npos != istart) {
				StrCmdLine = StrCmdLine.substr(istart + 1);

				istart = StrCmdLine.find_first_of('-');
				if (std::wstring::npos != istart) {
					auto iend = StrCmdLine.find_first_of(':', istart);
					if (std::wstring::npos != iend) {
						auto sCmd = StrCmdLine.substr(istart, iend - 1);

						if (!_wcsicmp(sCmd.c_str(), L"-GeneratePreCombined") ||
							!_wcsicmp(sCmd.c_str(), L"-GeneratePreVisData"))
							bPatchedPreVis = std::filesystem::exists(app_path + CREATIONKIT_PATCHED);
					}
				}
			}
			else
				StrCmdLine.clear();
		}
	}
l_skip:
#ifndef HIDEWINDOW
	std::wcout << L"Root dir: " << app_path << L"\n";
	std::wcout << L"Delete files.\n";
#endif
	ReadFileToMemory();
#ifndef HIDEWINDOW
	std::wcout << L"Open Creation Kit.\n";
#endif
	if (bSafeRun)
		ShellExecuteW(NULL, L"open", bPatchedPreVis ? CREATIONKIT_PATCHED : CREATIONKIT, StrCmdLine.c_str(), app_path.c_str(), SW_SHOW);
#ifndef HIDEWINDOW
	std::wcout << L"Wait 15 sec.\n";
#endif
	Sleep(bPatchedPreVis ? 25000 : 15000);
#ifndef HIDEWINDOW
	std::wcout << L"Return files.\n";
#endif
	WriteFileToFolder();

	return 0;
}

#pragma warning(default : 26451)