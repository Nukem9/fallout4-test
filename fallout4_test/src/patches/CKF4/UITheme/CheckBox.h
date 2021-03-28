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

					VOID WINAPI DrawIdeterminate_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clColor);
					VOID WINAPI DrawIdeterminate_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawIdeterminate_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawIdeterminate_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawIdeterminate_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}

				namespace Event
				{
					VOID WINAPI OnBeforeDrawText(Graphics::CUICanvas& canvas, DWORD& flags, INT iStateId);
				}
			}
		}
	}
}