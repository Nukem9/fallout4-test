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

#pragma once

#include "TESFile.h"
#include "BSTList.h"

#include <vector>

namespace api {
	class TESDataFileHandler
	{
	public:
		using TESFileList = BSSimpleList<TESFile*>;
		using TESFileListPtr = TESFileList*;
		using TESFileArray = std::vector<TESFile*>;
	public:
		bool Load(int Unknown);
		bool InitUnknownDataSetTextStatusBar(void);
		static void DetectSelectFile(TESFile* File);
		static void EndLoadEvent_SendDone(int32_t index, LPCSTR message);
		static void AttachBA2File(LPCSTR _filename, LPCSTR _folder = NULL);
	public:
		static TESFileListPtr GetArchiveFiles(void);
		static TESFileArray* GetSelectedFiles(void);
		TESFile* GetActiveFile(void) const;
		bool IsActiveFile(void) const;
		bool IsLoaded(void) const;
	public:
		static void Initialize(void);
	public:
		READ_PROPERTY(GetArchiveFiles) TESFileListPtr ArchiveFiles;
		READ_PROPERTY(GetSelectedFiles) TESFileArray* SelectedFiles;
		READ_PROPERTY(GetActiveFile) TESFile* ActiveFile;
	};

	extern TESDataFileHandler* FileHandler;
}