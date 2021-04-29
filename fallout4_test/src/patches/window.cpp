/*
Author: Perchik71 29/04/2021
This file is part of Fallout 4 Fixes source code.

Adapted for Fallout 4 and Fallout 4 CK
The original
URL: https://github.com/Nukem9/SkyrimSETest/blob/master/skyrim64_test/src/patches/window.cpp
*/

#include "../common.h"
#include <future>

#define WM_APP_THREAD_TASK		(WM_APP + 1)
#define WM_APP_UPDATE_CURSOR	(WM_APP + 2)

static HWND g_Fallout4Window = NULL;
static DWORD MessageThreadId;
static WNDPROC g_OriginalWndProc;
static BOOL MouseEnableInput = TRUE;
static BOOL KeyboardEnableInput = TRUE;
static BOOL OtherEnableInput = TRUE;
static CHAR* KeyBuffer = NULL;

#if FALLOUT4_TRANSLATE_KEY_LOG
#include <fstream>
std::ofstream ofs_key;
#endif


/*
==================
Sys_MouseToggle

Enable/Disable the mouse
==================
*/
VOID FIXAPI Sys_MouseToggle(BOOL bV)
{
	if (bV == MouseEnableInput) 
		return;

	MouseEnableInput = bV;
	if (bV)
	{
		while (ShowCursor(FALSE) >= 0) {}
	}
	else
	{
		while (ShowCursor(TRUE) < 0) {}
	}
}


/*
==================
Sys_KeyboardToggle

Enable/Disable the keyboard
==================
*/
VOID FIXAPI Sys_KeyboardToggle(BOOL bV)
{
	if (bV == MouseEnableInput)
		return;

	KeyboardEnableInput = bV;
}

/*
==================
Sys_OtherDeviceToggle

Enable/Disable the other input device
==================
*/
VOID FIXAPI Sys_OtherDeviceToggle(BOOL bV)
{
	if (bV == OtherEnableInput)
		return;

	OtherEnableInput = bV;
}


/*
==================
Sys_GetKeyboardBuffer

Returns the address of the keyboard input array
==================
*/
BOOL FIXAPI Sys_GetKeyboardBuffer(CHAR* &buffer)
{
	buffer = NULL;
	LPVOID* lpBuffer = (LPVOID*)(OFFSET(0x5A5F668, 0));
	if (!*lpBuffer)
		return FALSE;

	buffer = ((CHAR*(__fastcall*)(LPVOID))OFFSET(0x1B2BFF0, 0))(*lpBuffer) + 0x170;
	return TRUE;
}


/*
==================
WindowProc

==================
*/
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_APP_UPDATE_CURSOR:
	{
		RECT rcClip;
		GetWindowRect(hwnd, &rcClip);

		// 1 pixel of padding
		rcClip.left += 1;
		rcClip.top += 1;

		rcClip.right -= 1;
		rcClip.bottom -= 1;

		ClipCursor(&rcClip);
	}
	return 0;

	// Handle window getting/losing focus (hide/show cursor)
	case WM_ACTIVATEAPP:
	case WM_ACTIVATE:
	case WM_SETFOCUS:
	{
		// Gained focus
		if (((uMsg == WM_ACTIVATEAPP) && (wParam == TRUE)) ||
			((uMsg == WM_ACTIVATE) && (wParam != WA_INACTIVE)) ||
			(uMsg == WM_SETFOCUS))
		{
			if (GetForegroundWindow() == g_Fallout4Window)
			{
				Sys_MouseToggle(TRUE);
				Sys_KeyboardToggle(TRUE);
				Sys_OtherDeviceToggle(TRUE);

				// FIXME: Can throw a grenade :)
				if (Sys_GetKeyboardBuffer(KeyBuffer))
					*(KeyBuffer + 0xA4) = 0;
			}
		}

		// Lost focus
		if ((uMsg == WM_ACTIVATEAPP && wParam == FALSE) ||
			(uMsg == WM_ACTIVATE && wParam == WA_INACTIVE))
		{
			Sys_MouseToggle(FALSE);
			Sys_KeyboardToggle(FALSE);
			Sys_OtherDeviceToggle(FALSE);

			// FIXME: Can throw a grenade :)
			if (Sys_GetKeyboardBuffer(KeyBuffer))
				*(KeyBuffer + 0xA4) = 0;
		}
	}
	return 0;

	// Handle mouse events input
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		return (MouseEnableInput) ? CallWindowProcA(g_OriginalWndProc, hwnd, uMsg, wParam, lParam) : 0;

	// Handle keyboard events input
	case WM_SYSCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_CHAR:
	case WM_KEYUP:
	case WM_KEYDOWN:
		return (KeyboardEnableInput) ? CallWindowProcA(g_OriginalWndProc, hwnd, uMsg, wParam, lParam) : 0;

	// Handle other devices events input  5AA43F0 rbp+170+symbol
	case WM_INPUT:
		return (OtherEnableInput) ? CallWindowProcA(g_OriginalWndProc, hwnd, uMsg, wParam, lParam) : 0;

	default:
		return CallWindowProcA(g_OriginalWndProc, hwnd, uMsg, wParam, lParam);
	}
}

