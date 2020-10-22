#pragma once

//#define __DBG_TRAN_UNICODE_READ
//#define __DBG_TRAN_UNICODE_WRITE

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <list>
#include <string>

namespace Core
{
	namespace Patch
	{
		namespace CreationKit
		{
			class CTranslateUnicodeMediator
			{
			private:
			//	int m_CodePage;
				bool m_Mode;
				char* m_preaddres;
				std::list<std::string> heap;
#ifdef __DBG_TRAN_UNICODE_WRITE
				std::ofstream out;
#elif __DBG_TRAN_UNICODE_READ
				std::ofstream out;
#endif // __DBG_TRAN_UNICODE
			private:
				bool IsValid(const char* addr, const bool no_ansi = false);
				char* Utf8ToWinCP(const char* src);
				char* WinCPToUtf8(const char* src);
			/*public:
				int GetCodePage(void);
				void SetCodePage(const int codepage);*/
			public:
				void SetMode(const bool mode);
				char* Translate(const char* src);
			/*public:
				__declspec(property(get = GetCodePage, put = SetCodePage)) int CodePage;*/
			public:
				CTranslateUnicodeMediator(/*const int codepage = -1*/void);
				~CTranslateUnicodeMediator(void);
			};

			extern CTranslateUnicodeMediator Translator;
		}
	}
}

namespace Experimental
{
	int __stdcall BeginPluginSave(void);
	void* __stdcall EndPluginSave(void* __arg1);
	char* __stdcall Translate(char* str);

	BOOL WINAPI hk_SetDlgItemTextA(HWND hDlg, INT nIDDlgItem, LPCSTR lpString);
	LRESULT WINAPI hk_SendDlgItemMessageA(HWND hDlg, INT nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam);
}