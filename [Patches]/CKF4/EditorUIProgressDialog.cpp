//////////////////////////////////////////
/*
* Copyright (c) 2020-2022 Perchik71 <email:perchik71@outlook.com>
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

#include "UIBaseWindow.h"
#include "UIProgressDialog.h"
#include "EditorUI.h"
#include "EditorUIProgressDialog.h"
#include "CellViewWindow.h"
#include "ObjectWindow.h"
#include "RenderWindow.h"
#include "MainWindow.h"

namespace EditorUI
{
	namespace sys = Core::Classes::UI;

	BOOL FIXAPI CloseProcessingHandlerProgressDlg(VOID) {
		return sys::ProgressDialog != NULL;
	}

	BOOL FIXAPI hk_CallLoadFile(TESDataHandler* io_handler, INT32 _zero_only) {
		// get and save F4FileDataHandler from CK
		// FileHandler = io_handler; 0x6D67960 and doesn't change

		// Replacing Tips with a progress Bar
		if (bReplaceTips) {

			auto wnd = &MainWindow::GetWindowObj();

			Assert(!sys::ProgressDialog);
			// show Progress
			sys::ProgressDialog = new sys::CUIProgressDialog(wnd, 3238);
			Assert(sys::ProgressDialog);
			sys::ProgressDialog->Create();
			// set Text
			sys::ProgressDialog->MessageText = "Loading Files...";
			
			// create progressbar on taskbar
			IProgressTaskBar::CreateInstance();
			
			// asunc processing message
			IProcessMessage::ProcessMessageAsync(NULL, 0, 0,
				IProcessMessage::NoYield | IProcessMessage::RemoveMessage | IProcessMessage::Loop,
				CloseProcessingHandlerProgressDlg);
		}

		// load
		return io_handler->Load(_zero_only);
	}

	VOID FIXAPI hk_EndLoadFile(VOID) {
		// Replacing Tips with a progress Bar
		if (bReplaceTips) {
			// close Progress
			delete sys::ProgressDialog;
			sys::ProgressDialog = NULL;
			
			// close Progress on taskbar
			IProgressTaskBar::DeleteInstance();
		}

		RenderWindow::GetWindowObj().Foreground();
		CellViewWindow::GetWindowObj().Foreground();

		ObjectWindow::OBJWNDS Wnds = ObjectWindow::GetAllWindowObj();

		for each (auto Wnd in Wnds) {
			if (Wnd.second->ObjectWindow.Visible)
				Wnd.second->ObjectWindow.Foreground();
		}
	}

	VOID FIXAPI hk_StepItProgress(LPCSTR* str) {
		Assert(sys::ProgressDialog);

		// set position 0..95%
		std::string s(MainWindow::GetWindowObj().GetTextToStatusBarA(3));

		s.assign(s.begin() + s.find('%') - 2, s.end());
		auto complete = strtol(s.c_str(), NULL, 10);
		sys::ProgressDialog->Position = complete;

		IProgressTaskBar::GetInstance()->SetPosition(complete);
	}

	//BOOL FIXAPI hk_UpdateProgress(LPVOID __this, INT32 __1)
	//{
	//	// Fallout CK is generally cut out.....

	//	static FLOAT lastPercent = 0.0f;

	//	// Only update every quarter percent, rather than every single form load
	//	FLOAT newPercent = ((FLOAT) * (UINT32*)OFFSET(0x6D6A7D8, 0) / (FLOAT) * (UINT32*)OFFSET(0x6D6A7D4, 0)) * 100.0f;

	//	if (abs(lastPercent - newPercent) > 0.50f)
	//		Core::Classes::UI::CUIMainWindow::ProcessMessages();

	//	return ((BOOL(__fastcall*)(LPVOID __this, INT32 __1))OFFSET(0x8027C0, 0))(__this, __1);
	//}

	VOID FIXAPI hk_SetTextAndSendStatusBar(UINT32 index, LPCSTR message) {
		// send Status Bar
		((VOID(__stdcall*)(UINT32, LPCSTR))OFFSET(0x5FDFE0, 0))(index, message);
		// set Text 
		sys::ProgressDialog->MessageText = message;
		Core::Classes::UI::CUIMainWindow::ProcessMessages();
	}

	//VOID FIXAPI SendFromCellViewToRender(VOID)
	//{
	//	auto wnd = &MainWindow::GetWindowObj();

	//	Assert(!sys::ProgressDialog);
	//	// show Progress
	//	sys::ProgressDialog = new sys::CUIProgressDialog(wnd, 3238);
	//	Assert(sys::ProgressDialog);
	//	sys::ProgressDialog->Create();
	//	// create progressbar on taskbar
	//	g_pContextProgressOnTaskBar = new ContextProgressOnTaskBar();
	//	Assert(g_pContextProgressOnTaskBar);
	//	g_pProgressOnTaskBar = new ProgressOnTaskBar(wnd->Handle, sys::ProgressDialog->Max - sys::ProgressDialog->Min);
	//	Assert(g_pProgressOnTaskBar);

	//	sys::ProgressDialog->Marquee = TRUE;
	//	sys::ProgressDialog->MessageText = "Please wait while requested cell loads ...";
	//	g_pProgressOnTaskBar->SetMarquee(TRUE);
	//}

	//VOID FIXAPI EndSendFromCellViewToRender(VOID)
	//{
	//	// Replacing Tips with a progress Bar
	//	if (bReplaceTips)
	//	{
	//		// close Progress
	//		delete sys::ProgressDialog;
	//		sys::ProgressDialog = NULL;
	//		// close Progress on taskbar
	//		delete g_pProgressOnTaskBar;
	//		g_pProgressOnTaskBar = NULL;
	//		delete g_pContextProgressOnTaskBar;
	//		g_pContextProgressOnTaskBar = NULL;
	//	}

	//	// enabled all markers
	//	Core::Classes::UI::CUIMenuItem MenuItem = MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(15);
	//	if (!MenuItem.Checked)
	//		MenuItem.Click();

	//	// enabled light markers
	//	MenuItem = MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(16);
	//	if (!MenuItem.Checked)
	//		MenuItem.Click();

	//	// enabled sound markers
	//	MenuItem = MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(17);
	//	if (!MenuItem.Checked)
	//		MenuItem.Click();

	//	RenderWindow::GetWindowObj().Foreground();
	//}

	///////////////////////////////////////

	VOID FIXAPI SetMarqueeInTaskbar(BOOL _value) {
		IProgressTaskBar::GetInstance()->SetMarquee(_value);
	}

	namespace SimpleDlgPleaseWait {
		class CUISimpleDlgPleaseWait;
		CUISimpleDlgPleaseWait* g_SimpleDlg;

		LRESULT WINAPI sdDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		class CUISimpleDlgPleaseWait : public Core::Classes::UI::CUICustomDialog {
		private:
			Core::Classes::UI::CUIBaseControl Text;
		public:
			CUISimpleDlgPleaseWait(CUICustomWindow* parent, const UINT res_id) :
				Core::Classes::UI::CUICustomDialog(parent, res_id, sdDlgProc) {}
			~CUISimpleDlgPleaseWait(VOID) {
				FreeRelease();
			}
		public:
			LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
				switch (uMsg) {
					case WM_INITDIALOG: {
						m_hWnd = hWnd;
						Text = GetControl(6027);

						std::vector<std::string> tips;

						auto fileName = BSString::Utils::GetApplicationPath() + "EditorTips.txt";
						if (BSString::Utils::FileExists(fileName)) {
							std::ifstream ifs(*fileName);

							CHAR szBuff[2048];
							while (ifs.good()) {
								ifs.getline(szBuff, 2048);

								if (szBuff[0] == '^')
									continue;

								tips.emplace_back(szBuff);
							}
							
							ifs.close();
						}

						auto tipsSize = tips.size();
						if (tipsSize > 0)
							Text.Caption = *BSString::FormatString("PLEASE WAIT...\n%s", tips[rand() % (tipsSize - 1)].c_str());
						else
							Text.Caption = "PLEASE WAIT...";

						Refresh();

						auto rWndRnd = RenderWindow::GetWindowObj().WindowRect();
						auto rWnd = WindowRect();

						SetWindowPos(hWnd, HWND_BOTTOM, 
							((rWndRnd.Width + rWnd.Width) >> 1) + rWnd.Left, 
							((rWndRnd.Height + rWnd.Height) >> 1) + rWnd.Top,
							0, 0, SWP_NOSIZE);
						SetForegroundWindow(hWnd);
						SetActiveWindow(hWnd);

						//MainWindow::GetWindowObj().ProcessMessages();
						break;
					}
				}

				return FALSE;
			}
		};

		LRESULT WINAPI sdDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			if (g_SimpleDlg)
				return g_SimpleDlg->WndProc(hWnd, uMsg, wParam, lParam);

			return TRUE;
		}

		VOID FIXAPI Show(VOID) {
			if (g_SimpleDlg)
				return;

			g_SimpleDlg = new CUISimpleDlgPleaseWait(&MainWindow::GetWindowObj(), 500);
			g_SimpleDlg->Create();
		}

		VOID FIXAPI Close(VOID) {
			if (g_SimpleDlg) {
				delete g_SimpleDlg;
				g_SimpleDlg = NULL;
			}
		}
	}
}
