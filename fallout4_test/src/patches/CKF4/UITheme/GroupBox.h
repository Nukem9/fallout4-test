#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace GroupBox
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawGroupBox_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clColor, COLORREF clColorShadow);
					VOID WINAPI DrawGroupBox_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawGroupBox_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}

				namespace Event
				{
					VOID WINAPI OnBeforeDrawText(Graphics::CUICanvas& canvas, DWORD& flags);
				}
			}
		}
	}
}