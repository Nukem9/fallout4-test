#pragma once

#include "../../common.h"
#include <Uxtheme.h>

namespace EditorUIDarkMode
{
	enum class ThemeType
	{
		None,
		ScrollBar,
		StatusBar,
		MDIClient,
		Static,
		Edit,
		RichEdit,
		Button,
		ComboBox,
		Header,
		ListBox,
		ListView,
		TreeView,
		TabControl,
		ToolBar,
		TrackBar,
		ProgressBar,
		PopupMenu,
		Spin
	};

	VOID Initialize(VOID);
	VOID InitializeThread(VOID);
	BOOL IsUIDarkMode(VOID);

	// Returns a valid visual theme type, depending on the window class
	ThemeType WINAPI GetThemeTypeFromWindow(HWND hWindow);
	// Binds the specified class type to the visual theme. hWindow takes only HTHEME
	// Returns TRUE if successful
	BOOL WINAPI RegisterThemeHandle(HWND hWindow, ThemeType eTheme);
	// Binds the specified class type to the visual theme
	// Returns TRUE if successful
	BOOL WINAPI RegisterThemeHandle(HTHEME hTheme, ThemeType eTheme);

	HWND WINAPI Comctl32CreateToolbarEx_1(HWND hwnd, DWORD ws, UINT wID, INT nBitmaps, HINSTANCE hBMInst, UINT_PTR wBMID, LPCTBBUTTON lpButtons,
		INT iNumButtons, INT dxButton, INT dyButton, INT dxBitmap, INT dyBitmap, UINT uStructSize);
	HIMAGELIST WINAPI Comctl32ImageList_LoadImageA_1(HINSTANCE hi, LPCSTR lpbmp, INT cx, INT cGrow, COLORREF crMask, UINT uType, UINT uFlags);
	HWND WINAPI Comctl32CreateWindowEx_1(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, INT x, INT y,
		INT nWidth, INT nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	VOID WINAPI HideOldTimeOfDayComponents(VOID);

	LRESULT CALLBACK CallWndProcCallback(INT nCode, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	DWORD WINAPI Comctl32GetSysColor(INT nIndex);
	HBRUSH WINAPI Comctl32GetSysColorBrush(INT nIndex);
	HRESULT WINAPI Comctl32DrawThemeText(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCWSTR pszText, INT cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect);
	HRESULT WINAPI Comctl32DrawThemeBackground(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect);
}