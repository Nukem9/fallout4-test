#include "../common.h"
#include "TES/MemoryManager.h"
#include "TES/bhkThreadMemorySource.h"

#pragma comment(lib, "Version.lib")

#include <string>	

VOID PatchSettings();
VOID PatchMemory();
VOID PatchThreading();
VOID PatchWindow();
VOID LoadSettingFromIni();
BOOL PatchAchievements();


std::string GetGameVersion()
{
	CHAR szExeName[1024];
	GetModuleFileNameA(GetModuleHandleA(NULL), szExeName, ARRAYSIZE(szExeName));

	DWORD dwBuffSize = GetFileVersionInfoSizeA(szExeName, 0);
	LPBYTE pBuff = new BYTE[dwBuffSize];
	std::string str = "";

	if (GetFileVersionInfoA(szExeName, 0, dwBuffSize, pBuff))
	{
		VS_FIXEDFILEINFO* fi = NULL;
		UINT uLen = 0;
		VerQueryValueA(pBuff, "\\", (LPVOID*)&fi, &uLen);

		WORD dwVerMajor = (WORD)(fi->dwProductVersionMS >> 16);
		WORD dwVerMinor = (WORD)(fi->dwProductVersionMS & 0xFF);
		WORD dwPatchLevel = (WORD)(fi->dwProductVersionLS >> 16);
		WORD dwVerBuild = (WORD)(fi->dwProductVersionLS & 0xFF);

		CHAR buff[1024];
		snprintf(buff, ARRAYSIZE(buff), "%u.%u.%u.%u", dwVerMajor, dwVerMinor, dwPatchLevel, dwVerBuild);
		str = buff;
	}

	return str;
}

void Patch_Fallout4Game()
{
	std::string version = GetGameVersion();

	if (version == "1.10.163.0")
	{
		// ???
	}
	else
	{
		CHAR modulePath[MAX_PATH];
		GetModuleFileNameA(GetModuleHandleA(NULL), modulePath, ARRAYSIZE(modulePath));

		CHAR message[1024];
		sprintf_s(message,
			"Unknown Fallout4 version detected. Patches are disabled.\n\n"
			"Required versions:\n"
			"Fallout4.exe 1.10.163.0\n"
			"\nExecutable path: %s", modulePath);

		MessageBoxA(NULL, message, "Version Check", MB_ICONERROR);
		return;
	}

	//
	// Settings
	//
	if (g_INI.GetBoolean("Fallout4", "SettingsPatch", false))
	{
		LoadSettingFromIni();
		PatchSettings();
	}

	if (g_INI.GetBoolean("Fallout4", "AchievementsPatch", false))
	{
		// opens access to steam achievements with mods
		if (!PatchAchievements())
		{
			MessageBoxA(NULL, "Failed to find at least one achievement patch address", "???", 0);
		}
	}

	//
	// MemoryManager
	//
	if (g_INI.GetBoolean("Fallout4", "MemoryPatch", false))
	{
		PatchMemory();

		XUtil::PatchMemory(OFFSET(0xD0C160, 0), { 0xC3 });							// [3GB  ] MemoryManager - Default/Static/File heaps
		XUtil::PatchMemory(OFFSET(0x1B0EDB0, 0), { 0xC3 });							// [1GB  ] BSSmallBlockAllocator
		XUtil::DetourJump(OFFSET(0x1E21B10, 0), &bhkThreadMemorySource::__ctor__);	// [512MB] bhkThreadMemorySource
		XUtil::PatchMemory(OFFSET(0x1B13DF0, 0), { 0xC3 });							// [64MB ] ScrapHeap init
		XUtil::PatchMemory(OFFSET(0x1B14740, 0), { 0xC3 });							// [64MB ] ScrapHeap deinit
																					// [128MB] BSScaleformSysMemMapper is untouched due to complexity

		XUtil::DetourJump(OFFSET(0x1B0EFD0, 0), &MemoryManager::Allocate);
		XUtil::DetourJump(OFFSET(0x1B0F2E0, 0), &MemoryManager::Deallocate);
		XUtil::DetourJump(OFFSET(0x1B13F70, 0), &ScrapHeap::Allocate);
		XUtil::DetourJump(OFFSET(0x1B14580, 0), &ScrapHeap::Deallocate);
	}

	//
	// Threads
	//
	if (g_INI.GetBoolean("Fallout4", "ThreadingPatch", false))
	{
		PatchThreading();
	}

	//
	// Window
	//
	PatchWindow();
}