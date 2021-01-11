#include "../../common.h"
#include "UIBaseWindow.h"
#include "UIProgressDialog.h"
#include "EditorUI.h"
#include "EditorUIProgressDialog.h"
#include "CellViewWindow.h"
#include "ObjectWindow.h"
#include "RenderWindow.h"
#include "MainWindow.h"

#ifndef HOTFIX_0001
#include "EditorUIDarkMode.h"
#endif // !HOTFIX_0001

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
			sys::ProgressDialog = new sys::CUIProgressDialog(&MainWindow::GetWindowObj(), 3238);
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

		RenderWindow::GetWindowObj().Foreground();
		CellViewWindow::GetWindowObj().Foreground();
		ObjectWindow::GetWindowObj().Foreground();
	}

	void __stdcall hk_StepItProgress(const char** str)
	{
		Assert(sys::ProgressDialog);

		// set position 0..95%

#ifndef HOTFIX_0001
		std::string s = "";

		if (EditorUIDarkMode::IsUIDarkMode())
			// fix crash DarkUI
			// when you enable the dark theme, the string is not available, because it is located in a different address spaceand is protected.
			s = GetMainWindowObj().GetTextToStatusBarA(3);
		else
			s = *str;
#else
		std::string s(MainWindow::GetWindowObj().GetTextToStatusBarA(3));
#endif

		s.assign(s.begin() + s.find('%') - 2, s.end());
		sys::ProgressDialog->Position = strtol(s.c_str(), nullptr, 10);
	}

	bool __stdcall hk_UpdateProgress(void* __this, int __1)
	{
		// Fallout CK is generally cut out.....

		static float lastPercent = 0.0f;

		// Only update every quarter percent, rather than every single form load
		float newPercent = ((float) * (uint32_t*)OFFSET(0x6D6A7D8, 0) / (float) * (uint32_t*)OFFSET(0x6D6A7D4, 0)) * 100.0f;

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
		sys::ProgressDialog = new sys::CUIProgressDialog(&MainWindow::GetWindowObj(), 3238);
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
}
