//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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
#include "version_info.h"

VOID FIXAPI Sys_DumpDisableBreakpoint(VOID);
VOID FIXAPI Sys_DumpEnableBreakpoint(VOID);
VOID FIXAPI MainFix_PatchFallout4CreationKit(VOID);
VOID FIXAPI MainFix_PatchFallout4Game(VOID);

/*
==================
Sys_ApplyPatches

The main function for code injection, called from dump.cpp
==================
*/
VOID FIXAPI Sys_ApplyPatches(VOID)
{
	// The EXE has been unpacked at this point
	strcpy_s(g_GitVersion, VER_CURRENT_COMMIT_ID);
	strcpy_s(g_GitDataVersion, VER_CURRENT_DATE);

	XUtil::SetThreadName(GetCurrentThreadId(), "Main Thread");

#if FALLOUT4_USE_VTUNE
	// Check if VTune is already active
	const char *libITTPath = getenv("INTEL_LIBITTNOTIFY64");

	if (!libITTPath)
		libITTPath = "libittnotify.dll";

	g_DllVTune = LoadLibraryA(libITTPath);

	__itt_domain_create("MemoryManager");
	ITT_AllocateCallback = __itt_heap_function_create("Allocate", "MemoryManager");
	ITT_ReallocateCallback = __itt_heap_function_create("Reallocate", "MemoryManager");
	ITT_FreeCallback = __itt_heap_function_create("Free", "MemoryManager");
#endif

	switch (g_LoadType)
	{
	case GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4:
		MainFix_PatchFallout4CreationKit();
		break;
	case GAME_EXECUTABLE_TYPE::GAME_FALLOUT4:
		MainFix_PatchFallout4Game();
		break;
	}
}

/*
==================
DllMain

==================
*/
BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
		// Force this dll to be loaded permanently
		HMODULE temp;
		g_hModule = hModule;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)hModule, &temp);

		// Then determine which exe is being loaded
		g_LoadType = GAME_EXECUTABLE_TYPE::UNKNOWN;

		CHAR modulePath[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(NULL), modulePath, MAX_PATH);

		CHAR executableName[MAX_PATH];
		_strlwr_s(modulePath);
		_splitpath_s(modulePath, NULL, 0, NULL, 0, executableName, ARRAYSIZE(executableName), NULL, 0);

		switch (CRC32(executableName))
		{
		case CRC32("fallout4launcher"):
			g_LoadType = GAME_EXECUTABLE_TYPE::LAUNCHER_FALLOUT4;
			break;

		case CRC32("fallout4"):
		case CRC32("fallout4_dump"):
			g_LoadType = GAME_EXECUTABLE_TYPE::GAME_FALLOUT4;
			break;

		case CRC32("creationkit"):
			g_LoadType = GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4;
			break;
		}

		// For now, skip everything except the game and CK
		switch (g_LoadType)
		{
		case GAME_EXECUTABLE_TYPE::GAME_FALLOUT4:
		case GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4:
		{
			// FIX: 
			// Bug report "pra": Cannot use CK to preview NIFs
			// Set current path to app and create ini reader objects
			//

			CHAR creationkitPath[MAX_PATH + 1];
			GetModuleFileNameA(NULL, creationkitPath, MAX_PATH);
			if (!GetLastError())
			{
				std::string directory, source(creationkitPath);
				const size_t last_slash_idx = source.rfind('\\');
				if (std::string::npos != last_slash_idx)
					directory = source.substr(0, last_slash_idx);
				if (directory.length() > 0)
				{
					if (!SetCurrentDirectoryA(directory.c_str()))
						goto SetCurPathLabFailed;
				}
				else
					goto SetCurPathLabFailed;
			}
			else
			SetCurPathLabFailed:
				MessageBoxA(
					NULL,
					"An error occurred when determining or setting the current path,"
					"perhaps this will lead to other errors or incorrect operation of the program.",
					"Warning",
					MB_OK);
#if FALLOUT4_MINI_PROJECT
			g_INI = new mINIReader("fallout4_test.ini");
			g_INI_CK = new mINIReader("CreationKitPrefs.ini");
			g_INI_CK_Cfg = new mINIReader("CreationKit.ini");
			g_INI_CK_CfgCustom = new mINIReader("CreationKitCustom.ini");
#else
			g_INI = new INIReader("fallout4_test.ini");
			g_INI_CK_Cfg = new INIReader("CreationKit.ini");
			g_INI_CK_CfgCustom = new INIReader("CreationKitCustom.ini");
#endif
#if FALLOUT4_CREATIONKIT_ONLY
			if (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4)
				return TRUE;
#else
			if (!g_INI->GetBoolean("Mode", "Extended", FALSE) && (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4))
			{
				return TRUE;
			}
#endif

			Sys_DumpEnableBreakpoint();
			break;
		}
		}
    }

    return TRUE;
}
