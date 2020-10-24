#include "../../common.h"
#include "TESDataFileHandler_CK.h"
#include "UIProgressDialog.h"
#include "LogWindow.h"

#define XBYAK_NO_OP_NAMES
#include <xbyak/xbyak.h>

using namespace Xbyak;

TESDataFileHandler_CK* FileHandler;
TESDataFileHandler_CK::TESFileList_CK g_SelectedFilesList, *g_SelectedFilesListNodeLast;
TESDataFileHandler_CK::TESFileArray_CK g_SelectedFilesArray;

void TESDataFileHandler_CK::Initialize(void)
{
	FileHandler = (TESDataFileHandler_CK*)OFFSET(0x6D67960, 0);

	// Recognition of loaded files
	XUtil::DetourClassCall(OFFSET(0x801AA7, 0), &TESDataFileHandler_CK::DetectSelectFile);

	// The entire list of files is checked three times per load mod. 
	// I will do it only once, replace the register with the already created list of selected elements, this slightly optimizes the loading.

	const size_t codeSize = 4096;
	static uint8_t buf[codeSize + 16];
	static uint8_t* p = CodeArray::getAlignedAddress(buf);

	CodeArray::protect(p, codeSize, true);
	class changeLoadFileHook : public Xbyak::CodeGenerator
	{
	public:
		changeLoadFileHook() : Xbyak::CodeGenerator(codeSize, (void*)p)
		{
			mov(rax, (size_t)GetSelectedFiles);
			call(rax);
			mov(r14, rsi);
			mov(r12, rax);
			mov(rbx, rax);
			ret();
		}
	} static changeLoadFileHook;

	XUtil::DetourCall(OFFSET(0x7D9F02, 0), (uintptr_t)changeLoadFileHook.getCode());
	XUtil::DetourCall(OFFSET(0x7DA306, 0), (uintptr_t)changeLoadFileHook.getCode());
	XUtil::PatchMemory(OFFSET(0x7D9F08, 0), { 0x48, 0x85, 0xC0 });
}

bool TESDataFileHandler_CK::Load(int Unknown)
{
	g_SelectedFilesList.RemoveAllNodes();
	g_SelectedFilesListNodeLast = &g_SelectedFilesList;
	g_SelectedFilesArray.clear();

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

void TESDataFileHandler_CK::DetectSelectFile(TESFile_CK* File)
{
	// Sometimes duplicated
	if (std::find(g_SelectedFilesArray.begin(), g_SelectedFilesArray.end(), File) == g_SelectedFilesArray.end())
	{
		if (File->IsActive()) // Active plugin
		{
			LogWindow::Log("Load active file %s...", File->FileName.c_str());
		}
		else
		{
			LogWindow::Log("Load dependent file %s...", File->FileName.c_str());
		}

		g_SelectedFilesArray.push_back(File);
		g_SelectedFilesListNodeLast = g_SelectedFilesList.AddNode(g_SelectedFilesListNodeLast, File);
	}

	((void(__fastcall*)(TESFile_CK*))OFFSET(0x7FFF10, 0))(File);
}

TESDataFileHandler_CK::TESFileListPtr_CK TESDataFileHandler_CK::GetArchiveFiles(void)
{
	return (TESDataFileHandler_CK::TESFileListPtr_CK)OFFSET(0x6D68910, 0);
}

TESDataFileHandler_CK::TESFileListPtr_CK TESDataFileHandler_CK::GetSelectedFiles(void)
{
	// first always null data
	return g_SelectedFilesList.m_pkNext;
}

TESFile_CK* TESDataFileHandler_CK::GetActiveFile(void) const
{
	return (TESFile_CK*)(((char*)this) + 0xFA8);
}

bool TESDataFileHandler_CK::IsActiveFile(void) const
{
	return GetActiveFile() != nullptr;
}