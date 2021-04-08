#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <iterator>
#include <memory>
#include <map>

struct membuf : public std::streambuf
{
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

std::map<std::wstring, membuf*> dllENBs =
{ 
	{ L"d3d11.dll", NULL },
	{ L"d3d10.dll", NULL },
	{ L"d3d9.dll",  NULL },
	{ L"d3dcompiler_46e.dll",  NULL },
};

std::wstring app_path;

BOOL FileExists(const std::wstring fname)
{
	return std::experimental::filesystem::exists(app_path + fname);
}

VOID ReadFileToMemory(VOID)
{
	for (auto dll = dllENBs.begin(); dllENBs.end() != dll; dll++)
	{
		if (FileExists((*dll).first))
		{
			std::wcout << "File """<< (*dll).first.c_str() << """ found and will be remove.\n";

			HANDLE fileDll = CreateFileW((app_path + (*dll).first).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (!fileDll)
			{
				std::wcout << "File """ << (*dll).first.c_str() << """ failed open.\n";
				break;
			}

			DWORD len = GetFileSize(fileDll, NULL);

			(*dll).second = new membuf(len);

			DWORD m;
			if (!ReadFile(fileDll, (*dll).second->get(), len, &m, NULL))
			{
				std::wcout << "File """ << (*dll).first.c_str() << """ failed read file.\n";
			}
			CloseHandle(fileDll);

			std::wcout << "File """ << (*dll).first.c_str() << """ read " << m <<".\n";

			DeleteFileW((app_path + (*dll).first).c_str());
		}
	}
}

VOID WriteFileToFolder(VOID)
{
	for (auto dll = dllENBs.begin(); dllENBs.end() != dll; dll++)
	{
		if ((*dll).second)
		{
			std::wcout << "File """ << (*dll).first.c_str() << """ was removed and will be restored.\n";

			HANDLE fileDll = CreateFileW((app_path + (*dll).first).c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
			if (!fileDll)
			{
				std::wcout << "File """ << (*dll).first.c_str() << """ failed create.\n";
				break;
			}

			DWORD m;
			if (!WriteFile(fileDll, (*dll).second->get(), (*dll).second->size(), &m, NULL))
			{
				std::wcout << "File """ << (*dll).first.c_str() << """ failed write file.\n";
			}
			CloseHandle(fileDll);

			std::wcout << "File """ << (*dll).first.c_str() << """ write " << m << ".\n";

			delete (*dll).second;
			(*dll).second = NULL;
		}
	}
}

INT32 WINAPI main(INT32 argc, LPWSTR argv[])
{
	WCHAR basePath[2048] = L"";
	GetFullPathNameW(argv[0], 2048, basePath, NULL);
	std::experimental::filesystem::path p = basePath;

	app_path = p.parent_path();
	app_path += L"\\";

	std::wcout << "Root dir: " << app_path << "\n";
	std::wcout << "Delete files.\n";
	ReadFileToMemory();
	std::wcout << "Open Creation Kit.\n";
	ShellExecuteW(NULL, L"open", L"CreationKit.exe", NULL, app_path.c_str(), SW_SHOW);
	std::wcout << "Wait 15 sec.\n";
	Sleep(15000);
	std::wcout << "Return files.\n";
	WriteFileToFolder();
}