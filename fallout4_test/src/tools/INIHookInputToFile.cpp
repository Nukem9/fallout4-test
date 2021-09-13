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

#include "INIHookInputToFile.h"
#include "..\xutil.h"

namespace Tools
{
	FILE* fHandle = nullptr;
	const char szFileName[MAX_PATH] = ".\\ini_hook.log";

	UINT WINAPI hk_GetPrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName)
	{
		UINT uResult = GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);

		fopen_s(&fHandle, szFileName, "a");
		if (fHandle)
		{
			fprintf(fHandle, "SectionName: %s; ParamName: %s; DefaultValue: %d; Value: %d\n", lpAppName, lpKeyName, nDefault, uResult);
			fclose(fHandle);
		}

		return uResult;
	}

	DWORD WINAPI hk_GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
	{
		DWORD dwResult = GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);

		fopen_s(&fHandle, szFileName, "a");
		if (fHandle)
		{
			fprintf(fHandle, "SectionName: %s; ParamName: %s; DefaultValue: %s; Value: %s\n", lpAppName, lpKeyName, lpDefault, lpReturnedString);
			fclose(fHandle);
		}

		return dwResult;
	}

	void __stdcall IniHookInputInit(void)
	{
		DeleteFileA(szFileName);

		// bool values
		XUtil::DetourCall(OFFSET(0x26051F9, 0), &hk_GetPrivateProfileIntA);
		// float values
		XUtil::DetourCall(OFFSET(0x260506A, 0), &hk_GetPrivateProfileStringA);
	}
}

