#pragma once

#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace TrackBar
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawTrack(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSlider_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSlider_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSlider_Focused(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSlider_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawSlider_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
				}
			}
		}
	}
}