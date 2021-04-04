#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace ComboBox
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawArrow_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF cColor, COLORREF cShadowColor);
					VOID WINAPI DrawArrow_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawArrow_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}

				namespace Event
				{
					VOID WINAPI OnBeforeDrawText(Graphics::CUICanvas& canvas, DWORD& flags, INT iStateId);
				}
			}
		}
	}
}