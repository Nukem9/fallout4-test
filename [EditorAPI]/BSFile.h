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

#include "NiClasses.h"

class BSFile : public NiFile {
protected:
	// members
	//     /*000*/ NiFile
	/*150*/ DWORD64	unk150;
	/*158*/ DWORD64	unk158;
	/*160*/ DWORD	unk160;					// init to -1
	/*164*/ DWORD	unk164;
	/*168*/ DWORD	unk168;
	/*16C*/ CHAR	_FileName2[MAX_PATH];	// I don't know why, but this is once again the path to the file
	/*270*/ DWORD64	unk270;
	/*278*/ DWORD64	unk278;
	/*280*/ DWORD64	_Pos2;
	/*288*/ DWORD64	_FileSize;
	/*290*/ DWORD64	unk290;
	/*298*/ DWORD64	unk298;
	/*2A0*/ DWORD	unk2A0;					// init to -1
	/*2A4*/ DWORD	unk2A4;
	/*2A8*/ DWORD	unk2A8;
	/*2AC*/ DWORD	unk2AC;
public:
	//__forceinline DWORD64 GetFileSize(VOID) const { return _FileSize; }
	static BSFile* FIXAPI CreateInstance(LPCSTR fileName, DWORD mode, DWORD64 bufferSize, BOOL isTextFile);
	VOID DeleteInstance(BOOL ReleaseMemory = TRUE);

	DWORD64 GetFileSize(VOID);
	DWORD64 Read(LPVOID buffer, DWORD64 size);
	DWORD64 Write(LPCVOID buffer, DWORD64 size);
public:
	inline static BOOL(*ICreateInstance)(BSFile*, LPCSTR, DWORD, DWORD64, BOOL);
	BOOL hk_ICreateInstance(LPCSTR fileName, DWORD mode, DWORD64 bufferSize, BOOL isTextFile);
};
static_assert(sizeof(BSFile) == 0x2B0, "BSFile class should be the size of 0x2B0");

#pragma pack(pop)