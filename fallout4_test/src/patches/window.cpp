// https://github.com/Nukem9/SkyrimSETest/blob/master/skyrim64_test/src/patches/window.cpp

#include "../common.h"
#include <future>

#define WM_APP_THREAD_TASK		(WM_APP + 1)
#define WM_APP_UPDATE_CURSOR	(WM_APP + 2)

HWND g_Fallout4Window = NULL;
WNDPROC g_OriginalWndProc;
DWORD MessageThreadId;
BOOL EnableInput = TRUE;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_APP_UPDATE_CURSOR)
	{
		RECT rcClip;
		GetWindowRect(hwnd, &rcClip);

		// 1 pixel of padding
		rcClip.left += 1;
		rcClip.top += 1;

		rcClip.right -= 1;
		rcClip.bottom -= 1;

		ClipCursor(&rcClip);

		return 0;
	}

	// Handle window getting/losing focus (hide/show cursor)
	if (uMsg == WM_ACTIVATEAPP || uMsg == WM_ACTIVATE || uMsg == WM_SETFOCUS)
	{
		// Gained focus
		if (((uMsg == WM_ACTIVATEAPP) && (wParam == TRUE)) ||
			((uMsg == WM_ACTIVATE) && (wParam != WA_INACTIVE)) ||
			(uMsg == WM_SETFOCUS))
		{
			while (ShowCursor(FALSE) >= 0) {}

			if (GetForegroundWindow() == g_Fallout4Window)
				EnableInput = TRUE;
		}

		// Lost focus
		if ((uMsg == WM_ACTIVATEAPP && wParam == FALSE) ||
			(uMsg == WM_ACTIVATE && wParam == WA_INACTIVE))
		{
			while (ShowCursor(TRUE) < 0) {}

			EnableInput = FALSE;
		}

		return 0;
	}

	// Always-forwarded game wndproc commands
	switch (uMsg)
	{
	case WM_SYSCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_CHAR:
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		return (EnableInput) ? CallWindowProcA(g_OriginalWndProc, hwnd, uMsg, wParam, lParam) : 0;
	default:
		return CallWindowProcA(g_OriginalWndProc, hwnd, uMsg, wParam, lParam);
	}
}

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

VOID PatchWindow()
{
	XUtil::PatchMemory(OFFSET(0x1D17EE1, 0), { 0xEB }); // No Caption
	XUtil::PatchMemory(OFFSET(0x1D17FE7, 0), { 0xEB }); // No HWND Top
	//XUtil::PatchMemoryNop(OFFSET(0x1D17EBB, 0), 2); // Get screen width and height

	CreateThread(NULL, 0, MessageThread, NULL, 0, &MessageThreadId);

	PatchIAT(hk_CreateWindowExA, "USER32.DLL", "CreateWindowExA");
}