#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace UpDown
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawUp_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColorBorder, 
						COLORREF cColorStart, COLORREF cColorEnd, COLORREF cColor, COLORREF cShadowColor);
					VOID WINAPI DrawUp_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawUp_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawUp_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawUp_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawDown_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColorBorder, 
						COLORREF cColorStart, COLORREF cColorEnd, COLORREF cColor, COLORREF cShadowColor);
					VOID WINAPI DrawDown_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawDown_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawDown_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawDown_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);

					VOID WINAPI DrawLeft_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColorBorder,
						COLORREF cColorStart, COLORREF cColorEnd, COLORREF cColor, COLORREF cShadowColor, 
						COLORREF cHihglighter, BOOL drawHihglighter);
					VOID WINAPI DrawLeft_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawLeft_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawLeft_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawLeft_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawRight_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColorBorder,
						COLORREF cColorStart, COLORREF cColorEnd, COLORREF cColor, COLORREF cShadowColor, 
						COLORREF cHihglighter, BOOL drawHihglighter);
					VOID WINAPI DrawRight_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawRight_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawRight_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawRight_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}

				VOID WINAPI Initialize(HWND hWindow);
				LRESULT CALLBACK UpDownSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
			}
		}
	}
}