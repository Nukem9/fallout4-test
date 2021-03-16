#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace StatusBar
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawBorder(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawBackground(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}
			}
		}
	}
}