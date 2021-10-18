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

#include "../../Common.h"
#include "UIBaseWindow.h"
#include "TESObjects/TES.h"

#define UI_PROGRESS_CMD_STEPIT	(UI_CUSTOM_MESSAGE + 5)

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			class CUIProgressDialog : public CUICustomDialog
			{
			private:
				ULONG_PTR m_OldStyles;
				BOOL m_Marquee;
				INT32 SafePos;
				UINT32 m_Min, m_Max;
				std::string tmp_str;
				CUIBaseControl m_Progress;
				CUIBaseControl m_Label;
			public:
				LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
				VOID SetMinAndMax(const UINT16 min, const UINT16 max);
			public:
				UINT16 GetMin(VOID);
				VOID SetMin(const UINT16 value);
				UINT16 GetMax(VOID);
				VOID SetMax(const UINT16 value);
				UINT16 GetIncrement(VOID);
				VOID SetIncrement(const UINT16 value);
				UINT16 GetPosition(VOID);
				VOID SetPosition(const UINT16 value);
				VOID SetMarquee(const BOOL value);
				BOOL GetMarquee(VOID);
				std::string GetMessageText(VOID);
				VOID SetMessageText(const std::string &str);
				VOID Step(VOID);
				static VOID FIXAPI ProcessMessages(VOID);
				static VOID FIXAPI ProcessMessagesOnlyLoadCellWorld(VOID);
			public:
				PROPERTY(GetMin, SetMin) UINT16 Min;
				PROPERTY(GetMax, SetMax) UINT16 Max;
				PROPERTY(GetIncrement, SetIncrement) UINT16 Increment;
				PROPERTY(GetPosition, SetPosition) UINT16 Position;
				PROPERTY(GetMarquee, SetMarquee) BOOL Marquee;
				PROPERTY(GetMessageText, SetMessageText) std::string MessageText;
			public:
				CUIProgressDialog(CUICustomWindow* parent, const UINT res_id);
				~CUIProgressDialog(VOID);
			};

			extern CUIProgressDialog* ProgressDialog;
		}
	}
}