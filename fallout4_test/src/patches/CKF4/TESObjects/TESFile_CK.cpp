//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
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

#include "../../../common.h"
#include "../UIBaseWindow.h"
#include "../LogWindow.h"
#include "TESFile_CK.h"

INT32 TESFile_CK::hk_LoadTESInfo(VOID)
{
	int error = LoadTESInfo(this);

	if (error != 0)
		return error;

	// If it's an ESM being loaded as the active file, force it to act like a normal ESP
	if (AllowSaveESM)
	{
		if (IsMaster() && IsActive())
		{
			LogWindow::Log("Loading master file '%s' as a plugin\n", m_FileName);

			// Strip ESM flag, clear loaded ONAM data
			m_Flags &= ~FILE_RECORD_ESM;
			((void(__fastcall*)(TESFile_CK*))OFFSET(0x805EF0, 0))(this);
		}
	}

	// If loading ESP files as masters, flag everything except for the currently active plugin
	if (AllowMasterESP)
	{
		if (!IsMaster() && !IsActive() && IsSelected())
		{
			LogWindow::Log("Loading plugin file '%s' as a master\n", m_FileName);
			m_Flags |= FILE_RECORD_ESM;
		}
	}

	return 0;
}

INT64 TESFile_CK::hk_WriteTESInfo(VOID)
{
	bool resetEsmFlag = false;

	if (AllowSaveESM)
	{
		if (IsActive())
		{
			LPCSTR extension = strrchr(m_FileName, '.');

			if (extension && !_stricmp(extension, ".esm"))
			{
				LogWindow::Log("Regenerating ONAM data for master file '%s'...\n", m_FileName);

				((void(__fastcall*)(TESFile_CK*))OFFSET(0x805F90, 0))(this);
				resetEsmFlag = true;
			}
		}
	}

	auto form = WriteTESInfo(this);

	if (resetEsmFlag)
		m_Flags &= ~FILE_RECORD_ESM;

	return form;
}

BOOL TESFile_CK::IsActiveFileBlacklist(VOID)
{
	if (IsMaster())
	{
		std::string str = FileName;
		XUtil::Str::LowerCase(str);

		if (!str.compare("fallout4.esm") ||
			!str.compare("dlcrobot.esm") ||
			!str.compare("dlcnukaworld.esm") ||
			!str.compare("dlccoast.esm") ||
			!str.compare("dlcworkshop01.esm") ||
			!str.compare("dlcworkshop02.esm") ||
			!str.compare("dlcworkshop03.esm"))
		{
			Core::Classes::UI::CUIMainWindow::MessageWarningDlg("Base game master files cannot be set as the active file.");
			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}