#pragma once

#include "TESFile_CK.h"
#include <vector>

typedef std::vector<TESFile_CK*> TESFiles_CK;

class TESDataFileHandler_CK
{
public:
	bool Load(int Unknown);
	bool InitUnknownDataSetTextStatusBar(void);

	TESFile_CK* GetActiveFile(void) const;
	TESFiles_CK& GetDependences(void) const;
};

// 0x6D67960 and doesn't change hmm...
extern TESDataFileHandler_CK* FileHandler;

void WINAPI hk_InputToLogLoadFile(TESFile_CK* File);
BOOL WINAPI hk_InputToLogOneOfDependencesActiveFile(TESFile_CK* File, int Unknown);