#define _CRT_SECURE_NO_WARNINGS

#include "..\..\Common.h"
#include "TranslateUnicode_CK.h"

#include <CommCtrl.h>

#define UI_RESID_AUTHOR_PLUGIN			1025
#define UI_RESID_DESCRIPTION_PLUGIN		1024

namespace Core
{
	namespace Patch
	{
		namespace CreationKit
		{
			CTranslateUnicodeMediator Translator;

			/*int CTranslateUnicodeMediator::getCodePage(void)
			{
				return m_CodePage;
			}

			void CTranslateUnicodeMediator::setCodePage(const int codepage)
			{
				m_CodePage = codepage;
			}*/

			CTranslateUnicodeMediator::CTranslateUnicodeMediator(/*const int codepage*/void)
			{
			//	m_CodePage = codepage;
				m_Mode = false;
				m_preaddres = nullptr;
			}

			CTranslateUnicodeMediator::~CTranslateUnicodeMediator(void)
			{
				heap.clear();
			}

			void CTranslateUnicodeMediator::SetMode(const bool mode)
			{
				m_Mode = mode;

				if (!m_Mode)
				{
					heap.clear();
					m_preaddres = nullptr;
				}
			}

			bool CTranslateUnicodeMediator::IsValid(const char* addr, const bool no_ansi)
			{
				if (no_ansi)
				{
					bool bRes = (addr != nullptr) && (addr != LPSTR_TEXTCALLBACKA);
					if (bRes)
					{
						// Don't check the entire line, it's very long and unnecessary.
						// Yes, I admit, there will be omissions, but extremely short lines like F0 to F12 and some small things

						size_t l = strlen(addr), limit = 5, lim = 0;
						if (l == 0) return false;

						if (limit > l) limit = l;

						for (size_t i = 0; i < l; i++)
						{
							if ((addr[i] >= 0x41) && (addr[i] <= 0x7E))
								lim++;

							if (limit == lim) return false;
						}

						return true;
					}
				}

				return ((addr != nullptr) && (addr != LPSTR_TEXTCALLBACKA) && (strlen(addr) > 0));
			}

			char* CTranslateUnicodeMediator::Utf8ToWinCP(const char* src)
			{
				// Ansi verification is necessary because there are a lot of strings, especially short and system strings. 
				// The debug file without this option was more than 70 mb, compared to 2604 kb.
				// Translation of fallout4.esm has become significantly faster.

				if ((src == m_preaddres) || !IsValid(src, false) || !XUtil::Conversion::IsUtf8Valid(src))
					return const_cast<char*>(src);

				std::string wincp_str = XUtil::Conversion::Utf8ToAnsi(src);

#ifdef __DBG_TRAN_UNICODE_READ
				out.open(L"dbg_tu_read.txt", std::ios::app);
				out << wincp_str << std::endl;
				out.close();
#endif // __DBG_TRAN_UNICODE_READ

				// utf-8 takes up more memory than ansi, so I can use active memory
				m_preaddres = const_cast<char*>(src);
				strcpy(m_preaddres, wincp_str.c_str());

				return m_preaddres;
			}

			char* CTranslateUnicodeMediator::WinCPToUtf8(const char* src)
			{
				// Not all strings are translated during loading and remain in Utf-8. 
				// They are loaded after opening the dialog. As an example "Description".

				if (!IsValid(src) || XUtil::Conversion::IsUtf8Valid(src))
					return const_cast<char*>(src);

				// in the Creation Kit code, the request to return a string occurs twice in a row.
				if ((m_preaddres == src) && (heap.size() > 0))
					return &heap.back()[0];

				// remove extra spaces and convert
				std::string utf8_str = XUtil::Conversion::AnsiToUtf8(XUtil::Str::trim(src));

#ifdef __DBG_TRAN_UNICODE_WRITE
				out.open("dbg_tu_write.txt", std::ios::app);
				out << utf8_str << std::endl;
				out.close();
#endif // __DBG_TRAN_UNICODE_WRITE

				// Unicode initially takes up more memory than ansi. 
				// Therefore, a heap is created that will store memory for the duration of saving.
				// Lists work quickly on adding items.
				heap.push_back(utf8_str);

				// pointer to the memory of the contents of the last line, it is of course unique
				m_preaddres = const_cast<char*>(src);
				return &heap.back()[0];
			}

