#pragma once

#include <atomic>
#include <string>

class TESFile_CK
{
private:
	TESFile_CK();
	~TESFile_CK();
private:
	char _pad0[0x78];
	char m_FileName[MAX_PATH];
	char m_FilePath[MAX_PATH];
	char _pad1[0x60];
public:
	std::string GetFileName(void) const;
	std::string GetFilePath(void) const;
public:
	__declspec(property(get = GetFileName)) std::string FileName;
	__declspec(property(get = GetFilePath)) std::string FilePath;
};