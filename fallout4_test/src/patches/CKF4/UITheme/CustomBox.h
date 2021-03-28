#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace CustomBox
			{
				namespace Graphics = Core::Classes::UI;

				enum AllowBox
				{
					abNormal,
					abColor
				};

				namespace Render
				{
					VOID WINAPI DrawBorder(Graphics::CUICanvas& canvas, Graphics::CRECT& rc);
				}

				VOID WINAPI Initialize(HWND hWindow, AllowBox eAllowBox);

				LRESULT CALLBACK NormalBoxSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
				LRESULT CALLBACK ColorBoxSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
			}
		}
	}
}