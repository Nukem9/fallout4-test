#include "../../common.h"
#include "TESFile_CK.h"

std::string TESFile_CK::GetFileName(void) const
{
	return m_FileName;
}

std::string TESFile_CK::GetFilePath(void) const
{
	return m_FilePath;
}