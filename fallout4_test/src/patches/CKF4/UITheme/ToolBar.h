#pragma once

#include <Uxtheme.h>
#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace ToolBar
			{
				namespace Graphics = Core::Classes::UI;

				namespace Render
				{
					VOID WINAPI DrawBackground(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Checked(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect);
					VOID WINAPI DrawButton_Icon(Graphics::CUICanvas& canvas, LPCRECT pRect, HIMAGELIST hImageList, INT nIndex);
				}

				VOID WINAPI Initialize(HWND hWindow);

				LRESULT WINAPI OnCustomDraw(HWND hWindow, LPNMTBCUSTOMDRAW lpToolBar);
			}
		}
	}
}