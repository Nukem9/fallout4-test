#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace RadioButton
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawPushButtonR_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect,
						COLORREF clGradientColorStart, COLORREF clGradientColorEnd,
						COLORREF clGradientHighlighterColorStart, COLORREF clGradientHighlighterColorEnd,
						COLORREF clDividerColor, COLORREF clDividerHighlighterColor);
					VOID WINAPI DrawPushButtonR_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawPushButtonR_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawPushButtonR_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawPushButtonR_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);

					VOID WINAPI DrawRadioButton_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clColor);
					VOID WINAPI DrawRadioButton_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawRadioButton_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawRadioButton_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawRadioButton_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}

				namespace Event
				{
					VOID WINAPI OnBeforeDrawText(Graphics::CUICanvas& canvas, DWORD& flags, INT iStateId);
				}
			}
		}
	}
}