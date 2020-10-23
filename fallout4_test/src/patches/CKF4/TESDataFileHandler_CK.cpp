#include "../../common.h"
#include "TESDataFileHandler_CK.h"
#include "UIProgressDialog.h"
#include "LogWindow.h"

TESDataFileHandler_CK* FileHandler;
TESFile_CK* ActiveFilePlugin;
TESFiles_CK Dependences;

// CF4FileDataHandler

bool TESDataFileHandler_CK::Load(int Unknown)
{
	ActiveFilePlugin = nullptr;
	Dependences.clear();

	// loads, checks.
	return ((bool(__fastcall*)(TESDataFileHandler_CK*, int))OFFSET(0x7D9D80, 0))(this, Unknown);
}

bool TESDataFileHandler_CK::InitUnknownDataSetTextStatusBar(void)
{
	// Replacing Tips with a progress Bar
	if (g_INI.GetBoolean("CreationKit", "ReplacingTipsWithProgressBar", false))
	{
		// set to progressbar
		Core::Classes::UI::ProgressDialog->MessageText = "Loading Files... Initializing...";
		Core::Classes::UI::ProgressDialog->Marquee = TRUE;
	}

	// Unknown. Initializes something.
	return ((bool(__fastcall*)(TESDataFileHandler_CK*))OFFSET(0x7D66A0, 0))(this);
}

TESFile_CK* TESDataFileHandler_CK::GetActiveFile(void) const
{
	return (TESFile_CK*)(((char*)this) + 0xFA8);
}

TESFiles_CK& TESDataFileHandler_CK::GetDependences(void) const
{
	return Dependences;
}

void WINAPI hk_InputToLogLoadFile(TESFile_CK* File)
{
	/*if (FileHandler->GetActiveFile() == File)
	{
		LogWindow::Log("Load active file %s...", File->FileName.c_str());
	}*/

	/*if (TESFile_CK::FILE_RECORD_ESM == File->Type)					// Master plugin
		LogWindow::Log("Load master file %s...", File->FileName.c_str());
	else if (TESFile_CK::FILE_RECORD_ACTIVE == File->Type)			// Active plugin
	{
		LogWindow::Log("Load active file %s...", File->FileName.c_str());
		ActiveFilePlugin = File;
	}
	else if (TESFile_CK::FILE_RECORD_ESL == File->Type)				// Small master file
		LogWindow::Log("Load small master file %s...", File->FileName.c_str());
	else*/
		LogWindow::Log("Load file %s...", File->FileName.c_str());

		/*
	if (TESFile_CK::FILE_RECORD_ACTIVE != File->Type)
	{
		if (std::find(Dependences.begin(), Dependences.end(), File) == Dependences.end())
			Dependences.push_back(File);
	}*/
		
	((void(__fastcall*)(TESFile_CK*))OFFSET(0x7FFF10, 0))(File);
}

BOOL WINAPI hk_InputToLogActiveFile(TESFile_CK* File, int Unknown)
{
	ActiveFilePlugin = File;

	LogWindow::Log("Load active file %s...", File->GetFileName().c_str());

	return ((BOOL(__fastcall*)(TESFile_CK*, int))OFFSET(0x801A90, 0))(File, Unknown);
}

BOOL WINAPI hk_InputToLogOneOfDependencesActiveFile(TESFile_CK* File, int Unknown)
{
	Dependences.push_back(File);

	LogWindow::Log("Load dependency file %s...", File->GetFileName().c_str());

	return ((BOOL(__fastcall*)(TESFile_CK*, int))OFFSET(0x801A90, 0))(File, Unknown);
}





