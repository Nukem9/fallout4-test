#pragma once

#include "..\UIGraphics.h"

#include <Uxtheme.h>

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace Memo
			{
				HTHEME WINAPI Initialize(HWND hWindow);
				LRESULT CALLBACK MemoSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
			}
		}
	}
}