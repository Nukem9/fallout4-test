#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace PageControl
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawBorder(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColor);
					VOID WINAPI DrawButton_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_SelectedAndFocused(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}

				namespace Event
				{
					VOID WINAPI OnBeforeDrawText(Graphics::CUICanvas& canvas, DWORD& flags);
				}

				VOID WINAPI Initialize(HWND hWnd);
			}
		}
	}
}