#include "..\..\Common.h"
#include "EditorUI.h"
#include "UIProgressDialog.h"
#include "LogWindow.h"
#include <CommCtrl.h>

#define MIN_STATUSBAR_VALUE 0
#define MAX_STATUSBAR_VALUE 94						// to 95%
#define UI_CONTROL_ID_LABEL_DLGPROGRESS		2217
#define UI_CONTROL_ID_PROGRESS_DLGPROGRESS	31007

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			CUIProgressDialog* ProgressDialog = nullptr;

			LRESULT WINAPI DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{	
				if(ProgressDialog)
					return ProgressDialog->WndProc(hWnd, uMsg, wParam, lParam);

				return TRUE;
			}

			CUIProgressDialog::CUIProgressDialog(CUICustomWindow* parent, const UINT res_id) :
				CUICustomDialog(parent, res_id, DlgProc)
			{}

			CUIProgressDialog::~CUIProgressDialog(void)
			{
				SetMarquee(FALSE);
				FreeRelease();
			}

			void CUIProgressDialog::SetMinAndMax(const UINT16 min, const UINT16 max)
			{
				m_Min = std::max(min, (UINT16)0);
				m_Max = std::max(max, (UINT16)1);

				Assert(m_Max != m_Min);

				m_Progress.Perform(PBM_SETRANGE, 0, MAKELPARAM(m_Min, m_Max));
				m_Progress.Perform(PBM_SETPOS, min, 0);
			}

			UINT16 CUIProgressDialog::GetPosition(void)
			{
				return (UINT16)m_Progress.Perform(PBM_GETPOS, 0, 0);
			}

			void CUIProgressDialog::SetPosition(const UINT16 value)
			{
				if (m_Marquee) return;
				m_Progress.Perform(PBM_SETPOS, value, 0);
				Core::Classes::UI::CUIMainWindow::ProcessMessages();
			}

			void CUIProgressDialog::SetMarquee(const BOOL value)
			{
				if (m_Marquee == value) return;

				m_Marquee = value;
				if (m_Marquee)
				{
					m_Progress.Style = WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_MARQUEE;
					SafePos = GetPosition();
					SetPosition(0);
					SetTimer(m_hWnd, UI_PROGRESS_CMD_STEPIT, 25, NULL);
				}
				else
				{
					KillTimer(m_hWnd, UI_PROGRESS_CMD_STEPIT);
					m_Progress.Style = WS_CHILD | WS_VISIBLE | PBS_SMOOTH;
					SetPosition(SafePos);
				}

				m_Progress.Perform(PBM_SETMARQUEE, (WPARAM)m_Marquee, NULL);
			}

			BOOL CUIProgressDialog::GetMarquee(void)
			{
				return m_Marquee;
			}

			std::string CUIProgressDialog::GetMessageText(void)
			{
				return m_Label.Caption;
			}

			void CUIProgressDialog::SetMessageText(const std::string &str)
			{
				m_Label.Caption = str;
			}

			UINT16 CUIProgressDialog::GetMin(void)
			{
				return m_Min;
			}

			void CUIProgressDialog::SetMin(const UINT16 value)
			{
				UINT16 m_newMin = std::max(value, (UINT16)0);
				if (m_newMin == value) 
					return;

				m_Min = m_newMin;
				Assert(m_Max != m_Min);

				m_Progress.Perform(PBM_SETRANGE, 0, MAKELPARAM(m_Min, m_Max));
			}

			UINT16 CUIProgressDialog::GetMax(void)
			{
				return m_Max;
			}

			void CUIProgressDialog::SetMax(const UINT16 value)
			{
				UINT16 m_newMax = std::max(value, (UINT16)1);
				if (m_newMax == value)
					return;

				m_Max = m_newMax;
				Assert(m_Max != m_Min);

				m_Progress.Perform(PBM_SETRANGE, 0, MAKELPARAM(m_Min, m_Max));
			}

			UINT16 CUIProgressDialog::GetIncrement(void)
			{
				return m_Progress.Perform(PBM_GETSTEP, 0, 0);
			}

			void CUIProgressDialog::SetIncrement(const UINT16 value)
			{
				m_Progress.Perform(PBM_SETSTEP, value, 0);
			}

			void CUIProgressDialog::Step(void)
			{
				m_Progress.Perform(PBM_STEPIT, 0, 0);
			}

			void CUIProgressDialog::ProcessMessages(void)
			{
				if (ProgressDialog)
					Core::Classes::UI::CUIMainWindow::ProcessMessages();
				else
					Sleep(1);
			}

			LRESULT CUIProgressDialog::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				switch (uMsg)
				{
					case WM_INITDIALOG:
					{
						m_hWnd = hWnd;
						m_Marquee = FALSE;
						m_Progress = GetControl(UI_CONTROL_ID_PROGRESS_DLGPROGRESS);
						m_Label = GetControl(UI_CONTROL_ID_LABEL_DLGPROGRESS);
						SetMinAndMax(MIN_STATUSBAR_VALUE, MAX_STATUSBAR_VALUE);
						MessageText = "";
						Increment = 1;

						break;
					}

					case UI_PROGRESS_CMD_STEPIT:
					{
						Step();

						break;
					}
				}

				return FALSE;
			}
		}
	}
}

