#include "..\..\Common.h"
#include "UIBaseWindow.h"
#include "Editor.h"

#include <commctrl.h>

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			// CFont

			CFont::CFont(const HDC hDC) : m_Cnt(nullptr), m_Handle(NULL)
			{
				Recreate(hDC);
			}

			CFont::CFont(CUIBaseWindow* Cnt) : m_Cnt(Cnt), m_Handle(NULL)
			{
				Assert(m_Cnt);
				HDC hDC = GetDC(m_Cnt->Handle);
				Recreate(hDC);
				ReleaseDC(m_Cnt->Handle, hDC);
			}

			CFont::CFont(const std::string &name, const LONG size, const CFontStyles &styles, const CFontQuality quality,
				const CFontPitch pitch) :
				m_Name(name), m_Cnt(nullptr), m_Quality(quality), m_FontStyles(styles), m_Pitch(pitch), m_Handle(NULL)
			{
				Size = size;
			}

			void CFont::Recreate(const HDC hDC)
			{
				TEXTMETRICA metric;
				GetTextMetricsA(hDC, &metric);
				m_Name.resize(MAX_PATH);
				m_Name.resize(GetTextFaceA(hDC, MAX_PATH, &m_Name[0]) + 1);

				m_Quality = fqClearTypeNatural;

				if (metric.tmItalic)
					m_FontStyles.insert(fsItalic);
				if (metric.tmUnderlined)
					m_FontStyles.insert(fsUnderline);
				if (metric.tmWeight >= FW_BOLD)
					m_FontStyles.insert(fsBold);

				m_Pitch = fpDefault;
				m_Height = metric.tmHeight;

				Recreate();
			}

			void CFont::SetStyles(const CFontStyles &styles)
			{
				m_FontStyles = styles;
				Change();
			}

			void CFont::SetName(const std::string &name)
			{
				m_Name = name;
				Change();
			}

			LONG CFont::GetSize(void) const
			{
				HDC hDC = GetDC(0);
				LONG res = -MulDiv(m_Height, 72, GetDeviceCaps(hDC, LOGPIXELSY));
				ReleaseDC(0, hDC);
				return res;
			}

			void CFont::SetSize(const LONG value)
			{
				HDC hDC = GetDC(0);
				m_Height = -MulDiv(value, GetDeviceCaps(hDC, LOGPIXELSY), 72);
				ReleaseDC(0, hDC);
				Change();
			}

			void CFont::SetHeight(const LONG value)
			{
				m_Height = value;
				Change();
			}

			void CFont::Apply(HWND window)
			{
				SendMessageA(window, WM_SETFONT, (WPARAM)m_Handle, 0);
			}

			void CFont::Apply(CUIBaseWindow* window)
			{
				Apply(window->Handle);
			}

			void CFont::SetQuality(const CFontQuality quality)
			{
				m_Quality = quality;
				Change();
			}

			void CFont::SetPitch(const CFontPitch pitch)
			{
				m_Pitch = pitch;
				Change();
			}

			void CFont::Recreate(void)
			{
				DWORD _Quality = DEFAULT_QUALITY;
				DWORD _Pitch = DEFAULT_PITCH;

				switch (m_Quality)
				{
				case fqClearType:
					_Quality = CLEARTYPE_QUALITY;
					break;
				case fqClearTypeNatural:
					_Quality = CLEARTYPE_NATURAL_QUALITY;
					break;
				case fqProof:
					_Quality = PROOF_QUALITY;
					break;
				case fqAntialiased:
					_Quality = NONANTIALIASED_QUALITY;
					break;
				case fqNoAntialiased:
					_Quality = ANTIALIASED_QUALITY;
					break;
				}

				switch (m_Pitch)
				{
				case fpFixed:
					_Pitch = FIXED_PITCH;
					break;
				case fpVariable:
					_Pitch = VARIABLE_PITCH;
					break;
				case fpMono:
					_Pitch = MONO_FONT;
					break;
				}

				if (m_Handle)
					DeleteObject(m_Handle);

				m_Handle = CreateFontA(Height, 0, 0, 0,
					m_FontStyles.count(fsBold) ? FW_BOLD : FW_NORMAL,
					m_FontStyles.count(fsItalic),
					m_FontStyles.count(fsUnderline),
					m_FontStyles.count(fsStrikeOut),
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					_Quality,
					_Pitch,
					m_Name.c_str());
			}

			void CFont::Change(void)
			{
				Recreate();

				if (m_Cnt)
					m_Cnt->__ChangeFont(this);
			}

			// CUIBaseWindow

			void CUIBaseWindow::__ChangeFont(const CFont* font)
			{
				SendMessageA(m_hWnd, WM_SETFONT, (WPARAM)(font->Handle), TRUE);
			}

			void CUIBaseWindow::LockUpdate(void)
			{
				if (m_LockUpdate)
					return;

				m_LockUpdate = TRUE;
				SendMessageA(m_hWnd, WM_SETREDRAW, FALSE, 0);
			}

			void CUIBaseWindow::UnlockUpdate(void)
			{
				if (!m_LockUpdate)
					return;

				m_LockUpdate = FALSE;
				SendMessageA(m_hWnd, WM_SETREDRAW, TRUE, 0);
			}

			void CUIBaseWindow::Move(const LONG x, const LONG y)
			{
				SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
			}

			void CUIBaseWindow::Invalidate(void)
			{
				InvalidateRect(m_hWnd, NULL, FALSE);
			}

			void CUIBaseWindow::Repaint(void)
			{
				RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
			}

			BOOL CUIBaseWindow::GetVisible(void)
			{
				return IsWindowVisible(m_hWnd);
			}

			BOOL CUIBaseWindow::GetConstVisible(void) const
			{
				return IsWindowVisible(m_hWnd);
			}

			void CUIBaseWindow::SetSize(const LONG cx, const LONG cy)
			{
				CRECT r = BoundsRect;
				SetWindowPos(m_hWnd, NULL, 0, 0, cx + r.Left, cy + r.Top, SWP_NOOWNERZORDER | SWP_NOMOVE);
			}

			void CUIBaseWindow::SetFocus(void)
			{
				::SetFocus(m_hWnd);
			}

			void CUIBaseWindow::SetVisible(const BOOL value)
			{
				Assert(value != Visible);

				WindowState = (value) ? wsNormal : wsHide;
			}

			std::string CUIBaseWindow::GetCaption(void) const
			{
				std::string s;
				s.resize(MAX_PATH);
				GetWindowTextA(m_hWnd, &s[0], MAX_PATH);
				return s;
			}

			void CUIBaseWindow::SetCaption(const std::string &value)
			{
				SetWindowTextA(m_hWnd, value.c_str());
			}

			std::wstring CUIBaseWindow::GetWideCaption(void) const
			{
				std::wstring s;
				s.resize(MAX_PATH);
				GetWindowTextW(m_hWnd, &s[0], MAX_PATH);
				return s;
			}

			void CUIBaseWindow::SetWideCaption(const std::wstring &value)
			{
				SetWindowTextW(m_hWnd, value.c_str());
			}

			ULONG_PTR CUIBaseWindow::GetStyle(void) const
			{
				return GetWindowLongA(m_hWnd, GWL_STYLE);
			}

			void CUIBaseWindow::SetStyle(const ULONG_PTR value)
			{
				SetWindowLongA(m_hWnd, GWL_STYLE, value);
			}

			ULONG_PTR CUIBaseWindow::GetStyleEx(void) const
			{
				return GetWindowLongA(m_hWnd, GWL_EXSTYLE);
			}

			void CUIBaseWindow::SetStyleEx(const ULONG_PTR value)
			{
				SetWindowLongA(m_hWnd, GWL_EXSTYLE, value);
			}

			BOOL CUIBaseWindow::GetEnabled(void) const
			{
				return IsWindowEnabled(m_hWnd);
			}

			void CUIBaseWindow::SetEnabled(const BOOL value)
			{
				Assert(value != Enabled);

				EnableWindow(m_hWnd, value);
			}

			std::string CUIBaseWindow::GetName(void) const
			{
				std::string s;
				s.resize(MAX_PATH);
				GetClassNameA(m_hWnd, &s[0], MAX_PATH);
				return s;
			}

			std::wstring CUIBaseWindow::GetWideName(void) const
			{
				std::wstring s;
				s.resize(MAX_PATH);
				GetClassNameW(m_hWnd, &s[0], MAX_PATH);
				return s;
			}

			WindowState_t CUIBaseWindow::GetWindowState(void) const
			{ 
				LONG style = GetWindowLongA(m_hWnd, GWL_STYLE);

				if (!IsWindowVisible(m_hWnd))
				{
					return wsHide;
				}
				else
				{
					if ((style & WS_MAXIMIZE) == WS_MAXIMIZE)
					{
						return wsMaximized;
					}
					else if ((style & WS_MINIMIZE) == WS_MINIMIZE)
					{
						return wsMinimized;
					}
					else
					{
						return wsNormal;
					}
				}
			}

			void CUIBaseWindow::SetWindowState(const WindowState_t state)
			{
				Assert(WindowState != state);

				int flag = SW_NORMAL;

				switch (state)
				{
				case wsMaximized:
				{
					flag = SW_MAXIMIZE;
					break;
				}
				case wsMinimized:
				{
					flag = SW_MINIMIZE;
					break;
				}
				case wsHide:
				{
					flag = SW_HIDE;
					break;
				}
				}

				ShowWindow(m_hWnd, flag);
			}

			CRECT CUIBaseWindow::WindowRect(void) const
			{
				RECT r;
				GetWindowRect(m_hWnd, &r);
				return r;
			}

			CRECT CUIBaseWindow::ClientRect(void) const
			{
				RECT r;
				GetClientRect(m_hWnd, &r);
				return r;
			}

			LONG CUIBaseWindow::ClientWidth(void) const
			{
				return ClientRect().Right;
			}

			LONG CUIBaseWindow::ClientHeight(void) const
			{
				return ClientRect().Bottom;
			}

			BOOL CUIBaseWindow::Is(void) const
			{
				return m_hWnd && IsWindow(m_hWnd);
			}

			HWND CUIBaseWindow::Parent(void) const
			{
				return GetParent(m_hWnd);
			}

			CUIBaseWindow CUIBaseWindow::ParentWindow(void) const
			{
				return CUIBaseWindow(Parent());
			}

			POINT CUIBaseWindow::ScreenToControl(const POINT &p) const
			{
				if (!Is())
					return POINT{ 0 };

				POINT pnt = p;

				return (ScreenToClient(m_hWnd, &pnt)) ? pnt : POINT{ 0 };
			}

			POINT CUIBaseWindow::ControlToScreen(const POINT &p) const
			{
				if (!Is())
					return POINT{ 0 };

				POINT pnt = p;

				return (ClientToScreen(m_hWnd, &pnt)) ? pnt : POINT{ 0 };
			}

			CRECT CUIBaseWindow::GetBoundsRect(void) const
			{
				CUIBaseWindow parent = ParentWindow();

				if (!parent.Is())
					return WindowRect();

				CRECT wrect = WindowRect();
				POINT Off = parent.ScreenToControl({ wrect.Left, wrect.Top });

				wrect.Left = Off.x;
				wrect.Top = Off.y;

				return wrect;
			}

			void CUIBaseWindow::SetBoundsRect(const CRECT &bounds)
			{
				MoveWindow(m_hWnd, bounds.Left, bounds.Top, bounds.Width, bounds.Height, TRUE);
			}

			LONG CUIBaseWindow::GetLeft(void) const
			{
				return BoundsRect.Left;
			}

			void CUIBaseWindow::SetLeft(const LONG value)
			{
				CRECT bounds = BoundsRect;
				LONG w = bounds.Width;
				bounds.Left = value;
				bounds.Width = w;
				BoundsRect = bounds;
			}

			LONG CUIBaseWindow::GetTop(void) const
			{
				return BoundsRect.Top;
			}

			void CUIBaseWindow::SetTop(const LONG value)
			{
				CRECT bounds = BoundsRect;
				LONG h = bounds.Height;
				bounds.Top = value;
				bounds.Height = h;
				BoundsRect = bounds;
			}

			LONG CUIBaseWindow::GetWidth(void) const
			{
				return BoundsRect.Width;
			}

			void CUIBaseWindow::SetWidth(const LONG value)
			{
				CRECT bounds = BoundsRect;
				bounds.Width = value;
				BoundsRect = bounds;
			}

			LONG CUIBaseWindow::GetHeight(void) const
			{
				return BoundsRect.Height;
			}

			void CUIBaseWindow::SetHeight(const LONG value)
			{
				CRECT bounds = BoundsRect;
				bounds.Height = value;
				BoundsRect = bounds;
			}

			LRESULT CUIBaseWindow::Perform(UINT uMsg, WPARAM wParam, LPARAM lParam)
			{ 
				return Perform(m_hWnd, uMsg, wParam, lParam); 
			}

			LRESULT CUIBaseWindow::Perform(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{ 
				return hk_SendMessageA(hWnd, uMsg, wParam, lParam);
			}

			LRESULT CUIBaseWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				return DefWindowProcA(hWnd, uMsg, wParam, lParam);
			}

			// CUICustomWindow

			CUIBaseControl CUICustomWindow::GetControl(const uint32_t index)
			{
				HWND hWnd = GetDlgItem(Handle, index);
				AssertMsgVa(hWnd, "control %d no found", index);

				return CUIBaseControl(hWnd);
			}

			void CUICustomWindow::Foreground(void)
			{
				SetForegroundWindow(m_hWnd);
			}

			// CUIDialog

			CUICustomDialog::CUICustomDialog(CUICustomWindow* parent, const UINT res_id, DLGPROC dlg_proc) : CUICustomWindow()
			{
				m_ResId = res_id;
				m_DlgProc = dlg_proc;
				m_Parent = parent;
			}

			void CUICustomDialog::Create(void)
			{
				// fix crashes dark theme
			//	CreateDialogParamA(GetModuleHandle(NULL), MAKEINTRESOURCEA(m_ResId), m_Parent->Handle, m_DlgProc, (LONG_PTR)this);
				hk_CreateDialogParamA(GetModuleHandle(NULL), MAKEINTRESOURCEA(m_ResId), m_Parent->Handle, m_DlgProc, (LONG_PTR)this);
				Assert(m_hWnd);
				ShowWindow(m_hWnd, SW_SHOW);
				UpdateWindow(m_hWnd);
			}

			void CUICustomDialog::FreeRelease(void)
			{
				if (!m_hWnd) return;
				DestroyWindow(m_hWnd);
				m_hWnd = NULL;
			};

			CUICustomDialog::~CUICustomDialog(void)
			{
				FreeRelease();
			}

			// CUIMainWindow

			void CUIMainWindow::FindToolWindow(void)
			{
				EnumChildWindows(Handle, [](HWND hwnd, LPARAM lParam) -> BOOL {
					CUIMainWindow* main = (CUIMainWindow*)lParam;
					CUIToolWindow Tool(hwnd);

					// For some reason, only the standard comparison function finds it...

					if (!strcmp(Tool.Name.c_str(), TOOLBARCLASSNAMEA))
						main->Toolbar = Tool;
					else if (!strcmp(Tool.Name.c_str(), STATUSCLASSNAMEA))
						main->Statusbar = Tool;
					return TRUE;
				}, (LPARAM)this);
			}

			void CUIMainWindow::ProcessMessages(void)
			{
				MSG msg;
				while (true)
				{
					if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
					{
						DispatchMessageA(&msg);
						TranslateMessage(&msg);
					}
					else
						break;
				}
			}

			INT32 CUIMainWindow::MessageDlg(const std::string message, const std::string caption, const UINT32 flags)
			{
				return MessageBoxA(GetForegroundWindow(), message.c_str(), caption.c_str(), flags);
			}

			INT32 CUIMainWindow::MessageDlg(const std::wstring message, const std::wstring caption, const UINT32 flags)
			{
				return MessageBoxW(GetForegroundWindow(), message.c_str(), caption.c_str(), flags);
			}

			INT32 CUIMainWindow::MessageWarningDlg(const std::string message)
			{
				return MessageDlg(message, "Warning", MB_ICONWARNING);
			}

			INT32 CUIMainWindow::MessageWarningDlg(const std::wstring message)
			{
				return MessageDlg(message, L"Warning", MB_ICONWARNING);
			}

			void CUIMainWindow::SetTextToStatusBar(const uint32_t index, const std::string text)
			{
				Statusbar.Perform(SB_SETTEXTA, index, (LPARAM)text.c_str());
			}

			void CUIMainWindow::SetTextToStatusBar(const uint32_t index, const std::wstring text)
			{
				Statusbar.Perform(SB_SETTEXTW, index, (LPARAM)text.c_str());
			}

			std::string CUIMainWindow::GetTextToStatusBarA(const uint32_t index)
			{
				LPSTR lpBuffer = NULL;
				INT nLen = Statusbar.Perform(SB_GETTEXTLENGTHA, index, NULL);
				if (nLen > 0)
				{
					lpBuffer = (LPSTR)malloc(nLen + 1);
					Statusbar.Perform(SB_GETTEXTA, index, (LPARAM)lpBuffer);
					std::string s(lpBuffer);
					free(lpBuffer);

					return s;
				}
				else return "";
			}

			std::wstring CUIMainWindow::GetTextToStatusBarW(const uint32_t index)
			{
				LPWSTR lpBuffer = NULL;
				INT nLen = Statusbar.Perform(SB_GETTEXTLENGTHW, index, NULL);
				if (nLen > 0)
				{
					lpBuffer = (LPWSTR)malloc((nLen + 1) << 1);
					Statusbar.Perform(SB_GETTEXTW, index, (LPARAM)lpBuffer);
					std::wstring s(lpBuffer);
					free(lpBuffer);

					return s;
				}
				else return L"";
			}
		}
	}
}


