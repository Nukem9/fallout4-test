#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace CheckBox
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawCheck_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clShadow, COLORREF clColor);
					VOID WINAPI DrawCheck_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawCheck_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawCheck_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawCheck_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}
			}
		}
	}
}