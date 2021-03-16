#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace ScrollBar
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawBackgroundHorz(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawBackgroundVert(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSliderHorz_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSliderHorz_Focused(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSliderVert_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSliderVert_Focused(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}
			}
		}
	}
}