			char* CTranslateUnicodeMediator::Translate(const char* src)
			{
				// Back to utf-8 (temporarily)
				if (m_Mode)
					return WinCPToUtf8(src);
				// All APIs are implemented as ANSI. In this regard, utf-8 must be converted to the specified ANSI.
				else
					return Utf8ToWinCP(src);
			}
		}
	}
}

namespace Experimental
{
	int __stdcall BeginPluginSave(void)
	{
		Core::Patch::CreationKit::Translator.SetMode(true);

		// need return 1 in rax
		return 1;
	}

	void* __stdcall EndPluginSave(void* __arg1)
	{
		void* ret = ((void* (__cdecl*)(void*))OFFSET(0x2A81F, 0))(__arg1);
		Core::Patch::CreationKit::Translator.SetMode(false);

		// return HCURSOR
		return ret;
	}

	char* __stdcall Translate(char* str)
	{
		return Core::Patch::CreationKit::Translator.Translate(str);
	}

	BOOL WINAPI hk_SetDlgItemTextA(HWND hDlg, int nIDDlgItem, LPCSTR lpString)
	{
		std::string dlg_name;
		dlg_name.resize(MAX_PATH);
		dlg_name.resize(GetWindowTextA(hDlg, &dlg_name[0], MAX_PATH));

		// need Data only
		if (dlg_name.compare("Data"))
			goto SetTextDef;

		switch (nIDDlgItem)
		{
		case UI_RESID_AUTHOR_PLUGIN:
		case UI_RESID_DESCRIPTION_PLUGIN:
		{
			if (!lpString || !XUtil::Conversion::IsUtf8Valid(lpString))
				goto SetTextDef;

			std::string wincp_str = XUtil::Conversion::Utf8ToAnsi(lpString);
			return SetDlgItemTextA(hDlg, nIDDlgItem, wincp_str.c_str());
		}
		default:
		SetTextDef:
			return SetDlgItemTextA(hDlg, nIDDlgItem, lpString);
		}
	}

	LRESULT WINAPI hk_SendDlgItemMessageA(HWND hDlg, int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Msg != WM_GETTEXT && Msg != WM_GETTEXTLENGTH)
			MsgTextDef:
		return SendDlgItemMessageA(hDlg, nIDDlgItem, Msg, wParam, lParam);

		std::string dlg_name;
		dlg_name.resize(MAX_PATH);
		dlg_name.resize(GetWindowTextA(hDlg, &dlg_name[0], MAX_PATH));

		// need Data only
		if (dlg_name.compare("Data"))
			goto MsgTextDef;

		HWND hCtrlWnd;

		switch (nIDDlgItem)
		{
		case UI_RESID_AUTHOR_PLUGIN:
		case UI_RESID_DESCRIPTION_PLUGIN:
		{
			hCtrlWnd = GetDlgItem(hDlg, nIDDlgItem);
			int maxlen = GetWindowTextLengthA(hCtrlWnd) << 2;

			if (maxlen <= 0)
				goto MsgTextDef;

			std::string ansi_str;
			ansi_str.resize(maxlen);
			ansi_str.resize(GetWindowTextA(hCtrlWnd, &ansi_str[0], maxlen));

			if (XUtil::Conversion::IsUtf8Valid(ansi_str))
				goto MsgTextDef;

			std::string utf8_str = XUtil::Conversion::AnsiToUtf8(ansi_str);

			if (Msg == WM_GETTEXT)
				strncpy((LPSTR)(lParam), utf8_str.c_str(), wParam);

			return utf8_str.length() + 1;
		}
		default:
			goto MsgTextDef;
		}
	}
}

