//////////////////////////////////////////
/*
* Copyright (c) 2020-2022 Perchik71 <email:perchik71@outlook.com>
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
#include "Core.h"
#include "..\[Patches]\CKF4\EditorUI.h"
#include "..\[Patches]\CKF4\EditorUIProgressDialog.h"
#include "..\[Patches]\CKF4\UIProgressDialog.h"
#include "BSArchiveManager.h"

enum {
	MAIN_ARCHIVE			= 1,
	TEXTURES_ARCHIVE		= 2,
	MESHES_ARCHIVE			= 4,
	MESHES_EXTRA_ARCHIVE	= 8,
	MATERIALS_ARCHIVE		= 16,
	VOICES_ARCHIVE			= 32,
	VOICES_EN_ARCHIVE		= 64,
	MISC_ARCHIVE			= 128,
};

BOOL TESUnknown_6D54CF8::TESRenderInfo::IsSky(VOID) const {
	BYTE flag = *(BYTE*)(((uintptr_t)this) + 0x37C);
	// 
	// 37C == 1 or 3
	return (flag == 1) || (flag == 3);
}


TESUnknown_6D54CF8* TESUnknown_6D54CF8::GetInstance(VOID) {
	return *(TESUnknown_6D54CF8**)(OFFSET(0x6D54CF8, 0));
}


BOOL TESUnknown_6D54CF8::IsEmpty(VOID) const {
	return (*(PDWORD)(_pad1 + 0x48) == 0x7FFFFFFF) && (*(PDWORD)(_pad1 + 0x4C) == 0x7FFFFFFF) &&
		(*(PDWORD)(_pad1 + 0x50) == 0x7FFFFFFF) && (*(PDWORD)(_pad1 + 0x54) == 0x7FFFFFFF);
}

BOOL Loaded = FALSE;
TESDataHandler* FileHandler;
std::vector<TESFile*> g_SelectedFilesArray;
std::vector<std::string> g_LoadTypesArchive;

/////////////////////

VOID FIXAPI AttachBA2File(LPCSTR _filename) {
	if (ArchiveManager.IsAvailableForLoad(_filename))
		goto attach_ba2;

	return;

attach_ba2:
	ArchiveManager.LoadArchive(_filename);
}

VOID FIXAPI DetectSelectFile(TESFile* File) {
	// Sometimes duplicated
	if (std::find(g_SelectedFilesArray.begin(), g_SelectedFilesArray.end(), File) == g_SelectedFilesArray.end()) {
		if (File->IsActive()) {
			_MESSAGE_FMT("Load active file %s...", *File->FileName);
//#if FALLOUT4_DEVELOPER_MODE
//			File->Dump();
//#endif // !FALLOUT4_DEVELOPER_MODE
		}
		else if (File->IsMaster() || File->IsSmallMaster())
			_MESSAGE_FMT("Load master file %s...", *File->FileName);
		else
			_MESSAGE_FMT("Load file %s...", *File->FileName);

		g_SelectedFilesArray.push_back(File);
	}

	if (g_i8NeedLoadBA2 > 0) {
		// Added .ba2 files

		auto sname = File->FileName;
		sname.Copy(0, sname.FindLastOf('.'));

		if ((g_i8NeedLoadBA2 & MAIN_ARCHIVE) == MAIN_ARCHIVE)
			AttachBA2File(*(sname + " - Main.ba2"));

		if ((g_i8NeedLoadBA2 & MESHES_ARCHIVE) == MESHES_ARCHIVE)
			AttachBA2File(*(sname + " - Meshes.ba2"));

		if ((g_i8NeedLoadBA2 & TEXTURES_ARCHIVE) == TEXTURES_ARCHIVE) {
			AttachBA2File(*(sname + " - Textures.ba2"));

			for (auto i = 1; i <= 9; i++)
				AttachBA2File(*BSString::FormatString("%s - Textures%d.ba2", *sname, i));
		}

		if ((g_i8NeedLoadBA2 & MATERIALS_ARCHIVE) == MATERIALS_ARCHIVE)
			AttachBA2File(*(sname + " - Materials.ba2"));

		if ((g_i8NeedLoadBA2 & VOICES_ARCHIVE) == VOICES_ARCHIVE)
			AttachBA2File(*(sname + " - Voices.ba2"));

		if ((g_i8NeedLoadBA2 & VOICES_EN_ARCHIVE) == VOICES_EN_ARCHIVE)
			AttachBA2File(*(sname + " - Voices_en.ba2"));

		if ((g_i8NeedLoadBA2 & MESHES_EXTRA_ARCHIVE) == MESHES_EXTRA_ARCHIVE)
			AttachBA2File(*(sname + " - MeshesExtra.ba2"));

		if ((g_i8NeedLoadBA2 & MISC_ARCHIVE) == MISC_ARCHIVE)
			AttachBA2File(*(sname + " - Misc.ba2"));
	}

	fastCall<VOID>(0x7FFF10, File);
}

VOID FIXAPI EndLoadEvent_SendDone(INT index, LPCSTR message) {
	fastCall<VOID>(0x5FDFE0, index, message);
	Loaded = TRUE;
	g_SelectedFilesArray.clear();
}

/////////////////////

BOOL TESDataHandler::IsLoaded(VOID) const {
	return Loaded;
}

VOID TESDataHandler::Initialize(VOID) {
	FileHandler = (TESDataHandler*)OFFSET(0x6D67960, 0);

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