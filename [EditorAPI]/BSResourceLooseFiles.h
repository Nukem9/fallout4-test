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
#include "..\[Common]\Utilities.h"
#include "BSEntryString.h"
#include "BSString.h"

// There is some kind of structure in rtti, I will follow it.

namespace BSResource
{
	class LooseFileStream
	{
	public:
		virtual LooseFileStream* __dtor__(LooseFileStream* Thisptr, unsigned __int8 flag);
	protected:
		// As a result, I see that the files are limited to 4 GB
		/* 008 */ DWORD dwFileSize;
		/* 00C */ DWORD dwUnkC;					// Flags ??
		/* 010 */ BSEntryString* _AppPath;		// init "<DIR FALLOUT 4>"
		/* 018 */ BSEntryString* _DataPath;		// init "/Data"
		/* 020 */ BSEntryString* _FileName;		// for example "Homemaker - Main.ba2"
		/* 028 */ DWORD BufferSize;
		/* 02C */ DWORD dwUnk2C;
		/* 030 */ DWORD dwUnk30;
		/* 034 */ DWORD dwUnk34;
		/* 038 */ CHAR szFullName[0x100];
		/* 138 */ DWORD dwUnk138;
		/* 13C */ DWORD dwUnk13C;
		/* 140 */ DWORD dwUnk140;
		/* 144 */ DWORD dwUnk144;
		/* 148 */ LPVOID lpData;				// BSResource::LooseFileLocation
		/* 150 */ DWORD64 UsefulDataSize;
		/* 158 */ CHAR szUnk150[0x8];
		///////////////////////////////
		/// F4CK FIXED ADDED
		///////////////////////////////
		/* 160 */ DWORD64 dw64FileSize;			// If zero, then the file is not found or inside the archive
		/* 168 */ DWORD64 dw64Position;			// while unused
		/* 170 */ CHAR szReserved[0x10];
	public:
		static VOID FIXAPI CreateInstance(LPCSTR FileName, LooseFileStream** Instance);
	public:
		VOID Initialize64BitData(LPCSTR FileName);
	public:
		INLINE DWORD GetFileSize(VOID) const { return dwFileSize; }
		INLINE std::string GetFullName(VOID) const { return szFullName; }
		INLINE BSEntryString* GetAppPath(VOID) const { return _AppPath; }
		INLINE BSEntryString* GetDataPath(VOID) const { return _DataPath; }
		INLINE BSEntryString* GetFileName(VOID) const { return _FileName; }
	public:
		READ_PROPERTY(GetFileSize) DWORD FileSize;
		READ_PROPERTY(GetFullName) std::string FullName;
		READ_PROPERTY(GetAppPath) BSEntryString* AppPath;
		READ_PROPERTY(GetDataPath) BSEntryString* DataPath;
		READ_PROPERTY(GetFileName) BSEntryString* FileName;
	};

	static_assert(sizeof(LooseFileStream) == 0x180, "BSResource::LooseFileStream class should be the size of 0x180");
}

typedef BSResource::LooseFileStream BSResourceLooseFile;

//
//// BSResourceLooseFile is another file class that I can't find a connection with NiFile or BSFile.
//// This class is directly related to archives .ba2
//// This class will cause an CTD when reading more than 4 GB
//
//class BSResourceLooseFile {
//public:
//	BSResourceLooseFile(VOID) = default;
//	virtual ~BSResourceLooseFile(VOID) = default;
//protected:
//	// As a result, I see that the files are limited to 4 GB
//	/* 008 */ DWORD dwFileSize;
//	/* 00C */ DWORD dwUnkC;					// Flags ??
//	/* 010 */ BSEntryString* _AppPath;		// init "<DIR FALLOUT 4>"
//	/* 018 */ BSEntryString* _DataPath;		// init "/Data"
//	/* 020 */ BSEntryString* _FileName;		// for example "Homemaker - Main.ba2"
//	/* 028 */ DWORD BufferSize;
//	/* 02C */ DWORD dwUnk2C;
//	/* 030 */ DWORD dwUnk30;
//	/* 034 */ DWORD dwUnk34;
//	/* 038 */ CHAR szFullName[/*MAX_PATH*/0x100];
//	/* 138 */ DWORD dwUnk138;				
//	/* 13C */ DWORD dwUnk13C;
//	/* 140 */ DWORD dwUnk140;
//	/* 144 */ DWORD dwUnk144;
//	/* 148 */ LPVOID lpData;				// Something valuable, some kind of class with data
//	/* 150 */ CHAR szUnk150[0x30];
//public:
//	INLINE DWORD GetFileSize(VOID) const { return dwFileSize; }
//	INLINE std::string GetFullName(VOID) const { return szFullName; }
//	INLINE BSEntryString* GetAppPath(VOID) const { return _AppPath; }
//	INLINE BSEntryString* GetDataPath(VOID) const { return _DataPath; }
//	INLINE BSEntryString* GetFileName(VOID) const { return _FileName; }
//public:
//	READ_PROPERTY(GetFileSize) DWORD FileSize;
//	READ_PROPERTY(GetFullName) std::string FullName;
//	READ_PROPERTY(GetAppPath) BSEntryString* AppPath;
//	READ_PROPERTY(GetDataPath) BSEntryString* DataPath;
//	READ_PROPERTY(GetFileName) BSEntryString* FileName;
//};
//static_assert(sizeof(BSResourceLooseFile) == 0x180, "BSResourceLooseFile class should be the size of 0x180");


#pragma pack(pop)