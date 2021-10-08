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

#include "../../common.h"
#include "TESDataFileHandler_CK.h"
#include "UIProgressDialog.h"
#include "LogWindow.h"
#include "EditorUI.h"
#include "EditorUIProgressDialog.h"

#include <filesystem>

bool Loaded = false;
TESDataFileHandler_CK* FileHandler;
TESDataFileHandler_CK::TESFileArray_CK g_SelectedFilesArray;
static std::list<std::string> g_ba2_list;

bool TESDataFileHandler_CK::IsLoaded(void) const {
	return Loaded;
}

void TESDataFileHandler_CK::Initialize(void) {
	FileHandler = (TESDataFileHandler_CK*)OFFSET(0x6D67960, 0);

	CHAR szBuf[MAX_PATH + 1];
	if (!GetModuleFileNameA(GetModuleHandleA(NULL), szBuf, MAX_PATH))
		_MESSAGE("ERROR: An error occurred while retrieving the root folder.");
	else {
		auto path = XUtil::Str::dirnameOf(szBuf);
		//_MESSAGE_FMT("path: %s", path.c_str());

		WIN32_FIND_DATA	FileFindData;
		HANDLE hFindFile = FindFirstFileExA((path + "\\Data\\*.ba2").c_str(), FindExInfoStandard, &FileFindData, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
		if (hFindFile != INVALID_HANDLE_VALUE) {
			do {
				std::string sname = FileFindData.cFileName;
				g_ba2_list.emplace_back(XUtil::Str::LowerCase(sname));
				//_MESSAGE_FMT("data: %s", FileFindData.cFileName);
			} while (FindNextFile(hFindFile, &FileFindData));
		}
	}

	// exclude .ba2 files

	if (g_INI_CK_Cfg) {
		auto func = [](const std::string& svalue) {
			std::string sname;

			if (svalue.length() > 0) {
				LPSTR s_c = new CHAR[svalue.length() + 1];
				strcpy(s_c, svalue.c_str());

				LPSTR stoken = strtok(s_c, ",");
				if (stoken) {
					do {
						sname = XUtil::Str::LowerCase(XUtil::Str::trim(stoken));
						//_MESSAGE_FMT("data: %s", sname.c_str());

						g_ba2_list.remove(sname);

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

	// Recognition of loaded files
	XUtil::DetourClassCall(OFFSET(0x801AA7, 0), &TESDataFileHandler_CK::DetectSelectFile);
	// End loaded files
	XUtil::DetourClassJump(OFFSET(0x5FBF71, 0), &TESDataFileHandler_CK::EndLoadEvent_SendDone);
}

void TESDataFileHandler_CK::EndLoadEvent_SendDone(int32_t index, LPCSTR message) {
	((void(__fastcall*)(int32_t, LPCSTR))OFFSET(0x5FDFE0, 0))(index, message);
	Loaded = true;
}

bool TESDataFileHandler_CK::Load(int Unknown) {
	Loaded = false;
	g_SelectedFilesArray.clear();
	// loads, checks.
	return ((bool(__fastcall*)(TESDataFileHandler_CK*, int))OFFSET(0x7D9D80, 0))(this, Unknown);
}

bool TESDataFileHandler_CK::InitUnknownDataSetTextStatusBar(void) {
	// Replacing Tips with a progress Bar
	if (g_UIEnabled && EditorUI::bReplaceTips)
	{
		// set to progressbar
		Core::Classes::UI::ProgressDialog->MessageText = "Loading Files... Initializing...";
		Core::Classes::UI::ProgressDialog->Marquee = TRUE;
		// also in taskbar
		EditorUI::SetMarqueeInTaskbar(TRUE);
	}

	// Unknown. Initializes something.
	return ((bool(__fastcall*)(TESDataFileHandler_CK*))OFFSET(0x7D66A0, 0))(this);
}

void TESDataFileHandler_CK::DetectSelectFile(TESFile_CK* File) {
	// Sometimes duplicated
	if (std::find(g_SelectedFilesArray.begin(), g_SelectedFilesArray.end(), File) == g_SelectedFilesArray.end())
	{
		if (File->IsActive()) 
			_MESSAGE_FMT("Load active file %s...", File->FileName.c_str());
		else if (File->IsMaster() || File->IsSmallMaster())
			_MESSAGE_FMT("Load master file %s...", File->FileName.c_str());
		else
			_MESSAGE_FMT("Load file %s...", File->FileName.c_str());

		g_SelectedFilesArray.push_back(File);
	}

	// Added .ba2 files

	CHAR szBuf[MAX_PATH + 1];
	if (!GetModuleFileNameA(GetModuleHandleA(NULL), szBuf, MAX_PATH))
		_MESSAGE("ERROR: An error occurred while retrieving the root folder.");
	else {
		auto path = XUtil::Str::dirnameOf(szBuf) + "\\Data\\";

		std::string sname = File->FileName.c_str();
		sname = sname.substr(0, sname.find_last_of('.'));

		AttachBA2File((sname + " - Main.ba2").c_str(), path.c_str());
		AttachBA2File((sname + " - Textures.ba2").c_str(), path.c_str());
		AttachBA2File((sname + " - Materials.ba2").c_str(), path.c_str());
	}

	((void(__fastcall*)(TESFile_CK*))OFFSET(0x7FFF10, 0))(File);
}

TESDataFileHandler_CK::TESFileListPtr_CK TESDataFileHandler_CK::GetArchiveFiles(void) {
	return (TESDataFileHandler_CK::TESFileListPtr_CK)OFFSET(0x6D68910, 0);
}

TESDataFileHandler_CK::TESFileArray_CK* TESDataFileHandler_CK::GetSelectedFiles(void) {
	return &g_SelectedFilesArray;
}

TESFile_CK* TESDataFileHandler_CK::GetActiveFile(void) const {
	return *(TESFile_CK**)(((char*)this) + 0xFA8);
}

bool TESDataFileHandler_CK::IsActiveFile(void) const {
	return GetActiveFile() != nullptr;
}

void TESDataFileHandler_CK::AttachBA2File(LPCSTR _filename, LPCSTR _folder) {
	std::string sname2 = _filename;
	for each (auto sname1 in g_ba2_list) {
		if (!sname1.compare(XUtil::Str::LowerCase(sname2)))
			goto attach_ba2;
	}

	return;

attach_ba2:
	LPVOID lpUnknownClass = (LPVOID)OFFSET(0x38559E8, 0);

	if (_folder) {
		if (std::filesystem::exists(std::string(_folder) + _filename)) {
			_MESSAGE_FMT("Load archive %s...", _filename);
			((void(__fastcall*)(LPCSTR, int32_t, LPVOID*))OFFSET(0x24CC100, 0))(_filename, 0, &lpUnknownClass);
		}
	}
	else {
		if (std::filesystem::exists(std::string("Data\\") + _filename)) {
			_MESSAGE_FMT("Load archive %s...", _filename);
			((void(__fastcall*)(LPCSTR, int32_t, LPVOID*))OFFSET(0x24CC100, 0))(_filename, 0, &lpUnknownClass);
		}
	}
}