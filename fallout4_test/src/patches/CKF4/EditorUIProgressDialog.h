//////////////////////////////////////////
/*
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

#include "../../common.h"
#include "TESObjects/TES.h"

namespace EditorUI
{
	BOOL FIXAPI hk_CallLoadFile(TESDataFileHandler_CK* io_handler, INT32 _zero_only);
	VOID FIXAPI hk_EndLoadFile(VOID);
	VOID FIXAPI hk_StepItProgress(LPCSTR* str);
	BOOL FIXAPI hk_UpdateProgress(LPVOID __this, INT32 __1);
	VOID FIXAPI hk_SetTextAndSendStatusBar(UINT32 index, LPCSTR message);
	VOID FIXAPI hk_SendFromCellViewToRender(LPVOID Unknown1, TESForm_CK* View, INT32 Unknown3);
	VOID FIXAPI hk_EndSendFromCellViewToRender(VOID);

	// Methods of the progress indicator displayed on a taskbar button.

	VOID FIXAPI SetMarqueeInTaskbar(BOOL _value);
}