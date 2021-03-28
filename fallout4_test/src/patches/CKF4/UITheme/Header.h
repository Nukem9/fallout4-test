#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace Header
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawBack_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, 
						COLORREF clGradientColorStart, COLORREF clGradientColorEnd, COLORREF cLighter);
					VOID WINAPI DrawBack_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawBack_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawBack_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawBack_Selected(Graphics::CUICanvas& canvas, LPCRECT pRect);

					VOID WINAPI DrawDiv(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}
			}
		}
	}
}