#pragma once

#include "..\UIGraphics.h"

#include <Uxtheme.h>

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace ListBox
			{
				HTHEME WINAPI Initialize(HWND hWindow);
				LRESULT CALLBACK ListBoxSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

				INT_PTR WINAPI OnCtlColorListBox(HWND hWindow, HDC hDC);
			}
		}
	}
}