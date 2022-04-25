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

#include "..\..\[Common]\StdAfx.h"

#define WM_APP_UPDATE_CURSOR (WM_APP + 0x9A36)

static HWND g_Fallout4Window = NULL;
static WNDPROC g_OriginalWndProc;

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
		if (g_Fallout4Window != GetActiveWindow())
		{
			ClipCursor(NULL);
			return 0;
		}
		
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
	// Handle mouse events input
	case WM_MOUSEMOVE:
		if (g_Fallout4Window == hwnd)
			PostMessageA(hwnd, WM_APP_UPDATE_CURSOR, 0, 0);
	}

	return CallWindowProcA(g_OriginalWndProc, hwnd, uMsg, wParam, lParam);
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
	HWND wnd = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (wnd && !g_Fallout4Window && !strcmp(lpClassName, "Fallout4"))
		// The original pointer must be saved BEFORE swapping it out
		g_Fallout4Window = wnd;

	return wnd;
}


/*
==================
Fix_PatchWindow

Game window patch
==================
*/
VOID FIXAPI Fix_PatchWindow(VOID)
{	
	*(uintptr_t*)&g_OriginalWndProc = Detours::X64::DetourFunctionClass(OFFSET(0xD40190, 0), &WindowProc);

	XUtil::PatchMemoryNop(OFFSET(0xD38ED6, 0), 5);		// bimbo function clear
	XUtil::PatchMemoryNop(OFFSET(0xD38EE7, 0), 41);		// skip Alt+Tab detect
	XUtil::PatchMemory(OFFSET(0x1D17EE1, 0), { 0xEB }); // No Caption
	XUtil::PatchMemory(OFFSET(0x1D17FE7, 0), { 0xEB }); // No HWND Top

	PatchIAT(hk_CreateWindowExA, "USER32.DLL", "CreateWindowExA");
}