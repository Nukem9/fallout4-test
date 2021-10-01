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

#include "../../common.h"
#include "UIBaseWindow.h"
#include "UIProgressDialog.h"
#include "EditorUI.h"
#include "EditorUIProgressDialog.h"
#include "CellViewWindow.h"
#include "ObjectWindow.h"
#include "RenderWindow.h"
#include "MainWindow.h"
#include "LogWindow.h"

#include "TESCellViewScene_CK.h"

#include <ShObjIdl_core.h>

namespace EditorUI
{
	namespace sys = Core::Classes::UI;

	// Let's make the program a little more modern, transfer the progress and TaskBar...
	// https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-itaskbarlist3

	class ContextProgressOnTaskBar {
	private:
		ITaskbarList3* m_ptbl;
	public:
		ContextProgressOnTaskBar(VOID) : m_ptbl(NULL) {
			ITaskbarList3* ptbl = NULL;
			if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ptbl)))) {
				if (SUCCEEDED(ptbl->HrInit()))
					m_ptbl = ptbl;
				else {
					ptbl->Release();
					_MESSAGE("ITaskbarList::HrInit() has failed.");
				}
			}
			else
				_MESSAGE("ITaskbarList could not be created.");
		}
		virtual ~ContextProgressOnTaskBar(VOID) {
			if (m_ptbl) {
				m_ptbl->Release();
			}
		}
	public:
		inline ITaskbarList3* operator->(VOID) { return m_ptbl; }
		inline const ITaskbarList3* operator->(VOID) const { return m_ptbl; }
	};
	static ContextProgressOnTaskBar* g_pContextProgressOnTaskBar = NULL;

	class ProgressOnTaskBar {
	private:
		HWND m_LinkWnd;
		BOOL m_Marquee;
		ULONGLONG m_ullCompleted;
		ULONGLONG m_ullTotal;
	public:
		ProgressOnTaskBar(HWND _LinkWnd, ULONGLONG _total) : m_LinkWnd(NULL), m_Marquee(FALSE), m_ullCompleted(0), m_ullTotal(std::max<ULONGLONG>(1, _total)) {
			if (!g_pContextProgressOnTaskBar)
				_MESSAGE("ContextProgressOnTaskBar not be created.");
			else if (!IsWindow(_LinkWnd))
				_MESSAGE("ProgressOnTaskBar could not be created, because _LinkWnd isn't window.");
			else {
				if (SUCCEEDED((*g_pContextProgressOnTaskBar)->SetProgressState(_LinkWnd, TBPF_NOPROGRESS)))
					m_LinkWnd = _LinkWnd;
				else
					_MESSAGE("ITaskbarList::SetProgressState() has failed.");
			}
		}
		virtual ~ProgressOnTaskBar(VOID) {
			if (g_pContextProgressOnTaskBar && m_LinkWnd)
				(*g_pContextProgressOnTaskBar)->SetProgressState(m_LinkWnd, TBPF_NOPROGRESS);
		}
	public:
		inline BOOL IsMarquee(VOID) const { return m_Marquee; }
		VOID SetMarquee(BOOL _value) {
			if (g_pContextProgressOnTaskBar && m_LinkWnd && (_value != m_Marquee)) {
				if (_value)
					m_Marquee = SUCCEEDED((*g_pContextProgressOnTaskBar)->SetProgressState(m_LinkWnd, TBPF_INDETERMINATE));
				else {
					if (m_Marquee = !SUCCEEDED((*g_pContextProgressOnTaskBar)->SetProgressState(m_LinkWnd, TBPF_NORMAL)); !m_Marquee)
						(*g_pContextProgressOnTaskBar)->SetProgressValue(m_LinkWnd, m_ullCompleted, m_ullTotal);
				}
			}
		}
		VOID SetPosition(ULONGLONG _complete) {
			if (!m_Marquee) {
				m_ullCompleted = std::max<ULONGLONG>(0, std::min<ULONGLONG>(_complete, m_ullTotal));
				(*g_pContextProgressOnTaskBar)->SetProgressValue(m_LinkWnd, m_ullCompleted, m_ullTotal);
			}
		}
	};

	static ProgressOnTaskBar* g_pProgressOnTaskBar = NULL;

	BOOL FIXAPI hk_CallLoadFile(TESDataFileHandler_CK* io_handler, INT32 _zero_only)
	{
		// get and save F4FileDataHandler from CK
		// FileHandler = io_handler; 0x6D67960 and doesn't change

		// Replacing Tips with a progress Bar
		if (bReplaceTips)
		{
			Assert(!sys::ProgressDialog);
			// show Progress
			sys::ProgressDialog = new sys::CUIProgressDialog(&MainWindow::GetWindowObj(), 3238);
			Assert(sys::ProgressDialog);
			sys::ProgressDialog->Create();
			// set Text
			sys::ProgressDialog->MessageText = "Loading Files...";
			// create progressbar on taskbar
			g_pContextProgressOnTaskBar = new ContextProgressOnTaskBar();
			Assert(g_pContextProgressOnTaskBar);
			g_pProgressOnTaskBar = new ProgressOnTaskBar(MainWindow::GetWindow(), sys::ProgressDialog->Max - sys::ProgressDialog->Min);
			Assert(g_pProgressOnTaskBar);
		}

		// load
		return io_handler->Load(_zero_only);
	}

	VOID FIXAPI hk_EndLoadFile(VOID)
	{
		// Replacing Tips with a progress Bar
		if (bReplaceTips)
		{
			// close Progress
			delete sys::ProgressDialog;
			sys::ProgressDialog = NULL;
			// close Progress on taskbar
			delete g_pProgressOnTaskBar;
			g_pProgressOnTaskBar = NULL;
			delete g_pContextProgressOnTaskBar;
			g_pContextProgressOnTaskBar = NULL;
		}

		RenderWindow::GetWindowObj().Foreground();
		CellViewWindow::GetWindowObj().Foreground();

		ObjectWindow::OBJWNDS Wnds = ObjectWindow::GetAllWindowObj();

		for each (auto Wnd in Wnds)
		{
			if (Wnd.second->ObjectWindow.Visible)
				Wnd.second->ObjectWindow.Foreground();
		}
	}

	VOID FIXAPI hk_StepItProgress(LPCSTR* str)
	{
		Assert(sys::ProgressDialog);
		Assert(g_pProgressOnTaskBar);

		// set position 0..95%
		std::string s(MainWindow::GetWindowObj().GetTextToStatusBarA(3));

		s.assign(s.begin() + s.find('%') - 2, s.end());
		auto complete = strtol(s.c_str(), NULL, 10);
		sys::ProgressDialog->Position = complete;
		g_pProgressOnTaskBar->SetPosition(complete);
	}

	BOOL FIXAPI hk_UpdateProgress(LPVOID __this, INT32 __1)
	{
		// Fallout CK is generally cut out.....

		static FLOAT lastPercent = 0.0f;

		// Only update every quarter percent, rather than every single form load
		FLOAT newPercent = ((FLOAT) * (UINT32*)OFFSET(0x6D6A7D8, 0) / (FLOAT) * (UINT32*)OFFSET(0x6D6A7D4, 0)) * 100.0f;

		if (abs(lastPercent - newPercent) > 0.25f)
			Core::Classes::UI::CUIMainWindow::ProcessMessages();

		return ((BOOL(__fastcall*)(LPVOID __this, INT32 __1))OFFSET(0x8027C0, 0))(__this, __1);
	}

	VOID FIXAPI hk_SetTextAndSendStatusBar(UINT32 index, LPCSTR message)
	{
		// send Status Bar
		((VOID(__stdcall*)(UINT32, LPCSTR))OFFSET(0x5FDFE0, 0))(index, message);
		// set Text 
		sys::ProgressDialog->MessageText = message;
		Core::Classes::UI::CUIMainWindow::ProcessMessages();
	}

	VOID FIXAPI hk_SendFromCellViewToRender(LPVOID Unknown1, TESForm_CK* View, INT32 Unknown3)
	{
		Assert(!sys::ProgressDialog);
		// show Progress
		sys::ProgressDialog = new sys::CUIProgressDialog(&MainWindow::GetWindowObj(), 3238);
		Assert(sys::ProgressDialog);
		sys::ProgressDialog->Create();
		// create progressbar on taskbar
		g_pContextProgressOnTaskBar = new ContextProgressOnTaskBar();
		Assert(g_pContextProgressOnTaskBar);
		g_pProgressOnTaskBar = new ProgressOnTaskBar(MainWindow::GetWindow(), sys::ProgressDialog->Max - sys::ProgressDialog->Min);
		Assert(g_pProgressOnTaskBar);

		sys::ProgressDialog->Marquee = TRUE;
		sys::ProgressDialog->MessageText = "Please wait while requested cell loads ...";
		g_pProgressOnTaskBar->SetMarquee(TRUE);

		// send
		((VOID(__fastcall*)(LPVOID, TESForm_CK*, INT32))OFFSET(0x45FE60, 0))(Unknown1, View, Unknown3);
	}

	VOID FIXAPI hk_EndSendFromCellViewToRender(VOID)
	{
		// Replacing Tips with a progress Bar
		if (bReplaceTips)
		{
			// close Progress
			delete sys::ProgressDialog;
			sys::ProgressDialog = NULL;
			// close Progress on taskbar
			delete g_pProgressOnTaskBar;
			g_pProgressOnTaskBar = NULL;
			delete g_pContextProgressOnTaskBar;
			g_pContextProgressOnTaskBar = NULL;
		}

		// enabled all markers
		Core::Classes::UI::CUIMenuItem MenuItem = MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(15);
		if (!MenuItem.Checked)
			MenuItem.Click();

		// enabled light markers
		MenuItem = MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(16);
		if (!MenuItem.Checked)
			MenuItem.Click();

		// enabled sound markers
		MenuItem = MainWindow::GetMainMenuObj().GetSubMenuItem(2).GetItemByPos(17);
		if (!MenuItem.Checked)
			MenuItem.Click();

		RenderWindow::GetWindowObj().Foreground();
	}

	///////////////////////////////////////

	VOID FIXAPI SetMarqueeInTaskbar(BOOL _value) {
		Assert(g_pProgressOnTaskBar);

		g_pProgressOnTaskBar->SetMarquee(_value);
	}
}
