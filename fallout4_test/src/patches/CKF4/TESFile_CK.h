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

#include "..\ComClasses.h"

#pragma pack(push, 1)

class TESFile_CK
{
private:
	TESFile_CK();
	~TESFile_CK();
public:
	enum : uint32_t
	{
		FILE_RECORD_ESM			= 0x1,			// Master plugin
		FILE_RECORD_CHECKED		= 0x4,			// Pending load/loaded
		FILE_RECORD_ACTIVE		= 0x8,			// Save target
		FILE_RECORD_LOCALIZED	= 0x80,			// Strings removed
		FILE_RECORD_ESL			= 0x200,		// Small file
	};
private:
	CHAR _pad0[0x78];
	CHAR m_FileName[MAX_PATH];
	CHAR m_FilePath[MAX_PATH];
	CHAR _pad1[0xBC];
	UINT32 m_Flags;
public:
	inline std::string GetFileName(VOID) const { return m_FileName ? m_FileName : ""; }
	inline std::string GetFilePath(VOID) const { return m_FilePath ? m_FilePath : ""; }
	inline UINT32 GetFlags(VOID) const { return m_Flags; }
	inline BOOL IsMaster(VOID) const { return m_Flags & FILE_RECORD_ESM; }
	inline BOOL IsSelected(VOID) const { return m_Flags & FILE_RECORD_CHECKED; }
	inline BOOL IsActive(VOID) const { return m_Flags & FILE_RECORD_ACTIVE; }
	inline BOOL IsLocalized(VOID) const { return m_Flags & FILE_RECORD_LOCALIZED; }
	inline BOOL IsSmallMaster(VOID) const { return m_Flags & FILE_RECORD_ESL; }
public:
	inline static INT32(*LoadTESInfo)(TESFile_CK*);
	inline static INT64(*WriteTESInfo)(TESFile_CK*);
	INT32 hk_LoadTESInfo(VOID);
	INT64 hk_WriteTESInfo(VOID);
	BOOL IsActiveFileBlacklist(VOID);
public:
	inline static BOOL AllowSaveESM;
	inline static BOOL AllowMasterESP;
public:
	READ_PROPERTY(GetFileName) std::string FileName;
	READ_PROPERTY(GetFilePath) std::string FilePath;
	READ_PROPERTY(GetFlags) UINT32 Flags;
};

#pragma pack(pop)