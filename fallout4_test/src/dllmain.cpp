#include "common.h"
#include "version_info.h"
#include "profiler_internal.h"

VOID Patch_Fallout4CreationKit();
VOID Patch_Fallout4Game();

VOID ApplyPatches()
{
	// The EXE has been unpacked at this point
	strcpy_s(g_GitVersion, VER_CURRENT_COMMIT_ID);
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
		Patch_Fallout4CreationKit();
		break;
	case GAME_EXECUTABLE_TYPE::GAME_FALLOUT4:
		Patch_Fallout4Game();
		break;
	}
}

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
#if FALLOUT4_CREATIONKIT_ONLY
			if (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4)
				return TRUE;
#else
			if (!g_INI.GetBoolean("Mode", "Extended", FALSE) && (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4))
			{
				return TRUE;
			}
#endif

			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex);
			if (GlobalMemoryStatusEx(&statex))
			{
				auto GB = statex.ullTotalPhys / (1024 * 1024 * 1024);
				g_ScrapSize = (GB > 4) ? 0x8000000 : 0x4000000;
				g_bhkMemSize = (GB > 8) ? 0x80000000 : 0x40000000;
			}
			else
			{
				g_ScrapSize = 0x2000000;
				g_bhkMemSize = 0x20000000;
			}

			DumpEnableBreakpoint();
			break;
		}
    }

    return TRUE;
}
