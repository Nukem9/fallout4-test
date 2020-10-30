#pragma once

#include"..\ComClasses.h"
#include "UIMenus.h"

namespace Core
{
	enum WindowState_t {
		wsNormal,
		wsMaximized,
		wsMinimized,
		wsHide
	};

	namespace Classes
	{
		namespace UI
		{
			class CUIBaseWindow;
			class CUIBaseControl;
			class CUICustomWindow;
			class CUIMainWindow;
			class CUIToolWindow;

			class CRECT
			{
			private:
				LONG m_left;
				LONG m_top;
				LONG m_right;
				LONG m_bottom;
			public:
				CRECT() : m_left(0), m_top(0), m_right(0), m_bottom(0) {}
				CRECT(const LONG l, const LONG t, const LONG r, const LONG b) : m_left(l), m_top(t), m_right(r), m_bottom(b) {}
				CRECT(const RECT &r) : m_left(r.left), m_top(r.top), m_right(r.right), m_bottom(r.bottom) {}
				CRECT(const CRECT &r) : m_left(r.m_left), m_top(r.m_top), m_right(r.m_right), m_bottom(r.m_bottom) {}
			public:
				inline LONG GetWidth(void) const { return m_right - m_left; }
				inline LONG GetHeight(void) const { return m_bottom - m_top; }
				inline LONG GetLeft(void) const { return m_left; }
				inline LONG GetTop(void) const { return m_top; }
				inline LONG GetRight(void) const { return m_right; }
				inline LONG GetBottom(void) const { return m_bottom; }
				inline void SetWidth(const LONG value) { m_right = value + m_left; }
				inline void SetHeight(const LONG value) { m_bottom = value + m_top; }
				inline void SetLeft(const LONG value) { m_right = GetWidth() + value; m_left = value; }
				inline void SetTop(const LONG value) { m_bottom = GetHeight() + value; m_top = value; }
				inline void SetRight(const LONG value) { m_right = value; }
				inline void SetBottom(const LONG value) { m_bottom = value; }
			public:
				inline CRECT& Inflate(const LONG x, const LONG y) { m_left -= x; m_top -= y; m_right += x; m_bottom += y; return *this; }
				inline BOOL IsEmpty(void) const { return GetWidth() == 0 || GetHeight() == 0; }
				inline CRECT Dublicate(void) const { return *this; }
			public:
				PROPERTY(GetWidth, SetWidth) LONG Width;
				PROPERTY(GetHeight, SetHeight) LONG Height;
				PROPERTY(GetLeft, SetLeft) LONG Left;
				PROPERTY(GetTop, SetTop) LONG Top;
				PROPERTY(GetRight, SetRight) LONG Right;
				PROPERTY(GetBottom, SetBottom) LONG Bottom;
			};

			enum CFontStyle {
				fsBold,
				fsItalic,
				fsUnderline,
				fsStrikeOut
			};
			typedef std::set<CFontStyle> CFontStyles;
			enum CFontQuality {
				fqDefault,
				fqClearType,
				fqClearTypeNatural,
				fqProof,
				fqAntialiased,
				fqNoAntialiased,
			};
			enum CFontPitch {
				fpDefault,
				fpFixed,
				fpVariable,
				fpMono
			};

			class CFont
			{
			private:
				HFONT m_Handle;
				std::string m_Name;
				CUIBaseWindow* m_Cnt;
				CFontStyles m_FontStyles;
				CFontQuality m_Quality;
				CFontPitch m_Pitch;
				LONG m_Height;
			private:
				void Change(void);
			public:
				inline HFONT GetHandle(void) const { return m_Handle; }
				std::string GetName(void) const { return m_Name; }
				void SetName(const std::string &name);
				LONG GetSize(void) const;
				void SetSize(const LONG value);
				inline LONG GetHeight(void) const { return m_Height; }
				void SetHeight(const LONG value);
				inline CFontStyles GetStyles(void) const { return m_FontStyles; }
				void SetStyles(const CFontStyles &styles);
				inline CFontQuality GetQuality(void) const { return m_Quality; }
				void SetQuality(const CFontQuality quality);
				inline CFontPitch GetPitch(void) const { return m_Pitch; }
				void SetPitch(const CFontPitch pitch);
			private:
				void Recreate(void);
				void Recreate(const HDC hDC);
			public:
				void Apply(HWND window);
				void Apply(CUIBaseWindow* window);
				inline void Release(void) { DeleteObject(m_Handle); }
			public:
				READ_PROPERTY(GetHandle) HFONT Handle;
				PROPERTY(GetName, SetName) const std::string Name;
				PROPERTY(GetSize, SetSize) const LONG Size;
				PROPERTY(GetHeight, SetHeight) const LONG Height;
				PROPERTY(GetStyles, SetStyles) const CFontStyles Styles;
				PROPERTY(GetQuality, SetQuality) const CFontQuality Quality;
				PROPERTY(GetPitch, SetPitch) const CFontPitch Pitch;
			public:
				CFont(const std::string &name, const LONG size, const CFontStyles &styles = {}, const CFontQuality quality = fqClearTypeNatural,
					const CFontPitch pitch = fpVariable);
				CFont(const HDC hDC);
				CFont(const CFont &parent) : m_Cnt(NULL), m_FontStyles(parent.m_FontStyles), m_Quality(parent.m_Quality), m_Pitch(parent.m_Pitch),
					m_Name(parent.m_Name), m_Height(parent.m_Height) { Recreate(); }
				CFont(CUIBaseWindow* Cnt);
			};

