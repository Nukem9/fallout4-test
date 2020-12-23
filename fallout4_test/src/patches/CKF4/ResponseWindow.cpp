#include "ResponseWindow.h"
#include "LogWindow.h"
#include "CellViewWindow.h"
#include "EditorUI.h"

namespace ResponseWindow
{
	Core::Classes::UI::CUICustomWindow ResponseWindow;

	DLGPROC OldDlgProc;

	HWND GetWindow(void)
	{
		return ResponseWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& GetWindowObj(void)
	{
		return ResponseWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		bool& enableLipGeneration = *(bool*)OFFSET(0x455E2C8, 0);

		if (Message == WM_INITDIALOG)
		{
			ResponseWindow = DialogHwnd;

			if (enableLipGeneration)
			{
				if (GetFileAttributes("CreationKit32.exe") == INVALID_FILE_ATTRIBUTES ||
					GetFileAttributes("GFSDK_GodraysLib.Win32.dll") == INVALID_FILE_ATTRIBUTES ||
					GetFileAttributes("ssce5532.dll") == INVALID_FILE_ATTRIBUTES)
					enableLipGeneration = false;

				if (!enableLipGeneration)
					LogWindow::LogWarning(7, "'CreationKit32.exe', 'GFSDK_GodraysLib.Win32.dll', or 'ssce5532.dll' is missing from your game directory. LIP generation will be disabled.");
			}
		}
		else if (Message == WM_COMMAND)
		{
			const uint32_t param = LOWORD(wParam);

			switch (param)
			{
			case 1016:// "Generate Lip File"
			case 2379:// "From WAV"
			case 2380:// "From LTF"
				auto item = (__int64)EditorUI::ListViewGetSelectedItem(GetDlgItem(DialogHwnd, 2168));

				if (!enableLipGeneration || !item)
					return 1;

				if (param == 1016)
				{
					char audioFilePath[MAX_PATH];
					strcpy_s(audioFilePath, (const char*)(item + 0xC));

					auto data = *(__int64*)OFFSET(0x6D7B480, 0);
					auto topic = ((__int64(__fastcall*)(__int64, uint32_t))OFFSET(0x0B99420, 0))(*(__int64*)(data + 0x28), *(uint8_t*)(*(__int64*)(data + 0x18) + 0x1A));

					// The sound file must exist on disk, not in archives
					if (GetFileAttributes(audioFilePath) == INVALID_FILE_ATTRIBUTES)
					{
						LogWindow::LogWarning(7, "'%s' was not found on disk. Trying WAV extension fallback.", audioFilePath);

						// Try .wav as a fallback
						*strrchr(audioFilePath, '.') = '\0';
						strcat_s(audioFilePath, ".wav");

						if (GetFileAttributes(audioFilePath) == INVALID_FILE_ATTRIBUTES)
						{
							MessageBoxA(DialogHwnd, audioFilePath, "Unable to find audio file on disk", MB_ICONERROR);
							return 1;
						}
					}

					// Run the 32-bit CK
					auto inputText = ((const char* (__fastcall*)(__int64))OFFSET(0x0B56920, 0))(topic);
					((bool(__fastcall*)(HWND, const char*, const char*))OFFSET(0x0B66BF0, 0))(DialogHwnd, audioFilePath, inputText);

					char lipFileTarget[MAX_PATH];
					strcpy_s(lipFileTarget, audioFilePath);
					*strrchr(lipFileTarget, '.') = '\0';
					strcat_s(lipFileTarget, ".lip");

					if (GetFileAttributes(lipFileTarget) == INVALID_FILE_ATTRIBUTES)
						LogWindow::LogWarning(7, "LIP generation failed", lipFileTarget);
					else
						*(uint32_t*)(item + 0x114) = 1;
				}
				else
				{
					bool enableButton =
						IsDlgButtonChecked(DialogHwnd, 2379) && *(uint32_t*)(item + 0x110) ||
						IsDlgButtonChecked(DialogHwnd, 2380) && *(uint32_t*)(item + 0x118);

					EnableWindow(GetDlgItem(DialogHwnd, 1016), enableButton);
				}

				return 1;
			}
		}
		else if (Message == WM_NOTIFY)
		{
			auto notify = (LPNMHDR)lParam;

			if (notify->code == LVN_ITEMCHANGED && notify->idFrom == 2168)
				return DlgProc(DialogHwnd, WM_COMMAND, 2379, 0);
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}

	void WINAPI hk_7FF70C322BC0(HWND ListViewHandle, TESForm_CK* Form, bool UseImage, int32_t ItemIndex)
	{
		bool allowInsert = true;
		CellViewWindow::GetWindowObj().Perform(UI_CELL_WINDOW_ADD_ITEM, (WPARAM)Form, (LPARAM)& allowInsert);

		if (!allowInsert)
			return;

		return ((void(__fastcall*)(HWND, TESForm_CK*, bool, int32_t))OFFSET(0x562BC0, 0))(ListViewHandle, Form, UseImage, ItemIndex);
	}
}