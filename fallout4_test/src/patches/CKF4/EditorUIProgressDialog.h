#pragma once

#include <atomic>
#include "TESDataFileHandler_CK.h"

namespace EditorUI
{
	bool __stdcall hk_CallLoadFile(TESDataFileHandler_CK* io_handler, int _zero_only);
	void __stdcall hk_EndLoadFile(void);
	void __stdcall hk_StepItProgress(const char** str);
	bool __stdcall hk_UpdateProgress(void* __this, int __1);
	void __stdcall hk_SetTextAndSendStatusBar(const uint32_t index, const char* message);
	void __stdcall hk_SendFromCellViewToRender(void* Unknown1, void* Unknown2, int Unknown3);
	void __stdcall hk_EndSendFromCellViewToRender(void);
}