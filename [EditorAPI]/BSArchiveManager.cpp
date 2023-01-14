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

#include "..\[Common]\StdAfx.h"

#include "BSString.h"
#include "BSTArray.h"
#include "BSArchiveManager.h"

BSArchiveManager ArchiveManager;

static BSTArray<BSString*> g_arrayArchivesAvailable;

VOID BSArchiveManager::Initialize(VOID) {
	auto pathData = BSString::Utils::GetDataPath();
	
	WIN32_FIND_DATA	FileFindData;
	HANDLE hFindFile = FindFirstFileExA(*(pathData + "*.ba2"), FindExInfoStandard, &FileFindData, 
		FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
	if (hFindFile != INVALID_HANDLE_VALUE) {
		do {
			g_arrayArchivesAvailable.Push(new BSString(FileFindData.cFileName));
		} while (FindNextFile(hFindFile, &FileFindData));
	}

	// exclude .ba2 files

	if (g_INI_CK_Cfg) {
		auto func = [](const std::string& svalue) {
			BSString strName;

			if (svalue.length() > 0) {
				LPSTR s_c = new CHAR[svalue.length() + 1];
				strcpy(s_c, svalue.c_str());

				LPSTR stoken = strtok(s_c, ",");
				if (stoken) {
					do {
						auto index = g_arrayArchivesAvailable.GetStrIndex(XUtil::Str::trim(stoken).c_str());
						if (index != MAXDWORD) {
							delete g_arrayArchivesAvailable[index];
							g_arrayArchivesAvailable.Remove(index);
						}

						stoken = strtok(NULL, ",");
						
					} while (stoken);
				}

				delete[] s_c;
			}
		};

		func(g_INI_CK_Cfg->Get("Archive", "sResourceArchiveList", ""));
		func(g_INI_CK_Cfg->Get("Archive", "sResourceArchiveList2", ""));
		func(g_INI_CK_Cfg->Get("Archive", "sResourceArchiveMemoryCacheList", ""));
		func(g_INI_CK_Cfg->Get("Archive", "sResourceStartUpArchiveList", ""));
		func(g_INI_CK_Cfg->Get("Archive", "sResourceIndexFileList", ""));
	}
}

VOID BSArchiveManager::LoadArchive(LPCSTR ArchiveName, DWORD64 Unk02, DWORD64 Unk03) {
	if (BSString::Utils::FileExists(BSString::Utils::GetDataPath() + ArchiveName))
		fastCall<VOID>(0x24CC100, ArchiveName, Unk02, Unk03);
}

BOOL BSArchiveManager::IsAvailableForLoad(LPCSTR ArchiveName) {
	auto index = g_arrayArchivesAvailable.GetStrIndex(ArchiveName);

	return index != MAXDWORD;
}