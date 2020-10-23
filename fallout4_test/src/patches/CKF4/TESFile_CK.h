#pragma once

#include <atomic>
#include <string>

#pragma pack(push, 1)

class TESFile_CK
{
private:
	TESFile_CK();
	~TESFile_CK();
public:
	enum Type_t: uint32_t
	{
		FILE_RECORD_ESM			= 0x1,			// Master plugin
		FILE_RECORD_CHECKED		= 0x4,			// Pending load/loaded
		FILE_RECORD_ACTIVE		= 0x8,			// Save target
		FILE_RECORD_LOCALIZED	= 0x80,			// Strings removed
		FILE_RECORD_ESL			= 0x200,		// Small file
	};
private:
	char _pad0[0x78];
	char m_FileName[MAX_PATH];
	char m_FilePath[MAX_PATH];
	//char _pad1[0x9];
	//Type_t m_Type;
public:
	std::string GetFileName(void) const { return m_FileName; }
	std::string GetFilePath(void) const { return m_FilePath; }
	//Type_t GetType(void) const { return m_Type; }
public:
	__declspec(property(get = GetFileName)) std::string FileName;
	__declspec(property(get = GetFilePath)) std::string FilePath;
	//__declspec(property(get = GetType)) Type_t Type;
};

#pragma pack(pop)