			class CUIBaseWindow
			{
			private:
				BOOL m_LockUpdate;
				HDC m_hDC;
				WindowState_t m_WindowState;
			public:
				CFont Font;
			protected:
				HWND m_hWnd;
			public:
				virtual void __ChangeFont(const CFont* font);
			public:
				inline HWND GetHandle(void) const { return m_hWnd; }
				inline WindowState_t GetWindowState(void) const { return m_WindowState; }
				void SetWindowState(const WindowState_t state);
				BOOL GetVisible(void);
				BOOL GetConstVisible(void) const;
				void SetVisible(const BOOL value);
				std::string GetCaption(void) const;
				void SetCaption(const std::string &value);
				std::wstring GetWideCaption(void) const;
				void SetWideCaption(const std::wstring &value);
				ULONG_PTR GetStyle(void) const;
				void SetStyle(const ULONG_PTR value);
				ULONG_PTR GetStyleEx(void) const;
				void SetStyleEx(const ULONG_PTR value);
				BOOL GetEnabled(void) const;
				void SetEnabled(const BOOL value);
				std::string GetName(void) const;
				std::wstring GetWideName(void) const;
				CRECT GetBoundsRect(void) const;
				virtual void SetBoundsRect(const CRECT &bounds);
				LONG GetLeft(void) const;
				virtual void SetLeft(const LONG value);
				LONG GetTop(void) const;
				virtual void SetTop(const LONG value);
				LONG GetWidth(void) const;
				virtual void SetWidth(const LONG value);
				LONG GetHeight(void) const;
				virtual void SetHeight(const LONG value);
			public:
				BOOL Is(void) const;
				CRECT WindowRect(void) const;
				CRECT ClientRect(void) const;
				LONG ClientWidth(void) const;
				LONG ClientHeight(void) const;
				void Move(const LONG x, const LONG y);
				void SetSize(const LONG cx, const LONG cy);
				HWND Parent(void) const;
				CUIBaseWindow ParentWindow(void) const;
				POINT ScreenToControl(const POINT &p) const;
				POINT ControlToScreen(const POINT &p) const;
				inline BOOL IsLockUpdate(void) const { return m_LockUpdate; }
				void LockUpdate(void);
				void UnlockUpdate(void);
				void Invalidate(void);
				void Repaint(void);
				void SetFocus(void);
				inline HDC DeviceContext(void) const { return m_hDC; }
			public:
				virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			public:
				LRESULT Perform(UINT uMsg, WPARAM wParam, LPARAM lParam);
				LRESULT Perform(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			public:
				CUIBaseWindow(void) : m_LockUpdate(FALSE), m_hWnd(NULL), m_WindowState(wsNormal), m_hDC(GetDC(0)), Font(this)
					{ }
				CUIBaseWindow(const HWND hWnd) : m_LockUpdate(FALSE), m_hWnd(hWnd), m_WindowState(wsNormal), m_hDC(GetDC(hWnd)), Font(m_hDC)
					{ }
				CUIBaseWindow(const CUIBaseWindow &base) : m_LockUpdate(base.m_LockUpdate), m_hWnd(base.m_hWnd), m_WindowState(base.m_WindowState), m_hDC(GetDC(m_hWnd)), Font(this)
					{ }
				virtual ~CUIBaseWindow(void) { ReleaseDC(m_hWnd, m_hDC); }
			public:
				READ_PROPERTY(GetHandle) HWND Handle;
				PROPERTY(GetWindowState, SetWindowState) WindowState_t WindowState;
				PROPERTY(GetConstWindowState, SetWindowState) const WindowState_t ConstWindowState;
				PROPERTY(GetVisible, SetVisible) BOOL Visible;
				PROPERTY(GetConstVisible, SetVisible) const BOOL ConstVisible;
				PROPERTY(GetCaption, SetCaption) std::string Caption;
				PROPERTY(GetWideCaption, SetWideCaption) const std::wstring WideCaption;
				PROPERTY(GetStyle, SetStyle) ULONG_PTR Style;
				PROPERTY(GetStyleEx, SetStyleEx) ULONG_PTR StyleEx;
				PROPERTY(GetEnabled, SetEnabled) BOOL Enabled;
				READ_PROPERTY(GetName) const std::string Name;
				READ_PROPERTY(GetWideName) const std::wstring WideName;
				PROPERTY(GetBoundsRect, SetBoundsRect) CRECT BoundsRect;
				PROPERTY(GetLeft, SetLeft) LONG Left;
				PROPERTY(GetTop, SetTop) LONG Top;
				PROPERTY(GetWidth, SetWidth) LONG Width;
				PROPERTY(GetHeight, SetHeight) LONG Height;
			};

