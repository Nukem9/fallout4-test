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

#pragma once
#pragma pack(push, 1)

#include "..\[Common]\CommInc.h"
#include "BSString.h"
#include "BSResourceLooseFiles.h"

class BSArchive {
public:
	enum EResultError : DWORD {
		EC_NONE = 0,
	};
public:
	BSArchive(VOID) = default;
	~BSArchive(VOID) = default;
public:
	static VOID FIXAPI GetFileSizeStr(DWORD64 fileSize, BSString& fileSizeStr);
public:
	// For myself, so as not to forget, arrayDataList is a kind of reference to the pointer where 
	// a specific container with the names of files stored in it is located.
	static EResultError FIXAPI hk_LoadArchive(LPVOID arrayDataList, BSResourceLooseFile*& resFile, LPVOID Unk1, DWORD Unk2);

	//24E2E40
	static BOOL FIXAPI hk_Check64bitSize(LPCSTR fileName, DWORD& fileSize);
};

#pragma pack(pop)