namespace EditorUI
{
	namespace sys = Core::Classes::UI;

	bool __stdcall hk_CallLoadFile(TESDataFileHandler_CK* io_handler, int _zero_only)
	{
		// get and save F4FileDataHandler from CK
		// FileHandler = io_handler; 0x6D67960 and doesn't change

		// Replacing Tips with a progress Bar
		if (g_INI.GetBoolean("CreationKit", "UI", false) && g_INI.GetBoolean("CreationKit", "ReplacingTipsWithProgressBar", false))
		{
			Assert(!sys::ProgressDialog);
			// show Progress
			sys::ProgressDialog = new sys::CUIProgressDialog(&GetMainWindowObj(), 3238);
			Assert(sys::ProgressDialog);
			sys::ProgressDialog->Create();
			// set Text 
			sys::ProgressDialog->MessageText = "Loading Files...";
		}

		// load
		return io_handler->Load(_zero_only);
	}

	void __stdcall hk_EndLoadFile(void)
	{
		// Replacing Tips with a progress Bar
		if (g_INI.GetBoolean("CreationKit", "UI", false) && g_INI.GetBoolean("CreationKit", "ReplacingTipsWithProgressBar", false))
		{
			// close Progress
			delete sys::ProgressDialog;
			sys::ProgressDialog = nullptr;
		}

		GetObjectWindowObj().Foreground();
		GetCellViewWindowObj().Foreground();
	}

	void __stdcall hk_StepItProgress(const char** str)
	{
		Assert(sys::ProgressDialog);

		// set position 0..95%
		std::string s(*str);
		s.assign(s.begin() + s.find('%') - 2, s.end());
		sys::ProgressDialog->Position = strtol(s.c_str(), nullptr, 10);
	}

	bool __stdcall hk_UpdateProgress(void* __this, int __1)
	{
		// Fallout CK is generally cut out.....

		static float lastPercent = 0.0f;

		// Only update every quarter percent, rather than every single form load
		float newPercent = ((float)*(uint32_t*)OFFSET(0x6D6A7D8, 0) / (float)*(uint32_t*)OFFSET(0x6D6A7D4, 0)) * 100.0f;

		if (abs(lastPercent - newPercent) > 0.25f)
			Core::Classes::UI::CUIMainWindow::ProcessMessages();

		return ((bool(__fastcall*)(void* __this, int __1))OFFSET(0x8027C0, 0))(__this, __1);
	}

	void __stdcall hk_SetTextAndSendStatusBar(const uint32_t index, const char* message)
	{
		// send Status Bar
		((void(__stdcall*)(const uint32_t, const char*))OFFSET(0x5FDFE0, 0))(index, message);
		// set Text 
		sys::ProgressDialog->MessageText = message;
		Core::Classes::UI::CUIMainWindow::ProcessMessages();
	}

	void __stdcall hk_SendFromCellViewToRender(void* Unknown1, void* Unknown2, int Unknown3)
	{
		Assert(!sys::ProgressDialog);
		// show Progress
		sys::ProgressDialog = new sys::CUIProgressDialog(&GetMainWindowObj(), 3238);
		Assert(sys::ProgressDialog);
		sys::ProgressDialog->Create();

		sys::ProgressDialog->Marquee = TRUE;
		sys::ProgressDialog->MessageText = "Please wait while requested cell loads ...";
		
		// send
		((void(__fastcall*)(void*, void*, int))OFFSET(0x45FE60, 0))(Unknown1, Unknown2, Unknown3);
	}

	void __stdcall hk_EndSendFromCellViewToRender(void)
	{
		// Replacing Tips with a progress Bar
		if (g_INI.GetBoolean("CreationKit", "ReplacingTipsWithProgressBar", false))
		{
			// close Progress
			delete sys::ProgressDialog;
			sys::ProgressDialog = nullptr;
		}

		// enabled all markers
		Core::Classes::UI::CUIMenuItem MenuItem = GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(15);
		if (!MenuItem.Checked)
			MenuItem.Click();

		// enabled light markers
		MenuItem = GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(16);
		if (!MenuItem.Checked)
			MenuItem.Click();

		// enabled sound markers
		MenuItem = GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(17);
		if (!MenuItem.Checked)
			MenuItem.Click();
	}
}