			class CUICustomWindow : public CUIBaseWindow
			{
			public:
				CUIBaseControl GetControl(const uint32_t index);
				void Foreground(void);
			public:
				CUICustomWindow(void) : CUIBaseWindow() {}
				CUICustomWindow(const HWND hWnd) : CUIBaseWindow(hWnd) {}
				CUICustomWindow(const CUICustomWindow &base) : CUIBaseWindow(base) {}
			};

			class CUIToolWindow : public CUIBaseWindow
			{
			public:
				inline void SetBoundsRect(const CRECT &) {}
				inline void SetLeft(const LONG) {}
				inline void SetTop(const LONG) {}
				inline void SetWidth(const LONG) {}
				inline void SetHeight(const LONG) {}
			public:
				CUIToolWindow(void) : CUIBaseWindow() {}
				CUIToolWindow(const HWND hWnd) : CUIBaseWindow(hWnd) {}
				CUIToolWindow(const CUIToolWindow &base) : CUIBaseWindow(base) {}
			};

			class CUIMainWindow : public CUICustomWindow
			{
			private:
				CUIMenu m_MainMenu;
			public:
				void FindToolWindow(void);
			public:
				void SetTextToStatusBar(const uint32_t index, const std::string text);
				void SetTextToStatusBar(const uint32_t index, const std::wstring text);
				static void ProcessMessages(void);
				static INT32 MessageDlg(const std::string message, const std::string caption, const UINT32 flags);
				static INT32 MessageDlg(const std::wstring message, const std::wstring caption, const UINT32 flags);
				static INT32 MessageWarningDlg(const std::string message);
				static INT32 MessageWarningDlg(const std::wstring message);
			public:
				CUIMainWindow(void) : CUICustomWindow() {}
				CUIMainWindow(const HWND hWnd) : CUICustomWindow(hWnd) {}
				CUIMainWindow(const CUIMainWindow &base) : CUICustomWindow(base) {}
			public:
				CUIMenu MainMenu;
				CUIToolWindow Toolbar;
				CUIToolWindow Statusbar;
			};

			class CUICustomDialog : public CUICustomWindow
			{
			private:
				UINT m_ResId;
				DLGPROC m_DlgProc;
				CUICustomWindow* m_Parent;
			public:
				void Create(void);
				void FreeRelease(void);
				inline UINT GetResourceID() const { return m_ResId; }
				inline DLGPROC GetDialogProc() const { return m_DlgProc; }
			public:
				CUICustomDialog(CUICustomWindow* parent, const UINT res_id, DLGPROC dlg_proc);
				virtual ~CUICustomDialog(void);
			public:
				READ_PROPERTY(GetResourceID) UINT ResourceID;
				READ_PROPERTY(GetDialogProc) DLGPROC DialogProc;
			};

			class CUIBaseControl : public CUIBaseWindow
			{
			public:
				CUIBaseControl(void) : CUIBaseWindow() {}
				CUIBaseControl(const HWND hWnd) : CUIBaseWindow(hWnd) {}
				CUIBaseControl(const CUIBaseControl &base) : CUIBaseWindow(base) {}
			};
		}
	}
}
