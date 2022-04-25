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
#include "BSArchive.h"

VOID FIXAPI BSArchive::GetFileSizeStr(DWORD64 fileSize, BSString& fileSizeStr) {
	if (fileSize >= 0x40000000)
		fileSizeStr.Format("%.3f GByte", ((long double)fileSize) / 0x40000000);
	else if (fileSize >= 0x100000)
		fileSizeStr.Format("%3.3f MByte", ((long double)fileSize) / 0x100000);
	else if (fileSize >= 0x400)
		fileSizeStr.Format("%3.3f KByte", ((long double)fileSize) / 0x400);
	else
		fileSizeStr.Format("%d Byte", fileSize);
}

BSArchive::EResultError FIXAPI BSArchive::hk_LoadArchive(LPVOID arrayDataList, BSResourceLooseFile*& resFile, LPVOID Unk1, DWORD Unk2) {
	auto fileName = resFile->FileName->Get<CHAR>(TRUE);
	AssertMsg(fileName, "There is no name of the load archive");

	BSString filePath, fileSizeStr;
	filePath.Format("%s%s%s", resFile->AppPath->Get<CHAR>(TRUE), resFile->DataPath->Get<CHAR>(TRUE), fileName);
	AssertMsgVa(std::filesystem::exists(*filePath), "Can't found file %s", *filePath);

	auto fileSize = std::filesystem::file_size(*filePath);
	AssertMsgVa(fileSize < 0x100000000, "It is not possible to load a file '%s' larger than 4 GB", fileName);

	GetFileSizeStr(fileSize, fileSizeStr);
	
	_MESSAGE_FMT("Load an archive file \"%s\" (%s)...", fileName, *fileSizeStr);

	auto resultNo = fastCall<EResultError, LPVOID, BSResourceLooseFile*&, LPVOID, DWORD>(0x24C59D0, arrayDataList, resFile, Unk1, Unk2);
	AssertMsgVa(resultNo == EC_NONE, "Failed load an archive file %s", fileName);

	return resultNo;
}

BOOL FIXAPI BSArchive::hk_Check64bitSize(LPCSTR fileName, DWORD& fileSize) {
	WIN32_FILE_ATTRIBUTE_DATA fileData;
	if (GetFileAttributesExA(fileName, GetFileExInfoStandard, &fileData)) {

		if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY ||
			fileData.dwFileAttributes == INVALID_FILE_ATTRIBUTES)
			return FALSE;
		
		if (fileData.nFileSizeHigh) {
			DWORD64 size = fileData.nFileSizeLow | ((DWORD64)fileData.nFileSizeHigh << 32);
			BSString fileSizeStr;

			GetFileSizeStr(size, fileSizeStr);

			std::string fileShortName = fileName;
			auto del = fileShortName.find_last_of("\\/");
			if (del != fileShortName.npos)
				fileShortName = fileShortName.substr(del + 1);

			_MESSAGE_FMT("Skip load an archive file \"%s\". The archive has a size of more than 4GB (%s)...", fileShortName.c_str(), *fileSizeStr);

			return FALSE;
		}

		fileSize = fileData.nFileSizeLow;
		return TRUE;
	}

	return FALSE;
}