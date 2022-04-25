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
#include "BSFile.h"
#include "NiMemoryManager.h"

BSFile* FIXAPI BSFile::CreateInstance(LPCSTR fileName, DWORD mode, DWORD64 bufferSize, BOOL isTextFile) {
	BSFile* NewInstance = (BSFile*)NiMemoryManager::Alloc(NULL, sizeof(BSFile));
	thisCall<BOOL, LPCSTR, DWORD, DWORD64, BOOL>(0x2482D70, NewInstance, fileName, mode, bufferSize, isTextFile);
	return NewInstance;
}

VOID BSFile::DeleteInstance(BOOL ReleaseMemory) {
	thisVirtualCall<VOID, BOOL>(0x0, this, ReleaseMemory);
}

DWORD64 BSFile::GetFileSize(VOID) {
	return thisVirtualCall<DWORD64>(0x40, this);
}

DWORD64 BSFile::Read(LPVOID buffer, DWORD64 size) {
	return thisVirtualCall<DWORD64, LPVOID, DWORD64>(0x28, this, buffer, size);
}

DWORD64 BSFile::Write(LPCVOID buffer, DWORD64 size) {
	return thisVirtualCall<DWORD64, LPCVOID, DWORD64>(0x30, this, buffer, size);
}

BOOL BSFile::hk_ICreateInstance(LPCSTR fileName, DWORD mode, DWORD64 bufferSize, BOOL isTextFile) {
	if (mode == FileModes::kFileMode_ReadOnly && bufferSize < 0x40000)
		bufferSize = 0x40000;

	return ICreateInstance(this, fileName, mode, bufferSize, isTextFile);
}