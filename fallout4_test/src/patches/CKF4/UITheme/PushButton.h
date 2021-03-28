#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace PushButton
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawPushButton_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, 
						COLORREF clGradientColorStart, COLORREF clGradientColorEnd,
						COLORREF clGradientHighlighterColorStart, COLORREF clGradientHighlighterColorEnd,
						COLORREF clDividerColor, COLORREF clDividerHighlighterColor);
					VOID WINAPI DrawPushButton_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawPushButton_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawPushButton_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawPushButton_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}

				namespace Event
				{
					VOID WINAPI OnBeforeDrawText(Graphics::CUICanvas& canvas, DWORD& flags, INT iStateId);
				}
			}
		}
	}
}