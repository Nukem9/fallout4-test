//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

#include "../StdAfx.h"
#include "NiMain/GameTypes.h"

#pragma pack(push, 1)

namespace api {
	class TESFile {
	private:
		TESFile(VOID);
		~TESFile(VOID);
	public:
		enum : DWORD {
			FILE_RECORD_ESM = 0x1,			// Master plugin
			FILE_RECORD_CHECKED = 0x4,		// Pending load/loaded
			FILE_RECORD_ACTIVE = 0x8,		// Save target
			FILE_RECORD_LOCALIZED = 0x80,	// Strings removed
			FILE_RECORD_ESL = 0x200,		// Small file
		};
	private:
		/* 000 */ CHAR _pad0[0x78];
		/* 078 */ CHAR m_FileName[MAX_PATH];
		/* 17C */ CHAR m_FilePath[MAX_PATH];
		/* 280 */ LPVOID m_UnkPtr280;
		/* 288 */ DWORD m_bufferSize;					// buffer size used when opening BSFile. init to 0x800
		/* 28C */ CHAR _pad1[0x38];
		/* 2C4 */ DWORD m_fileSize;						// same as FileSizeLow in find data
		/* 2C8 */ DWORD m_currentRecordOffset;			// offset of current record in file
		/* 2CC */ DWORD m_currentChunkOffset;			// offset of current chunk in record
		/* 2D0 */ DWORD m_fetchedChunkDataSize;			// number of bytes read in last GetChunkData() call
		/* 2D4 */ CHAR _pad2[0x34];
		/* 308 */ BOOL m_headerRead;					// set after header has been successfully parsed
		
		struct WIN32_FIND_DATA_UNKNOWN {
			CHAR _pad0[0xC];
			FILETIME ftCreationTime;
			FILETIME ftLastWriteTime;
			DWORD nFileSizeHigh;
			DWORD nFileSizeLow;
		};
		
		/* 30C */ WIN32_FIND_DATA_UNKNOWN m_findData;

		struct FileHeaderInfo {
			/*00*/ FLOAT fileVersion;
			/*04*/ DWORD numRecords;		// number of record blocks in file
			/*08*/ DWORD nextFormID;		// including file index in highest byte
		};

		/* 330 */ FileHeaderInfo m_fileHeader;
		/* 33C */ DWORD m_Flags;

		// Data for Depend files - compared against size in findData of masters to check if they have changed since last edit
		// 08
		struct DependFileData {
			/*00*/ DWORD nFileSizeHigh;
			/*04*/ DWORD nFileSizeLow;
		};
		typedef BSSimpleList<DependFileData*> DependDataList;
		typedef BSSimpleList<LPCSTR> DependNameList;

		/* 340 */ DependNameList m_dependNames;
		/* 350 */ DependDataList m_dependData;
		/* 360 */ DWORD m_dependCount;
		/* 364 */ CHAR _pad3[0x4];
		/* 368 */ TESFile** m_dependArray;
		/* 370 */ CHAR _pad4[0x8];
		/* 378 */ CHAR m_fileIndex;						// index of this file in load order (or 0xFF if not loaded)
		/* 379 */ CHAR _pad5[0x7];
		/* 380 */ BSString m_authorName;
		/* 390 */ BSString m_description;
		/* 3A0 */ CHAR _pad6[0x6E0];
	public:
		BSString GetAuthorName(VOID) const;
		inline BSString GetDescription(VOID) const { return *m_description ? m_description : ""; }
		inline BSString GetFileName(VOID) const { return m_FileName ? m_FileName : ""; }
		inline BSString GetFilePath(VOID) const { return m_FilePath ? m_FilePath : ""; }
		inline CHAR GetFileLoadIndex(VOID) const { return m_fileIndex; }
		inline TESFile** GetDependArray(VOID) const { return m_dependArray; }
		inline DWORD GetDependCount(VOID) const { return m_dependCount; }
		inline DWORD GetFileSize(VOID) const { return m_fileSize; }
		inline DWORD GetFlags(VOID) const { return m_Flags; }
		inline BOOL IsLoaded(VOID) const { return m_fileIndex != 0xFF; }
		inline BOOL IsMaster(VOID) const { return m_Flags & FILE_RECORD_ESM; }
		inline BOOL IsSelected(VOID) const { return m_Flags & FILE_RECORD_CHECKED; }
		inline BOOL IsActive(VOID) const { return m_Flags & FILE_RECORD_ACTIVE; }
		inline BOOL IsLocalized(VOID) const { return m_Flags & FILE_RECORD_LOCALIZED; }
		inline BOOL IsSmallMaster(VOID) const { return m_Flags & FILE_RECORD_ESL; }
		SYSTEMTIME GetCreationTime(VOID) const;
		SYSTEMTIME GetLastWriteTime(VOID) const;
	public:
		inline static INT32(*LoadTESInfo)(TESFile*);
		inline static INT64(*WriteTESInfo)(TESFile*);
		INT32 hk_LoadTESInfo(VOID);
		INT64 hk_WriteTESInfo(VOID);
		BOOL IsMasterFileToBlacklist(VOID);
		VOID Dump(VOID);
	public:
		inline static BOOL AllowSaveESM;
		inline static BOOL AllowMasterESP;
	public:
		READ_PROPERTY(GetAuthorName) BSString AuthorName;
		READ_PROPERTY(GetDescription) BSString Description;
		READ_PROPERTY(GetFileName) BSString FileName;
		READ_PROPERTY(GetFilePath) BSString FilePath;
		READ_PROPERTY(GetFlags) DWORD Flags;
		READ_PROPERTY(GetFileLoadIndex) CHAR FileLoadIndex;
		READ_PROPERTY(GetDependArray) TESFile** DependArray;
		READ_PROPERTY(GetDependCount) DWORD DependCount;
		READ_PROPERTY(GetFileSize) DWORD FileSize;
		READ_PROPERTY(GetCreationTime) SYSTEMTIME CreationTime;
		READ_PROPERTY(GetLastWriteTime) SYSTEMTIME LastWriteTime;
	};

	static_assert(sizeof(TESFile) == 0xA80, "TESFile class should be the size of 0xA80");
}

#pragma pack(pop)