/*
==================
MessageThread

Processing messages in a separate thread
==================
*/
DWORD WINAPI MessageThread(LPVOID)
{
	XUtil::SetThreadName(GetCurrentThreadId(), "Game Message Loop");

	MSG msg;
	while (GetMessageA(&msg, NULL, 0, 0) > 0)
	{
		if (msg.message == WM_APP_THREAD_TASK)
		{
			// Check for hk_CreateWindowExA wanting to execute here
			(*reinterpret_cast<std::packaged_task<HWND()>*>(msg.wParam))();
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		// GetForegroundWindow hack since alt+tab or windows key don't always play nice
		if (msg.message == WM_MOUSEMOVE && msg.hwnd == g_Fallout4Window && msg.hwnd == GetForegroundWindow())
			WindowProc(msg.hwnd, WM_APP_UPDATE_CURSOR, 0, 0);
	}

	// Message loop exited (WM_QUIT) or there was an error
	return 0;
}

/*
==================
hk_CreateWindowExA

Replacement WINAPI CreateWindowExA
==================
*/
HWND WINAPI hk_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, INT x, INT y, INT nWidth, INT nHeight, 
							   HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	// Create this window on a separate thread
	auto threadTask = std::packaged_task<HWND()>([&]()
		{
			HWND wnd = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

			if (wnd && !g_Fallout4Window && !strcmp(lpClassName, "Fallout4"))
			{
				// The original pointer must be saved BEFORE swapping it out
				g_Fallout4Window = wnd;
				// D40190 offset
				g_OriginalWndProc = (WNDPROC)GetWindowLongPtrA(wnd, GWLP_WNDPROC);
				SetWindowLongPtrA(wnd, GWLP_WNDPROC, (LONG_PTR)&WindowProc);
			}

			return wnd;
		});

	// Wait for completion...
	auto taskVar = threadTask.get_future();
	PostThreadMessageA(MessageThreadId, WM_APP_THREAD_TASK, (WPARAM)&threadTask, 0);

	return taskVar.get();
}

VOID FIXAPI Fix_PatchWindow(VOID)
{
#if FALLOUT4_TRANSLATE_KEY_LOG
	ofs_key.open("f4key.log", std::ios::out);
#endif
	
	XUtil::PatchMemoryNop(OFFSET(0xD36D7E, 0), 11);		// GetActiveWindow nop
	XUtil::PatchMemoryNop(OFFSET(0xD38ED6, 0), 5);		// bimbo function clear
	XUtil::PatchMemoryNop(OFFSET(0xD38EE7, 0), 41);		// skip Alt+Tab detect

	XUtil::PatchMemory(OFFSET(0x1D17EE1, 0), { 0xEB }); // No Caption
	XUtil::PatchMemory(OFFSET(0x1D17FE7, 0), { 0xEB }); // No HWND Top
	// XUtil::PatchMemoryNop(OFFSET(0x1D17EBB, 0), 2);		// Get screen width and height

	CreateThread(NULL, 0, MessageThread, NULL, 0, &MessageThreadId);

	PatchIAT(hk_CreateWindowExA, "USER32.DLL", "CreateWindowExA");
}