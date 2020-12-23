#pragma once

#include "UIBaseWindow.h"
#include "TESDataFileHandler_CK.h"

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
				int SafePos;
				UINT32 m_Min, m_Max;
				std::string tmp_str;
				CUIBaseControl m_Progress;
				CUIBaseControl m_Label;
			public:
				LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
				void SetMinAndMax(const UINT16 min, const UINT16 max);
			public:
				UINT16 GetMin(void);
				void SetMin(const UINT16 value);
				UINT16 GetMax(void);
				void SetMax(const UINT16 value);
				UINT16 GetIncrement(void);
				void SetIncrement(const UINT16 value);
				UINT16 GetPosition(void);
				void SetPosition(const UINT16 value);
				void SetMarquee(const BOOL value);
				BOOL GetMarquee(void);
				std::string GetMessageText(void);
				void SetMessageText(const std::string &str);
				void Step(void);
				static void ProcessMessages(void);
			public:
				PROPERTY(GetMin, SetMin) UINT16 Min;
				PROPERTY(GetMax, SetMax) UINT16 Max;
				PROPERTY(GetIncrement, SetIncrement) UINT16 Increment;
				PROPERTY(GetPosition, SetPosition) UINT16 Position;
				PROPERTY(GetMarquee, SetMarquee) BOOL Marquee;
				PROPERTY(GetMessageText, SetMessageText) std::string MessageText;
			public:
				CUIProgressDialog(CUICustomWindow* parent, const UINT res_id);
				~CUIProgressDialog(void);
			};

			extern CUIProgressDialog* ProgressDialog;
		}
	}
}