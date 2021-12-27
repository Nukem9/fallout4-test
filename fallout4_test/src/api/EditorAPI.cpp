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

#include "../StdAfx.h"
#include "EditorAPI.h"

#include "../patches/CKF4/EditorUI.h"
#include "../patches/CKF4/EditorUIProgressDialog.h"
#include "../patches/CKF4/UIProgressDialog.h"

using namespace api;

BOOL TESScene::TESRenderInfo::IsSky(VOID) const {
	BYTE flag = *(BYTE*)(((uintptr_t)this) + 0x37C);
	// 
	// 37C == 1 or 3
	return (flag == 1) || (flag == 3);
}


TESScene* TESScene::GetScene(VOID) {
	return *(TESScene**)(OFFSET(0x6D54CF8, 0));
}


BOOL TESScene::IsEmpty(VOID) const {
	return (*(PDWORD)(_pad1 + 0x48) == 0x7FFFFFFF) && (*(PDWORD)(_pad1 + 0x4C) == 0x7FFFFFFF) &&
		(*(PDWORD)(_pad1 + 0x50) == 0x7FFFFFFF) && (*(PDWORD)(_pad1 + 0x54) == 0x7FFFFFFF);
}

namespace api {
	BOOL Loaded = FALSE;
	TESDataHandler* FileHandler;
	std::vector<TESFile*> g_SelectedFilesArray;
	static std::list<std::string> g_ba2_list;

	/////////////////////

	VOID FIXAPI AttachBA2File(LPCSTR _filename, LPCSTR _folder) {
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
				fastCall<VOID>(0x24CC100, _filename, 0, &lpUnknownClass);
			}
		}
		else {
			if (std::filesystem::exists(std::string("Data\\") + _filename)) {
				_MESSAGE_FMT("Load archive %s...", _filename);
				fastCall<VOID>(0x24CC100, _filename, 0, &lpUnknownClass);
			}
		}
	}

	VOID FIXAPI DetectSelectFile(TESFile* File) {
		// Sometimes duplicated
		if (std::find(g_SelectedFilesArray.begin(), g_SelectedFilesArray.end(), File) == g_SelectedFilesArray.end()) {
			if (File->IsActive()) {
				_MESSAGE_FMT("Load active file %s...", *File->FileName);
#if FALLOUT4_DEVELOPER_MODE
				File->Dump();
#endif // !FALLOUT4_DEVELOPER_MODE
			}
			else if (File->IsMaster() || File->IsSmallMaster())
				_MESSAGE_FMT("Load master file %s...", *File->FileName);
			else
				_MESSAGE_FMT("Load file %s...", *File->FileName);

			g_SelectedFilesArray.push_back(File);
		}

		if (g_i8NeedLoadBA2) {
			// Added .ba2 files

			CHAR szBuf[MAX_PATH + 1];
			if (!GetModuleFileNameA(GetModuleHandleA(NULL), szBuf, MAX_PATH))
				_MESSAGE("ERROR: An error occurred while retrieving the root folder.");
			else {
				auto path = XUtil::Str::dirnameOf(szBuf) + "\\Data\\";

				std::string sname = *File->FileName;
				sname = sname.substr(0, sname.find_last_of('.'));

				AttachBA2File((sname + " - Main.ba2").c_str(), path.c_str());
				AttachBA2File((sname + " - Meshes.ba2").c_str(), path.c_str());
				AttachBA2File((sname + " - Textures.ba2").c_str(), path.c_str());
				AttachBA2File((sname + " - Materials.ba2").c_str(), path.c_str());
			}
		}

		fastCall<VOID>(0x7FFF10, File);
	}

	VOID FIXAPI EndLoadEvent_SendDone(INT index, LPCSTR message) {
		fastCall<VOID>(0x5FDFE0, index, message);
		Loaded = TRUE;
		g_SelectedFilesArray.clear();
	}

	/////////////////////

	bool TESDataHandler::IsLoaded(void) const {
		return Loaded;
	}

	VOID TESDataHandler::Initialize(VOID) {
		FileHandler = (TESDataHandler*)OFFSET(0x6D67960, 0);
		if (g_i8NeedLoadBA2) {
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
		}

		// Recognition of loaded files
		XUtil::DetourCall(OFFSET(0x801AA7, 0), &DetectSelectFile);
		// End loaded files
		XUtil::DetourJump(OFFSET(0x5FBF71, 0), &EndLoadEvent_SendDone);
	}

	bool TESDataHandler::Load(int Unknown) {
		Loaded = false;
		g_SelectedFilesArray.clear();

		// loads, checks.
		return thisCall<BOOL>(0x7D9D80, this, Unknown);
	}

	bool TESDataHandler::InitUnknownDataSetTextStatusBar(void) {
		// Replacing Tips with a progress Bar
		if (EditorUI::bReplaceTips) {
			// set to progressbar
			Core::Classes::UI::ProgressDialog->MessageText = "Loading Files... Initializing...";
			Core::Classes::UI::ProgressDialog->Marquee = TRUE;
			// also in taskbar
			EditorUI::SetMarqueeInTaskbar(TRUE);
		}

		// Unknown. Initializes something.
		return thisCall<BOOL>(0x7D66A0, this);
	}

	//BOOL TESDataHandler::hk_SaveTESFile(LPCSTR filename) {
	//	CHAR szBuf[MAX_PATH + 1];
	//	if (!GetModuleFileNameA(GetModuleHandleA(NULL), szBuf, MAX_PATH))
	//		_MESSAGE("ERROR: An error occurred while retrieving the root folder.");
	//	else {
	//		auto fname = XUtil::Str::dirnameOf(szBuf) + "\\Data\\" + filename;

	//		if (std::filesystem::exists(fname)) {
	//			auto to = XUtil::Str::ChangeFileExt(fname, ".bak");

	//			_MESSAGE_FMT("Beginning backup file: %s", to.c_str());
	//			
	//			if (!CopyFileA(fname.c_str(), to.c_str(), FALSE))
	//				_MESSAGE_FMT("Failed CopyFileA return code %d msg %s", GetLastError(), XUtil::Str::GetLastErrorToStr(__FUNCTION__).c_str());
	//			// Crashes when copying a file some people, some crap, what's wrong with the world.
	//			//std::filesystem::copy_file(fname, to);

	//			_MESSAGE_FMT("End backup file: %s", to.c_str());

	//			BOOL bResult = SaveTESFile(this, filename);
	//			if (bResult) {
	//				_MESSAGE("Successfully saving, deleting a backup copy.");
	//				DeleteFileA(to.c_str());
	//				//std::filesystem::remove(to);
	//			}

	//			return bResult;
	//		}
	//		else
	//			return SaveTESFile(this, filename);

	//	}

	//	return FALSE;
	//}
}